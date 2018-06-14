/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software  Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2017, Blender Foundation
 * This is a new part of Blender
 *
 * Contributor(s): Antonio Vazquez
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 */

/** \file blender/modifiers/intern/MOD_gpencillattice.c
 *  \ingroup modifiers
 */

#include <stdio.h>

#include "DNA_meshdata_types.h"
#include "DNA_scene_types.h"
#include "DNA_object_types.h"
#include "DNA_gpencil_types.h"

#include "BLI_utildefines.h"

#include "BKE_context.h"
#include "BKE_deform.h"
#include "BKE_gpencil.h"
#include "BKE_lattice.h"
#include "BKE_library_query.h"
#include "BKE_scene.h"
#include "BKE_main.h"
#include "BKE_layer.h"

#include "MEM_guardedalloc.h"

#include "MOD_modifiertypes.h"
#include "MOD_gpencil_util.h"

#include "DEG_depsgraph.h"
#include "DEG_depsgraph_build.h"

static void initData(ModifierData *md)
{
	LatticeGreasePencilModifierData *gpmd = (LatticeGreasePencilModifierData *)md;
	gpmd->pass_index = 0;
	gpmd->layername[0] = '\0';
	gpmd->vgname[0] = '\0';
	gpmd->object = NULL;
	gpmd->cache_data = NULL;
	gpmd->strength = 1.0f;
}

static void copyData(const ModifierData *md, ModifierData *target)
{
	modifier_copyData_generic(md, target);
}

static void gp_deformStroke(
        ModifierData *md, Depsgraph *UNUSED(depsgraph),
        Object *ob, bGPDlayer *gpl, bGPDstroke *gps)
{
	LatticeGreasePencilModifierData *mmd = (LatticeGreasePencilModifierData *)md;
	int vindex = defgroup_name_index(ob, mmd->vgname);
	float weight = 1.0f;

	if (!is_stroke_affected_by_modifier(ob, 
	        mmd->layername, mmd->pass_index, 3, gpl, gps,
	        mmd->flag & GP_LATTICE_INVERT_LAYER, mmd->flag & GP_LATTICE_INVERT_PASS))
	{
		return;
	}

	if (mmd->cache_data == NULL) {
		return;
	}

	for (int i = 0; i < gps->totpoints; i++) {
		bGPDspoint *pt = &gps->points[i];
		MDeformVert *dvert = &gps->dvert[i];

		/* verify vertex group */
		weight = get_modifier_point_weight(dvert, (int)(!(mmd->flag & GP_LATTICE_INVERT_VGROUP) == 0), vindex);
		if (weight < 0) {
			continue;
		}

		calc_latt_deform((struct LatticeDeformData *)mmd->cache_data, &pt->x, mmd->strength * weight);
	}
}

/* FIXME: Ideally we be doing this on a copy of the main depsgraph
 * (i.e. one where we don't have to worry about restoring state)
 */
static void gp_bakeModifier(
		Main *bmain, Depsgraph *depsgraph,
        ModifierData *md, Object *ob)
{
	LatticeGreasePencilModifierData *mmd = (LatticeGreasePencilModifierData *)md;
	Scene *scene = md->scene;
	struct LatticeDeformData *ldata = NULL;
	bGPdata *gpd = ob->data;
	int oldframe = CFRA;

	if (mmd->object == NULL)
		return;

	for (bGPDlayer *gpl = gpd->layers.first; gpl; gpl = gpl->next) {
		for (bGPDframe *gpf = gpl->frames.first; gpf; gpf = gpf->next) {
			/* apply lattice effects on this frame
			 * NOTE: this assumes that we don't want lattice animation on non-keyframed frames
			 */
			CFRA = gpf->framenum;
			BKE_scene_graph_update_for_newframe(depsgraph, bmain);
			
			/* recalculate lattice data */
			BKE_gpencil_lattice_init(ob);
			
			/* compute lattice effects on this frame */
			for (bGPDstroke *gps = gpf->strokes.first; gps; gps = gps->next) {
				gp_deformStroke(md, depsgraph, ob, gpl, gps);
			}
		}
	}
	
	/* free lingering data */
	ldata = (struct LatticeDeformData *)mmd->cache_data;
	if (ldata) {
		end_latt_deform(ldata);
		mmd->cache_data = NULL;
	}

	/* return frame state and DB to original state */
	CFRA = oldframe;
	BKE_scene_graph_update_for_newframe(depsgraph, bmain);
}

static void freeData(ModifierData *md)
{
	LatticeGreasePencilModifierData *mmd = (LatticeGreasePencilModifierData *)md;
	struct LatticeDeformData *ldata = (struct LatticeDeformData *)mmd->cache_data;
	/* free deform data */
	if (ldata) {
		end_latt_deform(ldata);
	}
}

static bool isDisabled(ModifierData *md, int UNUSED(userRenderParams))
{
	LatticeGreasePencilModifierData *mmd = (LatticeGreasePencilModifierData *)md;

	return !mmd->object;
}

static void updateDepsgraph(ModifierData *md, const ModifierUpdateDepsgraphContext *ctx) 
{
	LatticeGreasePencilModifierData *lmd = (LatticeGreasePencilModifierData *)md;
	if (lmd->object != NULL) {
		DEG_add_object_relation(ctx->node, lmd->object, DEG_OB_COMP_GEOMETRY, "Lattice Modifier");
		DEG_add_object_relation(ctx->node, lmd->object, DEG_OB_COMP_TRANSFORM, "Lattice Modifier");
	}
	DEG_add_object_relation(ctx->node, ctx->object, DEG_OB_COMP_TRANSFORM, "Lattice Modifier");
}

static void foreachObjectLink(
        ModifierData *md, Object *ob,
        ObjectWalkFunc walk, void *userData)
{
	LatticeGreasePencilModifierData *mmd = (LatticeGreasePencilModifierData *)md;

	walk(userData, ob, &mmd->object, IDWALK_CB_NOP);
}

ModifierTypeInfo modifierType_Gpencil_Lattice = {
	/* name */              "Lattice",
	/* structName */        "LatticeGreasePencilModifierData",
	/* structSize */        sizeof(LatticeGreasePencilModifierData),
	/* type */              eModifierTypeType_Gpencil,
	/* flags */             eModifierTypeFlag_GpencilMod | eModifierTypeFlag_Single | eModifierTypeFlag_SupportsEditmode,

	/* copyData */          copyData,

	/* deformVerts_DM */    NULL,
	/* deformMatrices_DM */ NULL,
	/* deformVertsEM_DM */  NULL,
	/* deformMatricesEM_DM*/NULL,
	/* applyModifier_DM */  NULL,
	/* applyModifierEM_DM */NULL,

	/* deformVerts */       NULL,
	/* deformMatrices */    NULL,
	/* deformVertsEM */     NULL,
	/* deformMatricesEM */  NULL,
	/* applyModifier */     NULL,
	/* applyModifierEM */   NULL,

	/* gp_deformStroke */      gp_deformStroke,
	/* gp_generateStrokes */   NULL,
	/* gp_bakeModifier */    gp_bakeModifier,

	/* initData */          initData,
	/* requiredDataMask */  NULL,
	/* freeData */          freeData,
	/* isDisabled */        isDisabled,
	/* updateDepsgraph */   updateDepsgraph,
	/* dependsOnTime */     NULL,
	/* dependsOnNormals */	NULL,
	/* foreachObjectLink */ foreachObjectLink,
	/* foreachIDLink */     NULL,
	/* foreachTexLink */    NULL,
};