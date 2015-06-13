#pragma once

#define DSGM_SOUND_STREAM_COUNT 0
#define DSGM_SOUND_EFFECT_COUNT 0
#define DSGM_SOUND_COUNT (DSGM_SOUND_STREAM_COUNT + DSGM_SOUND_EFFECT_COUNT)
#define DSGM_BACKGROUND_COUNT 4
#define DSGM_PALETTE_COUNT 4
#define DSGM_SPRITE_COUNT 4
#define DSGM_OBJECT_COUNT 12
#define DSGM_ROOM_COUNT 1

// Include backgrounds, palettes and sprites to be loaded from RAM

#include "Board_bin.h"
#include "BoardLight_Texture_bin.h"

#include "Piece_bin.h"
#include "Piece_Blue_Texture_bin.h"
#include "Piece_Red_Texture_bin.h"

// No sounds, no enum
//typedef enum {
//} DSGM_SoundNames;

typedef enum {
	BGCoffeeTop,
	BGCoffeeBottom,
	RotBoard,
	BGCoffee1,
} DSGM_BackgroundNames;

typedef enum {
	PiecePal,
	SliderPal,
	LayerPal,
	ArrowPal,
} DSGM_PaletteNames;

typedef enum {
	Pieces_Spr,
	Slider_Spr,
	Layers_Spr,
	Arrow_Spr,
} DSGM_SpriteNames;

typedef enum {
	renderer,
	Debugger,
	Piece_Blue,
	Piece_Red,
	Pieces_Obj,
	Slider_Obj,
	Layer_1_Obj,
	Layer_2_Obj,
	Layer_3_Obj,
	Arrow_Obj,
	PieceTemp_Obj,
} DSGM_ObjectNames;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
	} *variables;
} rendererObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
	} *variables;
} DebuggerObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
		float x,y,z;
	} *variables;
} PieceBlueObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
		float x,y,z;
	} *variables;
} PieceRedObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
		int ID;
		int bx, by;
		int x, y;
		int relativeRotation;
		int distance;
		bool touched;
	} *variables;
} PieceObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
		bool touched;
	} *variables;
} SliderObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
	} *variables;
} Layer1ObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
	} *variables;
} Layer2ObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
	} *variables;
} Layer3ObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
	} *variables;
} ArrowObjectInstance;

typedef struct {
	DSGM_ObjectInstanceBase;
	struct {
	} *variables;
} PieceTempObjectInstance;

typedef enum {
	Room_1,
} DSGM_RoomNames;

extern DSGM_Sound DSGM_Sounds[DSGM_SOUND_COUNT];
extern DSGM_Background DSGM_Backgrounds[DSGM_BACKGROUND_COUNT];
extern DSGM_Palette DSGM_Palettes[DSGM_PALETTE_COUNT];
extern DSGM_Sprite DSGM_Sprites[DSGM_SPRITE_COUNT];
extern DSGM_Object DSGM_Objects[DSGM_OBJECT_COUNT];
extern DSGM_Room DSGM_Rooms[DSGM_ROOM_COUNT];

extern int DSGM_currentRoom;

void DSGM_SetupRooms(int room);

void renderer_create(rendererObjectInstance *me);
void renderer_loop(rendererObjectInstance *me);

void Debugger_loop(DebuggerObjectInstance *me);

void Piece_Blue_create(PieceBlueObjectInstance *me);
void Piece_Blue_loop(PieceBlueObjectInstance *me);

void Piece_Red_create(PieceRedObjectInstance *me);
void Piece_Red_loop(PieceRedObjectInstance *me);

void Piece_create(PieceObjectInstance *me);
void Piece_loop(PieceObjectInstance *me);

void Slider_create(SliderObjectInstance *me);
void Slider_loop(SliderObjectInstance *me);

void Layer_1_create(Layer1ObjectInstance *me);
void Layer_1_loop(Layer1ObjectInstance *me);

void Layer_2_create(Layer2ObjectInstance *me);
void Layer_2_loop(Layer2ObjectInstance *me);

void Layer_3_create(Layer3ObjectInstance *me);
void Layer_3_loop(Layer3ObjectInstance *me);

void Arrow_create(ArrowObjectInstance *me);
void Arrow_loop(ArrowObjectInstance *me);

void PieceTemp_create(PieceTempObjectInstance *me);
void PieceTemp_loop(PieceTempObjectInstance *me);
void PieceTemp_loop(PieceTempObjectInstance *me);
