#ifndef TAB_CHAIR_H
#define TAB_CHAIR_H

#include "xi_engine.h"
#include "server_chair.h"

#define BORDER_GRAB_GIVE 8
#define BORDER_MIN 128
#define BORDER_MAX_W 1366
#define BORDER_MAX_H 768
#define LINE_SPACING 8

#define LAYER_FOCUSED 10
#define LAYER_UNFOCUSED 5

typedef enum TAB_FLAGS{
	MIN=1,
	MV=2,
	HX=3,
	HY=4,
	RX=5,
	RY=6
}TAB_FLAGS;

struct tab_chair;

VECTOR(tab_list, struct tab_chair*)

typedef struct tab_controller{
	struct tab_chair* focused;
	tab_list tabs;
}tab_controller;

typedef enum TAB_STATE{
	TAB_IDLE,
	TAB_MOVE,
	TAB_RESIZE
}TAB_STATE;

typedef struct tab_chair{
	char client[BUFFER_SIZE];
	u8 flags;
	u32 w, h;
	u32 sx, sy;
	tab_controller* controller;
	TAB_STATE state;
}tab_chair;

SYSTEM(draw_tab);
SYSTEM(mutate_tab);
SYSTEM(switch_tabs);

#endif
