#include <stdio.h>

#include "project.h"
#include "xi_engine.h"

#include "server_chair.h"
#include "tab_chair.h"
#include "client_chair.h"

void project_structs_init(xi_utils* xi){	
	xi->project = malloc(sizeof(project_structs));

	server_address_space_init(&xi->project->address_space);
	xi->project->controller.focused = NULL;
	xi->project->controller.tabs = tab_listInit();
}

void xisetup(program_state* state, xi_utils* xi){
	project_structs_init(xi);
	system_add(state, system_init(mutate_tab, 2, POSITION_C, TAB_C), XI_STATE_UPDATE);
	system_add(state, system_init(switch_tabs, 1, SINGLE_RUN_CONTROLLER_C), XI_STATE_UPDATE);
	system_add(state, system_init(client_input, 2, TAB_C, CLIENT_C), XI_STATE_UPDATE);
	system_add(state, system_init(draw_tab, 2, POSITION_C, TAB_C), XI_STATE_RENDER);
}

void spawn_server(xi_utils* xi, u16 port_count){
	u32 entity = entity_create(xi->ecs);

	server_chair s;
	server_init(&s, port_count, &xi->project->address_space);

	component_add(xi->ecs, entity, SERVER_C, &s);
}

void spawn_window(xi_utils* xi, u32 x, u32 y, u32 w, u32 h){
	u32 entity = entity_create(xi->ecs);

	v2 pos = {x, y};
	tab_chair t;
	t.w = w;
	t.h = h;
	t.flags = 0;
	t.state = TAB_IDLE;

	strcpy(t.client, "This is a single line test\nand this a multiline test");


	component_add(xi->ecs, entity, POSITION_C, &pos);
	component_add(xi->ecs, entity, TAB_C, &t);

	tab_listPushBack(&xi->project->controller.tabs, component_get(xi->ecs, entity, TAB_C));
}

void spawn_client(xi_utils* xi, u32 x, u32 y, u32 w, u32 h){
	u32 entity = entity_create(xi->ecs);
	v2 pos = {x, y};
	tab_chair t;
	t.w = w;
	t.h = h;
	t.flags = 0;
	t.state = TAB_IDLE;

	strcpy(t.client, "");

	client_chair client;
	client_buffer_init(&client);

	server_chair s;
	server_init(&s, 1, &xi->project->address_space);

	component_add(xi->ecs, entity, POSITION_C, &pos);
	component_add(xi->ecs, entity, TAB_C, &t);
	component_add(xi->ecs, entity, CLIENT_C, &client);
	component_add(xi->ecs, entity, SERVER_C, &s);

	tab_listPushBack(&xi->project->controller.tabs, component_get(xi->ecs, entity, TAB_C));
}

void xistart(xi_utils* xi){
	//toggleFullscreen(xi->graphics);
	loadFont(xi->graphics, XI_FONT"m9.ttf", "m9");
	setFont(xi->graphics, "m9");
	spawn_server(xi, 10);
	spawn_window(xi, 32, 32, 320, 180);
	spawn_client(xi, 128, 128, 320, 180);
}
