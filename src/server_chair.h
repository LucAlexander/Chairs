#ifndef SERVER_CHAIR_H
#define SERVER_CHAIR_H

#include "xi_engine.h"

#define BUFFER_SIZE 4096
#define FILE_SIZE 512
#define FILE_EDGE_MAX 16
#define FILE_NAME_SIZE 16

typedef enum FILE_TYPE{
	FILE_DATA,
	FILE_DIR
}FILE_TYPE;

typedef struct file_chair{
	char name[FILE_NAME_SIZE];
	struct file_chair* files[FILE_EDGE_MAX];
	char data[FILE_SIZE];
	FILE_TYPE type;
}file_chair;

file_chair* file_init(FILE_TYPE type, char* name, char* data);
void file_remove(file_chair* fl);
void file_connect(file_chair* a, file_chair* b);
void file_disconnect(file_chair* a, file_chair* b);
char* file_parse_pointer(file_chair* s, char* path);

VECTOR(address_stack, u64)

typedef enum PORT_JOB{
	PORT_LISTEN,
	PORT_RECIEVE,
	PORT_SEND
}PORT_JOB;

typedef struct port_chair{
	u64 address;
	PORT_JOB job;
	struct port_chair* socket;
	i8 data[1024];
}port_chair;

VECTOR(port_list, port_chair)

typedef struct server_chair{
	u64 address;
	port_list ports;
	i8 rom[BUFFER_SIZE];
	file_chair* root;
	file_chair* ptr;
}server_chair;

typedef struct server_address_space{
	address_stack stack;
	u64 current;
}server_address_space;

void server_address_space_init(server_address_space* addr);
u64 generate_address(server_address_space* addr);

void port_init(port_chair* p, u64 server);
void server_init(server_chair* s, u16 port_count, server_address_space* addr);

void server_send(server_chair* s, const char* data, u16 port);
void server_transfer_data(port_chair* sender, port_chair* reciever);

void server_connect(server_chair* s, server_chair* foreign, u16 port);
void server_disconnect(server_chair* s, u64 address);

void substring(i8 s[], i8 sub[], i32 p, i32 l);
i32 find_ch_index(i8 string[], i8 ch);

void server_cd(server_chair* s, char* arg, char* line);
void server_ls(server_chair* s, char* line);
void server_mkdir(server_chair* s, char* arg, char* line);
void server_link(server_chair* s, char* arg, char* line);
void server_rm(server_chair* s, char* arg, char* line);

#endif
