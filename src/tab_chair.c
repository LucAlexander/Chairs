#include "tab_chair.h"
#include "project.h"

SYSTEM(mutate_tab){
	ARG(tab_chair* tab, TAB_C);
	ARG(v2* position, POSITION_C);
	entity_set_layer(xi->ecs, id, LAYER_UNFOCUSED);
	if (xi->project->controller.focused != tab) return;
	entity_set_layer(xi->ecs, id, LAYER_FOCUSED);
	v2 mouse = mousePos(xi->user_input);
	u8 leftClick = mousePressed(xi->user_input, 1);
	u8 leftReleased = mouseReleased(xi->user_input, 1);
	u8 leftHold = mouseHeld(xi->user_input, 1);
	u8 shiftHeld = keyHeld(xi->user_input, "Left Shift");
	u8 shiftReleased = keyReleased(xi->user_input, "Left Shift");
	switch(tab->state){
		case TAB_IDLE:{
			tab->flags = 0;
			if (
				shiftHeld &&
				leftClick &&
				(mouse.x < position->x + tab->w) &&
				(mouse.y < position->y + tab->h) && 
				(mouse.x > position->x) &&
				(mouse.y > position->y)
			){
				tab->sx = mouse.x;
				tab->sy = mouse.y;
				tab->state = TAB_MOVE;
			}
			i32 cx = position->x - mouse.x;
			i32 cy = position->y - mouse.y;
			if (
				leftHold && 
				abs(cx) < BORDER_GRAB_GIVE
			){
				tab->flags = bit_on(tab->flags, HX);
				tab->state = TAB_RESIZE;
			}
			if (
				leftHold && 
				abs(cy) < BORDER_GRAB_GIVE
			){
				tab->flags = bit_on(tab->flags, HY);
				tab->state = TAB_RESIZE;
			}
			cx = mouse.x - (position->x+tab->w);
			cy = mouse.y - (position->y+tab->h);
			if (
				leftHold && 
				abs(cx) < BORDER_GRAB_GIVE
			){
				tab->flags = bit_on(tab->flags, RX);
				tab->state = TAB_RESIZE;
			}
			if (
				leftHold && 
				abs(cy) < BORDER_GRAB_GIVE
			){
				tab->flags = bit_on(tab->flags, RY);
				tab->state = TAB_RESIZE;
			}
		}break;
		case TAB_MOVE:{
			position->x += mouse.x - tab->sx;
			position->y += mouse.y - tab->sy;
			tab->sx = mouse.x;
			tab->sy = mouse.y;
			tab->flags = 0;
			if (shiftReleased || leftReleased){
				tab->state = TAB_IDLE;
			}
		}break;
		case TAB_RESIZE:{
			i32 cx = position->x - mouse.x;
			i32 cy = position->y - mouse.y;
			if (
				bit_check(tab->flags, HX)&&
				tab->w + cx < BORDER_MAX_W &&
				tab->w + cx > BORDER_MIN
			){
				position->x -= cx;
				tab->w += cx;
			}
			if (
				bit_check(tab->flags, HY) && 
				tab->h + cy < BORDER_MAX_H &&
				tab->h + cy > BORDER_MIN
			){
				position->y -= cy;
				tab->h += cy;
			}
			cx = mouse.x - (position->x+tab->w);
			cy = mouse.y - (position->y+tab->h);
			if (
				bit_check(tab->flags, RX) &&
				tab->w + cx < BORDER_MAX_W &&
				tab->w + cx > BORDER_MIN
			){
				tab->w += cx;
			}
			if (
				bit_check(tab->flags, RY) && 
				tab->h + cy < BORDER_MAX_H &&
				tab->h + cy > BORDER_MIN
			){
				tab->h += cy;
			}
			if (leftReleased){
				tab->state = TAB_IDLE;
			}
		}break;
	}
}

SYSTEM(draw_tab){
	ARG(tab_chair* tab, TAB_C);
	ARG(v2* position, POSITION_C);
	renderSetColor(xi->graphics, 8, 60, 112, 128);
	if (xi->project->controller.focused == tab){
		renderSetColor(xi->graphics, 8, 60, 112, 200);
	}
	drawRect(xi->graphics, position->x, position->y, tab->w, tab->h, FILL);
	renderSetColor(xi->graphics, 244, 244, 255, 183);
	if (xi->project->controller.focused == tab){
		renderSetColor(xi->graphics, 244, 244, 255, 255);
	}
	u16 i8_w, i8_h, p, dy, n, line_w;
	i8_w = getTextWidth(xi->graphics, "#");
	i8_h = getTextHeight(xi->graphics, "#")+LINE_SPACING;
	line_w = (tab->w/i8_w)-1;
	dy = 0;
	p = 0;
	n = strlen(tab->client);
	i8 line[line_w];
	while (p < n && dy < (tab->h-i8_h)){
		strncpy(line, tab->client+p, line_w);
		line[line_w] = '\0';
		i32 nl = find_ch_index(line, '\n');
		if (nl != -1){
			line[nl-1] = '\0';
			drawText(xi->graphics, position->x, position->y + dy, line);
			dy += i8_h;
			p += nl;
			continue;
		}
		drawText(xi->graphics, position->x, position->y + dy, line);
		dy += i8_h;
		p += line_w;
	}
	renderSetColor(xi->graphics, 0, 0, 0, 0);
	v2 mouse = mousePos(xi->user_input);
	if (
		mousePressed(xi->user_input, 1) && (
		(mouse.x < position->x + tab->w) &&
		(mouse.y < position->y + tab->h) && 
		(mouse.x > position->x) &&
		(mouse.y > position->y))
	){
		xi->project->controller.focused = tab;
	}
}
