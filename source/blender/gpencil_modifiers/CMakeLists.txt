# ***** BEGIN GPL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# The Original Code is Copyright (C) 2006, Blender Foundation
# All rights reserved.
#
# The Original Code is: all of this file.
#
# ***** END GPL LICENSE BLOCK *****

set(INC 
	.
	intern
	../blenkernel
	../blenlib
	../blenfont
	../depsgraph
	../makesdna
	../makesrna
	../render/extern/include
	../../../intern/guardedalloc
	../../../intern/eigen
)

set(INC_SYS
	${ZLIB_INCLUDE_DIRS}
)

set(SRC
	intern/MOD_gpencil_build.c
	intern/MOD_gpencil_color.c
	intern/MOD_gpencil_hook.c
	intern/MOD_gpencil_instance.c
	intern/MOD_gpencil_lattice.c
	intern/MOD_gpencil_noise.c
	intern/MOD_gpencil_offset.c
	intern/MOD_gpencil_opacity.c
	intern/MOD_gpencil_simplify.c
	intern/MOD_gpencil_smooth.c
	intern/MOD_gpencil_subdiv.c
	intern/MOD_gpencil_thick.c
	intern/MOD_gpencil_tint.c
	intern/MOD_gpencil_util.c

	MOD_gpencil_modifiertypes.h
	intern/MOD_gpencil_util.h
)

if(WITH_INTERNATIONAL)
	add_definitions(-DWITH_INTERNATIONAL)
endif()

# So we can have special tricks in modifier system.
add_definitions(${GL_DEFINITIONS})

blender_add_lib(bf_gpencil_modifiers "${SRC}" "${INC}" "${INC_SYS}")
