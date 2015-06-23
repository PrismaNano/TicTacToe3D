#include "DSGM.h"

#include "DSGM_projectHelper.h"

#define BLUE 1
#define RED  2

// User variables / declarations
int angle_1, angle_2;
int rotation, rotateboard, RotVelocity;
bool touch;
float x, y, z, a, b;
int frames;
int layer;
float View_Y, View_X;
bool Debug;
bool zoom;
bool PieceTouched;

typedef struct {
	bool InUse;
	int color;
} BoardData;

BoardData Board[3][3][3];

typedef struct {
	int ID;
	float X, Y;
	int Rotation;
} ModelData;

ModelData Model[10];

bool Row_Win(void) {
	if(
		(Board[0][0][0].color == BLUE &&
		Board[1][1][1].color == BLUE &&
		Board[2][2][2].color == BLUE) ||
		
		(Board[0][2][2].color == BLUE &&
		Board[1][1][1].color == BLUE &&
		Board[2][0][0].color == BLUE) ||
		
		(Board[0][2][0].color == BLUE &&
		Board[1][1][1].color == BLUE &&
		Board[2][0][2].color == BLUE) ||
		
		(Board[0][0][2].color == BLUE &&
		Board[1][1][1].color == BLUE &&
		Board[2][2][0].color == BLUE)
	) return true;
	
	int i, layer;
	for(i = 0; i < 3; i++) {
		if(
			(Board[0][0][i].color == BLUE &&
			Board[1][1][i].color == BLUE &&
			Board[2][2][i].color == BLUE) ||
			
			(Board[0][2][i].color == BLUE &&
			Board[1][1][i].color == BLUE &&
			Board[2][0][i].color == BLUE) ||
			
			(Board[0][i][0].color == BLUE &&
			Board[1][i][1].color == BLUE &&
			Board[2][i][2].color == BLUE) ||
			
			(Board[0][i][2].color == BLUE &&
			Board[1][i][1].color == BLUE &&
			Board[2][i][0].color == BLUE)
		) return true;
		
		for(layer = 0; layer < 3; layer++) {
			if(
				(Board[layer][0][i].color == BLUE &&
				Board[layer][1][i].color == BLUE &&
				Board[layer][2][i].color == BLUE) ||
				
				(Board[layer][i][0].color == BLUE &&
				Board[layer][i][1].color == BLUE &&
				Board[layer][i][2].color == BLUE) ||
				
				(Board[layer][0][0].color == BLUE &&
				Board[layer][1][1].color == BLUE &&
				Board[layer][2][2].color == BLUE) ||
				
				(Board[layer][2][0].color == BLUE &&
				Board[layer][1][1].color == BLUE &&
				Board[layer][0][2].color == BLUE) ||
				
				(Board[0][i][layer].color == BLUE &&
				Board[1][i][layer].color == BLUE &&
				Board[2][i][layer].color == BLUE)
			
			) return true;
		}
	}
	
	return false;
}

int DegreesToRadians(int degrees) {
	return degrees * M_PI / 180;
}

void reset_board(void) {
	/*
	int i, j, k;
	for(i = 0; i < 3; i++) for(j = 0; j < 3; j++) for(k = 0; k < 3; k++) {
		Board[i][j][k].InUse = 0;
		Board[i][j][k].color = 0;
	}
	*/
	
	memset(Board, 0, sizeof(Board));
}

DSGM_Camera camera;

DSGM_Model BoardModel;
DSGM_Texture BoardTexture = DSGM_FORM_RAM_TEXTURE(GL_RGB, TEXTURE_SIZE_128, TEXTURE_SIZE_128, BoardLight_Texture_bin);

DSGM_Model PieceBlueModel;
DSGM_Model PiecePurpleModel;
DSGM_Texture PieceBlueTexture = DSGM_FORM_RAM_TEXTURE(GL_RGB, TEXTURE_SIZE_128, TEXTURE_SIZE_128, Piece_Blue_Texture_bin);
DSGM_Texture PieceRedTexture = DSGM_FORM_RAM_TEXTURE(GL_RGB, TEXTURE_SIZE_128, TEXTURE_SIZE_128, Piece_Red_Texture_bin);

DSGM_Model TableTopModel;
DSGM_Texture TableTopTexture = DSGM_FORM_RAM_TEXTURE(GL_RGB, TEXTURE_SIZE_128, TEXTURE_SIZE_128, TableTop_Texture_bin);

// Resources
DSGM_Sound DSGM_Sounds[DSGM_SOUND_COUNT] = {
};

DSGM_Background DSGM_Backgrounds[DSGM_BACKGROUND_COUNT] = {
	DSGM_FORM_NITRO_BACKGROUND(Background_Coffee, BgSize_T_256x256, BgType_Text8bpp),
	DSGM_FORM_NITRO_BACKGROUND(BGCoffeeBottom, BgSize_T_256x256, BgType_Text8bpp),
	DSGM_FORM_NITRO_BACKGROUND(RotBoard, BgSize_ER_256x256, BgType_ExRotation),
	DSGM_FORM_NITRO_BACKGROUND(BGCoffee1, BgSize_T_256x256, BgType_Text8bpp),
	DSGM_FORM_NITRO_BACKGROUND(BG1, BgSize_T_512x512, BgType_Text8bpp),
};

DSGM_Palette DSGM_Palettes[DSGM_PALETTE_COUNT] = {
	DSGM_FORM_NITRO_PALETTE(Pieces),
	DSGM_FORM_NITRO_PALETTE(Slider),
	DSGM_FORM_NITRO_PALETTE(Layer_Buttons),
	DSGM_FORM_NITRO_PALETTE(Arrow),
};

DSGM_Sprite DSGM_Sprites[DSGM_SPRITE_COUNT] = {
	DSGM_FORM_NITRO_SPRITE(Pieces, PiecePal, SpriteSize_32x32, 3),
	DSGM_FORM_NITRO_SPRITE(Slider, SliderPal, SpriteSize_32x16, 2),
	DSGM_FORM_NITRO_SPRITE(Layer_Buttons, LayerPal, SpriteSize_32x64, 6),
	DSGM_FORM_NITRO_SPRITE(Arrow, ArrowPal, SpriteSize_16x16, 4),
};

DSGM_Object DSGM_Objects[DSGM_OBJECT_COUNT] = {
	// Renderer
	{
		DSGM_NO_SPRITE,
		(DSGM_Event)renderer_create,
		(DSGM_Event)renderer_loop,
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((rendererObjectInstance *)0)->variables)
	},
	
	// Debugger
	{
		DSGM_NO_SPRITE,
		DSGM_NO_EVENT,
		(DSGM_Event)Debugger_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((DebuggerObjectInstance *)0)->variables)
	},
	
	// Blue Piece
	{
		DSGM_NO_SPRITE,
		(DSGM_Event)Piece_Blue_create,
		(DSGM_Event)Piece_Blue_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((PieceBlueObjectInstance *)0)->variables)
	},
	
	// Red Piece
	{
		DSGM_NO_SPRITE,
		(DSGM_Event)Piece_Red_create,
		(DSGM_Event)Piece_Red_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((PieceRedObjectInstance *)0)->variables)
	},

	//Pieces
	{
		&DSGM_Sprites[Pieces_Spr],
		(DSGM_Event)Piece_create,
		(DSGM_Event)Piece_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((PieceObjectInstance *)0)->variables)
	},
		
	//Slider
	{
		&DSGM_Sprites[Slider_Spr],
		DSGM_NO_EVENT,
		(DSGM_Event)Slider_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((SliderObjectInstance *)0)->variables)
	},
	
	//Layer 1 button
	{
		&DSGM_Sprites[Layers_Spr],
		(DSGM_Event)Layer_1_create,
		(DSGM_Event)Layer_1_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((Layer1ObjectInstance *)0)->variables)
	},
	
	//Layer 2 button
	{
		&DSGM_Sprites[Layers_Spr],
		(DSGM_Event)Layer_2_create,
		(DSGM_Event)Layer_2_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((Layer2ObjectInstance *)0)->variables)
	},
	
	//Layer 3 button
	{
		&DSGM_Sprites[Layers_Spr],
		(DSGM_Event)Layer_3_create,
		(DSGM_Event)Layer_3_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((Layer3ObjectInstance *)0)->variables)
	},
	
	//Arrow
	{
		&DSGM_Sprites[Arrow_Spr],
		(DSGM_Event)Arrow_create,
		(DSGM_Event)Arrow_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((ArrowObjectInstance *)0)->variables)
	},
	
	//Piece that you place on the board when its your turn
	{
		&DSGM_Sprites[Pieces_Spr],
		(DSGM_Event)PieceTemp_create,
		(DSGM_Event)PieceTemp_loop,	
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((PieceTempObjectInstance *)0)->variables)
	},
	// Object_Example
	/*{
		&DSGM_Sprites[Sprite],
		(DSGM_Event)Object_create,
		(DSGM_Event)Object_loop,
		DSGM_NO_EVENT,
		DSGM_NO_EVENT,
		NULL, 0,
		
		sizeof(*((ObjectInstance *)0)->variables)
	},
	*/
};

DSGM_Room DSGM_Rooms[DSGM_ROOM_COUNT] = {
	// Room_1
	{
		// Backgrounds
		{
			// Bottom screen
			{
				// Layer 0
				{
					&DSGM_Backgrounds[BGCoffee1],//Background
					DSGM_BOTTOM,				// Screen
					0,							// Layer
					false,						// Attached to view system
					17,							// Map base
					1,							// Tile base
					0, 0, 0
				},
				
				// Layer 1
				{
					&DSGM_Backgrounds[BGCoffeeBottom],// Background
					DSGM_BOTTOM,				// Screen
					1,							// Layer
					true,						// Attached to view system
					5,							// Map base
					3,							// Tile base
					0, 0, 0
				},
				
				// Layer 2
				{
					DSGM_NO_BACKGROUND,			// Background
					DSGM_BOTTOM,				// Screen
					2,							// Layer
					true,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
				
				// Layer 3
				{
					&DSGM_Backgrounds[RotBoard],// Background
					DSGM_BOTTOM,				// Screen
					3,							// Layer
					true,						// Attached to view system
					16,							// Map base
					8,							// Tile base
					0, 0, 0
				},
			},
			
			// Top screen
			{
				// Only top screen layer 0 can be a 3D renderer
				// Layer 0
				{
					DSGM_3D_RENDERER,			// Background
					DSGM_TOP,					// Screen
					0,							// Layer
					false,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
				
				// Layer 1
				{
					DSGM_DEFAULT_FONT,			// Background
					DSGM_TOP,					// Screen
					1,							// Layer
					false,						// Attached to view system
					0,							// Map base
					1,							// Tile base
					0, 0, 0
				},
				
				// Layer 2
				{
					&DSGM_Backgrounds[Backdrop],// Background
					DSGM_TOP,					// Screen
					2,							// Layer
					true,						// Attached to view system
					1,							// Map base
					2,							// Tile base
					0, 0, 0
				},
				
				// Layer 3
				{
					DSGM_NO_BACKGROUND,			// Background
					DSGM_TOP,					// Screen
					3,							// Layer
					true,						// Attached to view system
					0,							// Map base
					0,							// Tile base
					0, 0, 0
				},
			}
		},
		
		// Initial views
		{
			// Bottom screen
			{
				0, 0
			},
			
			// Top screen
			{
				0, 0
			}
		},
		
		// Views
		{
			// Bottom screen
			{
				0, 0
			},
			
			// Top screen
			{
				0, 0
			}
		},
		
		NULL,
		
		// Object groups are dynamic, so must be set up at run time, see DSGM_SetupRooms.
		{
			NULL,
			NULL
		},
		{
			0,
			0
		}
	},
};

int DSGM_currentRoom = Room_1;

void DSGM_SetupRooms(int room) {
	if(room != DSGM_ALL_ROOMS) {
		switch(room) {
			case Room_1: goto Room_1; break;
		}
	}
	
	Room_1:
	DSGM_Debug("Room_1 reset\n");
	DSGM_LeaveRoom(&DSGM_Rooms[Room_1]);
	
	DSGM_SetupViews(&DSGM_Rooms[Room_1]);
	
	DSGM_SetupObjectGroups(&DSGM_Rooms[Room_1], DSGM_TOP, 2); //Don't forget to update number of object groups
	
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_TOP][0], &DSGM_Objects[renderer], DSGM_TOP, 1, 0, 0);
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_TOP][1], &DSGM_Objects[Debugger], DSGM_TOP, 1, 0, 0);
	
	//DSGM_SetupObjectInstances(&DSGM_Rooms[ROOM].objectGroups[SCREEN][OBJECT_GROUP_NUMBER], &DSGM_Objects[OBJECT_NAME], DSGM_TOP, HOW_MANY, X1, Y1, X2, Y2, ...);
	
	DSGM_SetupObjectGroups(&DSGM_Rooms[Room_1], DSGM_BOTTOM, 15);
	
	//Slider button
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][0], &DSGM_Objects[Slider_Obj], DSGM_BOTTOM, 1, 224, 16);
	
	//Layer buttons
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][1], &DSGM_Objects[Layer_1_Obj], DSGM_BOTTOM, 1, 0, 0);
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][2], &DSGM_Objects[Layer_2_Obj], DSGM_BOTTOM, 1, 0, 56);
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][3], &DSGM_Objects[Layer_3_Obj], DSGM_BOTTOM, 1, 0, 112);
	
	//Piece selector
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][4], &DSGM_Objects[PieceTemp_Obj], DSGM_BOTTOM, 1, 200, 160);	
	
	//Pieces
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][5], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 144, 49);	
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][6], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 112, 49);
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][7], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 80, 49);

	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][8], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 144, 80);
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][9], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 112, 80);
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][10], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 80, 80);

	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][11], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 145, 113);
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][12], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 112, 112);
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][13], &DSGM_Objects[Pieces_Obj], DSGM_BOTTOM, 1, 80, 113);
	
	DSGM_SetupObjectInstances(&DSGM_Rooms[Room_1].objectGroups[DSGM_BOTTOM][14], &DSGM_Objects[Arrow_Obj], DSGM_BOTTOM, 1, 238, 168);
	

	
	if(room != DSGM_ALL_ROOMS) return;
}

void renderer_create(rendererObjectInstance *me) {
	//Enable 3D!
	DSGM_InitStandard3D(); 
	
	//Loading Textures
	DSGM_LoadTexture(&BoardTexture);
	DSGM_LoadTexture(&PieceBlueTexture);
	DSGM_LoadTexture(&PieceRedTexture);
	DSGM_LoadTexture(&TableTopTexture);
	
	//gluLookAt(
    //    eye_x, eye_y, eye_z,
    //    center_x, center_y, center_z,   
    //    up_x, up_y, up_z
    //);
	
	//Set view
	gluLookAt(-2, 0.7, 0, 0, 0.7, 0, 0, 1, 0);
	
	glLight(0, RGB15(31, 31, 31), 0, floattov10(-1.0), 0);
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_MODULATION | POLY_FORMAT_LIGHT0);
	
	//Arranging the layers in a different order
	DSGM_SetLayerPriority(DSGM_BOTTOM, 0, 1);
	DSGM_SetLayerPriority(DSGM_BOTTOM, 1, 2);
	DSGM_SetLayerPriority(DSGM_BOTTOM, 2, 3);	
	
	DSGM_SetLayerPriority(DSGM_TOP, 2, 3);
	DSGM_SetLayerPriority(DSGM_TOP, 2, 3);
	
	//Adjusting the position and rotatioon center of the rot bg
	DSGM_layers[DSGM_BOTTOM][3].x = 128;
	DSGM_layers[DSGM_BOTTOM][3].y = 96;
	DSGM_SetRotationCenter(DSGM_BOTTOM, 128, 96);
	
	glMatrixMode(GL_MODELVIEW);
}

void renderer_loop(rendererObjectInstance *me) {
	//Switching between layers
	if(DSGM_newpress.Stylus) {
		if(DSGM_stylus.x < 30) {
			if(DSGM_stylus.y > 6 && DSGM_stylus.y < 57)	layer = 0;
			if(DSGM_stylus.y > 62 && DSGM_stylus.y < 113)	layer = 1;
			if(DSGM_stylus.y > 118 && DSGM_stylus.y < 169)	layer = 2;
		}
	}
	
	if(DSGM_newpress.Left && layer > 0) layer--;
	if(DSGM_newpress.Right && layer < 2) layer++;	
	
	if(layer == 1 && View_Y > 0) View_Y -= 0.05f;
	if(layer == 0 && View_Y > -1) View_Y -= 0.05f;
	
	if(layer == 2 && View_Y < 1) View_Y += 0.05f;
	if(layer == 1 && View_Y < 0) View_Y += 0.05f;
	
	if(DSGM_held.Up && View_X > 0.0f) View_X -= 0.05f;
	if(DSGM_held.Down && View_X < 2.5f) View_X += 0.05f;
	
	x = DSGM_stylus.x;
	y = DSGM_stylus.y;
	
	if(!PieceTouched){
		// Only rotate board if you touch in a certain area
		if(DSGM_newpress.Stylus && DSGM_stylus.x > 32 && DSGM_stylus.x < 256 - 32) {
			angle_1 = DSGM_GetAngle(128, 96, x, y);
			touch = true;
		}
	
		//Store position of angle
		if(!DSGM_held.Stylus) {
			rotateboard = rotation;
			touch = false;
		}
	
		//Adjusting the angle of the board
		if(touch) {
			angle_2 = angle_1 - DSGM_GetAngle(128, 96, x, y);
			rotation = rotateboard - angle_2;
		}
	
		//Rotatng the board
		DSGM_RotateBackground(DSGM_BOTTOM, rotation);
	}

	//Load 3d pieces
	int i,j;
	
	for(i=0;i<3;i++)for(j=0;j<3;j++)if(!Board[layer][i][j].InUse){
		switch(Board[layer][i][j].color){
			case BLUE:
				DSGM_CreateObjectInstance(DSGM_TOP, j, i, &DSGM_Objects[Piece_Blue]);
				Board[layer][i][j].InUse = true;
				break;
			case RED:
				DSGM_CreateObjectInstance(DSGM_TOP, j, i, &DSGM_Objects[Piece_Red]);
				Board[layer][i][j].InUse = true;
				break;
		}
	}
	
	if(Row_Win()) DSGM_DrawText(DSGM_TOP, 10, 5, "YOU WIN!");

	
	//Scroll panoramic backgroud
	DSGM_view[DSGM_TOP].x = (rotation / 32) % 512;
	
	
	glPushMatrix();
	
	//Render board Models
	glBindTexture(0, BoardTexture.id);
	
	glTranslatef(View_X, View_Y, z);
	glRotateYi(rotation);
	
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, 0.0f);
	glCallList((u32 *)Board_bin);
	glPopMatrix(1);
	
	glCallList((u32 *)Board_bin);
	
	glPushMatrix();
	glTranslatef(0.0f, -1.0f, 0.0f);
	glCallList((u32 *)Board_bin);
	glPopMatrix(1);
	
	glBindTexture(0, TableTopTexture.id);
	glPushMatrix();
	glTranslatef(0.0f, -1.2f, 0.0f);
	glCallList((u32 *)TableTop_bin);
	glPopMatrix(1);
	
	glPopMatrix(1);
}

void Debugger_loop(DebuggerObjectInstance *me) {
	//Toggle debug on/off
	if(DSGM_newpress.A) {
		if(Debug) {
			Debug = false;
			}else{
			Debug = true;
		}
	}

	// Draw variables for debugging
	if(Debug) {
		//Clear Text
		DSGM_ClearText(1);
	
		//Debugging
		DSGM_DrawText(DSGM_TOP, 0,  3, "frames");
		DSGM_DrawText(DSGM_TOP, 20, 3, "%d",frames);
		DSGM_DrawText(DSGM_TOP, 20, 4, "%d",DSGM_stylus.x);
		DSGM_DrawText(DSGM_TOP, 20, 5, "%d",DSGM_stylus.y);
		DSGM_DrawText(DSGM_TOP, 0,  4, "DSGM_stylus.x");
		DSGM_DrawText(DSGM_TOP, 0,  5, "DSGM_stylus.y");
		DSGM_DrawText(DSGM_TOP, 20, 6, "%d",DSGM_stylus.dx);
		DSGM_DrawText(DSGM_TOP, 20, 7, "%d",DSGM_stylus.dy);
		DSGM_DrawText(DSGM_TOP, 0,  6, "DSGM_stylus.dx");
		DSGM_DrawText(DSGM_TOP, 0,  7, "DSGM_stylus.dy");
		DSGM_DrawText(DSGM_TOP, 0,  8, "layer");
		DSGM_DrawText(DSGM_TOP, 20, 8, "%d",layer);
		DSGM_DrawText(DSGM_TOP, 0,  9, "View_Y");
		DSGM_DrawText(DSGM_TOP, 20, 9, "%f",View_Y);
		DSGM_DrawText(DSGM_TOP, 0,  10, "View_X");
		DSGM_DrawText(DSGM_TOP, 20, 10, "%f",View_X);
		DSGM_DrawText(DSGM_TOP, 0,  11, "zoom");
		DSGM_DrawText(DSGM_TOP, 20, 11, "%d",zoom);
		DSGM_DrawText(DSGM_TOP, 0,  12, "rotation");
		DSGM_DrawText(DSGM_TOP, 20, 12, "%d",rotation);
		DSGM_DrawText(DSGM_TOP, 0,  13, "angle_1");
		DSGM_DrawText(DSGM_TOP, 20, 13, "%d",angle_1);
		DSGM_DrawText(DSGM_TOP, 0,  14, "angle_2");
		DSGM_DrawText(DSGM_TOP, 20, 14, "%d",angle_2);
		DSGM_DrawText(DSGM_TOP, 0,  15, "rotateboard");
		DSGM_DrawText(DSGM_TOP, 20, 15, "%d",rotateboard);
		DSGM_DrawText(DSGM_TOP, 0,  16, "rotvelcotiy");
		DSGM_DrawText(DSGM_TOP, 20, 16, "%d",RotVelocity);
		DSGM_DrawText(DSGM_TOP, 0,  17, "D to A Rotation");
		DSGM_DrawText(DSGM_TOP, 20, 17, "%d",DegreesToRadians(rotation));
		DSGM_DrawText(DSGM_TOP, 0,  18, "DSGM_view[DSGM_TOP].x");
		DSGM_DrawText(DSGM_TOP, 20, 18, "%d",DSGM_view[DSGM_TOP].x);
		}else{
		//DSGM_ClearText(1);
	}
}

void Piece_Blue_create(PieceBlueObjectInstance *me) {

	switch(me->x){
		case 0:
			me->variables->x = 0.65;
			break;
		case 1:
			me->variables->x = 0;
			break;
		case 2:
			me->variables->x = -0.65;
			break;
	}
	
	me->variables->y = 1-layer;
	
	switch(me->y){
		case 0:
			me->variables->z = -0.65;
			break;
		case 1:
			me->variables->z = 0;
			break;
		case 2:
			me->variables->z = 0.65;
			break;
	}
}

void Piece_Blue_loop(PieceBlueObjectInstance *me) {
	// Draw the blue piece
	glPushMatrix();
	glTranslatef(View_X, View_Y, z);
	glRotateYi(rotation);
	
	// Apply transformation after rotation, and the rotation will correct the position
	glTranslatef(me->variables->x, me->variables->y, me->variables->z);
	
	glBindTexture(0, PieceBlueTexture.id);
	glCallList((u32 *)Piece_bin);
	glPopMatrix(1);
	
	if(DSGM_newpress.Start){
		DSGM_DeleteObjectInstance(me);
		DSGM_ClearText(1);
		reset_board();
	}
}

void Piece_Red_create(PieceRedObjectInstance *me) {
	
	switch(me->x){
		case 0:
			me->variables->x = 0.65;
			break;
		case 1:
			me->variables->x = 0;
			break;
		case 2:
			me->variables->x = -0.65;
			break;
	}
	
	me->variables->y = 1-layer;	
	
	switch(me->y){
		case 0:
			me->variables->z = -0.65;
			break;
		case 1:
			me->variables->z = 0;
			break;
		case 2:
			me->variables->z = 0.65;
			break;
	}
}

void Piece_Red_loop(PieceRedObjectInstance *me) {
	// Draw the purple piece
	glPushMatrix();
	glTranslatef(View_X, View_Y, z);
	glRotateYi(rotation);
	
	// Apply transformation after rotation, and the rotation will correct the position
	glTranslatef(me->variables->x,me->variables->y ,me->variables->z);
	
	glBindTexture(0, PieceRedTexture.id);
	glCallList((u32 *)Piece_bin);
	glPopMatrix(1);
	
	if(DSGM_newpress.Start){
		DSGM_DeleteObjectInstance(me);
		DSGM_ClearText(1);
		reset_board();
	}
}

void Piece_create(PieceObjectInstance *me) {

	me->variables->relativeRotation = DSGM_GetAngle(128, 96, me->x + 16, me->y + 16);
	me->variables->distance = DSGM_Distance(128, 96, me->x + 16, me->y + 16);
	me->bitshift = 12;
	
	switch(me->spriteNumber){
		case 5:
			me->bx = 0;
			me->by = 0;
			break;
		case 6:
			me->bx = 1;
			me->by = 0;
			break;
		case 7:
			me->bx = 2;
			me->by = 0;
			break;
		case 8:
			me->bx = 0;
			me->by = 1;
			break;
		case 9:
			me->bx = 1;
			me->by = 1;
			break;
		case 10:
			me->bx = 2;
			me->by = 1;
			break;
		case 11:
			me->bx = 0;
			me->by = 2;
			break;
		case 12:
			me->bx = 1;
			me->by = 2;
			break;
		case 13:
			me->bx = 2;
			me->by = 2;
			break;
	}
}

void Piece_loop(PieceObjectInstance *me) {
	
	//Display correct piece color
	me->frame = Board[layer][me->bx][me->by].color;

	//Move pieces along with the board
	int r = rotation - me->variables->relativeRotation;
	
	me->x = 128 - 16 - ((cosLerp(r) * me->variables->distance) >> me->bitshift);
	me->y = 96 - 16 + ((sinLerp(r) * me->variables->distance) >> me->bitshift);
	
	//Set piece on board
	if(DSGM_release.Stylus){
		if(PieceTouched){
			if(DSGM_stylus.x>me->x && DSGM_stylus.x<me->x+32 && DSGM_stylus.y>me->y && DSGM_stylus.y<me->y+32){
				if(!Board[layer][me->bx][me->by].InUse){
					if(DSGM_held.L)Board[layer][me->bx][me->by].color = RED;
					if(!DSGM_held.L)Board[layer][me->bx][me->by].color = BLUE;
					PieceTouched = false;
				}
			}
		}
	}
}

void Slider_loop(SliderObjectInstance *me) {
	if(DSGM_held.Up || DSGM_held.Down) {
		if(!me->variables->touched) {
			zoom = true;
		}
	}
	else{
		zoom = false;
	}
	
	if(!zoom) {
		if(DSGM_newpress.Stylus) {
			if(DSGM_stylus.x > me-> x + 7 && DSGM_stylus.x < me->x + 26 && DSGM_stylus.y > me->y + 2 && DSGM_stylus.y < me->y + 12) {
				me->variables->touched = true;
			}
		}
	}
	else{
		me->y -= (DSGM_held.Up - DSGM_held.Down) * 3;
		me->frame = 1;
	}
	
	if(me->variables->touched) {
		me->y = DSGM_stylus.y - 10;
		me->frame = 1;
		View_X = (DSGM_stylus.y - 20) / 20.0f;
	}
	
	if(!DSGM_held.Stylus) {
		me->variables->touched = false;
	}
	
	if(!zoom && !me->variables->touched) {
		me->frame = 0;
	}
	
	if(me->y < 8)   me->y = 8;
	if(me->y > 152) me->y = 152;
}

void Layer_1_create(Layer1ObjectInstance *me) {
	me->frame = 1;
}

void Layer_1_loop(Layer1ObjectInstance *me) {
	if(layer == 0) {
		me->frame = 1;
	}
	else{
		me->frame = 0;
	}
}

void Layer_2_create(Layer2ObjectInstance *me) {
	layer = 0;
}

void Layer_2_loop(Layer2ObjectInstance *me) {
	if(layer == 1) {
		me->frame = 3;
	}
	else{
		me->frame = 2;
	}
}

void Layer_3_create(Layer3ObjectInstance *me) {
	me->frame = 4;
}

void Layer_3_loop(Layer3ObjectInstance *me) {
	if(layer == 2) {
		me->frame = 5;
	}
	else{
		me->frame = 4;
	}
}

void Arrow_create(ArrowObjectInstance *me) {

}

void Arrow_loop(ArrowObjectInstance *me) {
	if(PieceTouched){
		me->frame = 0;
	}
	else{
		DSGM_AnimateObjectInstance(me, 1, 3, 10);
	}
}

void PieceTemp_create(PieceTempObjectInstance *me){
}

void PieceTemp_loop(PieceTempObjectInstance *me){
	me->frame = 1;

	if(DSGM_newpress.Stylus && DSGM_stylus.x>me->x+5 && DSGM_stylus.x<me->x+32-6&&DSGM_stylus.y>me->y+6&&DSGM_stylus.y<me->y+32-6){
		PieceTouched = true;
	}
	if(DSGM_held.Stylus && PieceTouched){
		me->x = DSGM_stylus.x-16;
		me->y = DSGM_stylus.y-16;
	}
	else{
		me->x = 208;
		me->y = 160;
		//PieceTouched = false;
	}
}
