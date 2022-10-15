#ifndef CLIENT_CHAIR_H
#define CLIENT_CHAIR_H

#include "xi_engine.h"
#include "server_chair.h"

#define LINE_WIDTH 64
#define LINE_COUNT BUFFER_SIZE/LINE_WIDTH

typedef struct input_stream{
	char line[LINE_COUNT][LINE_WIDTH];
	u8 target;
}input_stream;

void input_stream_init(input_stream* stream);
void input_stream_new_line(input_stream* stream);

typedef struct client_chair{
	input_stream buffer;
	u16 scroll;
}client_chair;

void client_buffer_init(client_chair* client);

SYSTEM(client_input);

#endif
