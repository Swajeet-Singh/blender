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
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file MOD_modifiertypes.h
 *  \ingroup modifiers
 */

#ifndef __MOD_GREASEPENCIL_MODIFIERTYPES_H__
#define __MOD_GREASEPENCIL_MODIFIERTYPES_H__

#include "BKE_gpencil_modifier.h"

/* ****************** Type structures for all modifiers ****************** */

extern GreasePencilModifierTypeInfo GreasePencilModifierType_Noise;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Subdiv;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Simplify;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Thick;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Tint;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Color;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Instance;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Build;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Opacity;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Lattice;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Smooth;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Hook;
extern GreasePencilModifierTypeInfo GreasePencilModifierType_Offset;

/* MOD_gpencil_util.c */
void greasepencil_modifier_type_init(GreasePencilModifierTypeInfo *types[]);

#endif  /* __MOD_GREASEPENCIL_MODIFIERTYPES_H__ */
