#include "client_chair.h"
#include "tab_chair.h"
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

SYSTEM(client_input){
	ARG(client_chair* client, CLIENT_C);
	ARG(tab_chair* tab, TAB_C);
	u8 up = keyPressed(xi->user_input, "Up");
	u8 down = keyPressed(xi->user_input, "Down");
	u8 enter = keyPressed(xi->user_input, "Return");
	u8 back = keyPressed(xi->user_input, "Backspace");
	u32 i;
	strcpy(tab->client, "");
	for (i = client->scroll;i<LINE_COUNT;++i){
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
		if (client->buffer.target != LINE_COUNT-1){
			client->scroll++;
		}
		input_stream_new_line(&client->buffer);
	}
	if (back){
		client->buffer.line[client->buffer.target][strlen(client->buffer.line[client->buffer.target])-1] = '\0';
	}
	i8 keys[48];
	keystream(xi->user_input, keys, "1234567890-=qazwsxedcrfvtgbyhnujmik,./;'lop[]\\");
	if ((strcmp(keys, "") == 0) || strlen(keys) + strlen(client->buffer.line[client->buffer.target]) > 63) return;
	strcat(client->buffer.line[client->buffer.target], keys);
}

