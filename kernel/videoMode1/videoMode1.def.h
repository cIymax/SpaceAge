/*
 *  Uzebox(tm) Video Mode 1
 *  Copyright (C) 20098  Alec Bourque
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

/** 
 * ==============================================================================
 *
 * This file contains global defines for video mode 1
 *
 * ===============================================================================
 */
#pragma once

#define VMODE_ASM_SOURCE "videoMode1/videoMode1.s"
#define VMODE_C_SOURCE "videoMode1/videoMode1.c"
#define VMODE_C_PROTOTYPES "videoMode1/videoMode1.h"
#define VMODE_FUNC sub_video_mode1

#define TILE_HEIGHT 8
#define TILE_WIDTH 6	
#define VRAM_TILES_H 40 
#ifndef VRAM_TILES_V
	#define VRAM_TILES_V 28
#endif
#define SCREEN_TILES_H 40
#define SCREEN_TILES_V 28 //28
#define FIRST_RENDER_LINE 20
#define VRAM_SIZE VRAM_TILES_H*VRAM_TILES_V*2
#define VRAM_ADDR_SIZE 2 //in bytes
#define SPRITES_ENABLED 0

#ifndef FRAME_LINES
	#define FRAME_LINES SCREEN_TILES_V*TILE_HEIGHT
#endif

//In mode 1, vsync flag is set only each frame (30Hz)
//set to MODE1_FAST_VSYNC=1 to sync on fields (60Hz) like other video modes
#ifndef MODE1_FAST_VSYNC
	#define MODE1_FAST_VSYNC 0
#endif


