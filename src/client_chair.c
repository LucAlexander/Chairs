#include "client_chair.h"
#include "tab_chair.h"
#include "project.h"
#include "xi_engine.h"

void client_buffer_init(client_chair* client){
	client->scroll = 0;
	input_stream_init(&client->buffer);
}

void input_stream_init(input_stream* stream){
	u8 i;
	stream->target = 0;
	for (i = 0;i<LINE_WIDTH;++i){
		strcpy(stream->line[i], "");
	}
}

void input_stream_new_line(input_stream* stream){
	strcat(stream->line[stream->target], "\n");
	if (stream->target < LINE_COUNT-1){
		stream->target ++;
		return;
	}
	u8 i;
	for (i = 0;i<LINE_COUNT-1;++i){
		strcpy(stream->line[i], stream->line[i+1]);
	}
	strcpy(stream->line[stream->target], "");
}

void scrollToCurrent(xi_utils* xi, client_chair* client, tab_chair* tab){
	u16 dist = (tab->h/(getTextHeight(xi->graphics, "#")+LINE_SPACING)-1);
	if (client->buffer.target - dist >= client->scroll){
		client->scroll = client->buffer.target - (dist-1);
	}
}

SYSTEM(client_input){
	ARG(client_chair* client, CLIENT_C);
	ARG(tab_chair* tab, TAB_C);
	ARG(server_chair* server, SERVER_C);
	if (xi->project->controller.focused != tab) return;
	u8 up = keyPressed(xi->user_input, "Up");
	u8 down = keyPressed(xi->user_input, "Down");
	u8 enter = keyPressed(xi->user_input, "Return");
	u8 back = keyPressed(xi->user_input, "Backspace");
	u32 i;
	strcpy(tab->client, "");
	for (i = client->scroll;i<LINE_COUNT;++i){
		if (client->buffer.target == i){
			strcat(tab->client, server->ptr->name);
			strcat(tab->client, "> ");
		}
		strcat(tab->client, client->buffer.line[i]);
	}
	if (down){
		if (client->scroll < LINE_COUNT-1){
			client->scroll++;
		}
	}
	if (up){
		if (client->scroll > 0){
			client->scroll--;
		}
	}
	if (enter){
		parseCommand(client, server);
		input_stream_new_line(&client->buffer);
		if (client->buffer.target != LINE_COUNT-1){
			u16 u8_h = getTextHeight(xi->graphics, "#")+LINE_SPACING;
			u16 current_pane = client->buffer.target-client->scroll;
			if ((current_pane+1)*u8_h >= tab->h){
				scrollToCurrent(xi, client, tab);
			}
		}
	}
	if (back){
		client->buffer.line[client->buffer.target][strlen(client->buffer.line[client->buffer.target])-1] = '\0';
	}
	if (keyPressed(xi->user_input, "Space")){
		strcat(client->buffer.line[client->buffer.target], " ");
		scrollToCurrent(xi, client, tab);
	}
	i8 keys[48];
	keystream(xi->user_input, keys, "1234567890-=qazwsxedcrfvtgbyhnujmik,./;'lop[]\\");
	if ((strcmp(keys, "") == 0) || strlen(keys) + strlen(client->buffer.line[client->buffer.target]) > 63) return;
	strcat(client->buffer.line[client->buffer.target], keys);
	scrollToCurrent(xi, client, tab);
	
}

void parseCommand(client_chair* client, server_chair* sever){
	if (strcmp(client->buffer.line[client->buffer.target],"exit")==0){
		printf("\tstub exit window\n");
		return;
	}
	char* token = strtok(client->buffer.line[client->buffer.target]," ");
	if (token != NULL){
		printf("token: %s\n", token);
		if (strcmp(token, "cd")==0){
			token = strtok(NULL, " ");
			printf("\tstub cd %s\n", token);
			return;
		}
		if (strcmp(token, "ls")==0){
			token = strtok(NULL, " ");
			printf("\tstub ls %s\n", token);
			return;
		}
		if (strcmp(token, "mkdir")==0){
			token = strtok(NULL, " ");
			printf("\tstub mkdir %s\n", token);
			return;
		}
		if (strcmp(token, "rm")==0){
			token = strtok(NULL, " ");
			printf("\tstub rm %s\n", token);
			return;
		}
		if (strcmp(token, "link")==0){
			token = strtok(NULL, " ");
			printf("\nstub link %s\n", token);
			return;
		}
	}
}
