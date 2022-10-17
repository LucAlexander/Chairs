#include "server_chair.h"

VECTOR_SOURCE(address_stack, u64)
VECTOR_SOURCE(port_list, port_chair)

file_chair* file_init(FILE_TYPE type, char* name, char* data){
	file_chair* fl = malloc(sizeof(file_chair));
	memset(fl->files, 0, FILE_EDGE_MAX*sizeof(file_chair*));
	strcpy(fl->name,name);
	strcpy(fl->data, "");
	if (data != NULL){
		strcpy(fl->data, data);
	}
	fl->type = type;
	return fl;
}

void file_remove(file_chair* fl){
	u16 i, k;
	for (i = 0;i<FILE_EDGE_MAX;++i){
		file_chair* node = fl->files[i];
		if (node == NULL){
			for (k = 0;k<FILE_EDGE_MAX;++k){
				if (node->files[k] == fl){
					node->files[k] = NULL;
				}
			}
			fl->files[i] = NULL;
		}
	}
	free(fl);
	fl = NULL;
}

void file_connect(file_chair* a, file_chair* b){
	u16 i;
	u8 seeka = 1;
	u8 seekb = 1;
	for (i = 0;i<FILE_EDGE_MAX;++i){
		if (seeka && a->files[i] == NULL){
			a->files[i] = b;
			seeka = 0;
		}
		if (seekb && b->files[i] == NULL){
			b->files[i] = a;
			seekb = 0;
		}
	}
}

void file_disconnect(file_chair* a, file_chair* b){
	u16 i;
	for (i = 0;i<FILE_EDGE_MAX;++i){
		if (a->files[i] == b){
			a->files[i] = NULL;
		}
		if (b->files[i] == a){
			b->files[i] = NULL;
		}
	}
}

char* file_parse_pointer(file_chair* node, char* path){
	char* token = strtok(path, "/");
	u16 i;
	file_chair* ptr = node;
	while (token != NULL){
		u8 found = 0;
		for(i = 0;i<FILE_EDGE_MAX;++i){
			file_chair* pos = ptr->files[i];
			if (pos==NULL)continue;
			if (strcmp(pos->name, token) == 0){
				ptr = pos;
				found = 1;
				break;
			}
			if (!found){
				if (NULL==strtok(NULL, "/")){
					return token;
				}
				return NULL;
			}
		}
		token = strtok(NULL, "/");
	}
	return NULL;
}

void server_address_space_init(server_address_space* addr){
	addr->stack = address_stackInit();
	addr->current = 0;
}

void port_init(port_chair* p, u64 server){
	p->address = server;
	p->job = PORT_LISTEN;
	strcpy(p->data, "");
	p->socket = NULL;
}

u64 generate_address(server_address_space* addr){
	if (addr->stack.size != 0){
		return address_stackPop(&addr->stack);
	}
	return addr->current++;
}

void server_init(server_chair* s, u16 port_count, server_address_space* addr){
	s->ports = port_listInit();
	u16 i;
	s->address = generate_address(addr);
	for (i = 0;i<port_count;++i){
		port_chair p;
		port_init(&p, s->address);
		port_listPushBack(&s->ports, p);
	}
	strcpy(s->rom, "");
	s->root = file_init(FILE_DIR, "root", NULL);
	s->ptr = s->root;
}

void server_send(server_chair* s, const char* data, u16 port){
	port_chair* p = &s->ports.data[port];
	if (p->job != PORT_SEND) return;
	strcpy(p->data, data);
}

void server_transfer_data(port_chair* sender, port_chair* reciever){
	strcpy(reciever->data, sender->data);
}

void server_connect(server_chair* s, server_chair* foreign, u16 port){
	port_chair* f = &foreign->ports.data[port];
	if (f->job != PORT_LISTEN) return;
	u16 i, n = s->ports.size;
	for (i = 0;i<n;++i){
		port_chair* p = &s->ports.data[i];
		if (p->job == PORT_LISTEN){
			p->job = PORT_SEND;
			p->socket = f;
			f->job = PORT_RECIEVE;
			f->socket = p;
			return;
		}
	}
}

void server_disconnect(server_chair* s, u64 address){
	u16 i, n = s->ports.size;
	for (i = 0;i<n;++i){
		port_chair* p = &s->ports.data[i];
		if (p->socket == NULL) continue;
		if (p->socket->address == address){
			p->socket->job = PORT_LISTEN;
			p->socket->socket = NULL;
			p->job = PORT_LISTEN;
			p->socket = NULL;
			return;
		}
	}
}

void substring(i8 s[], i8 sub[], i32 p, i32 l) {
	i32 c = 0;
	while (c < l) {
		sub[c] = s[p+c-1];
		c++;
	}
	sub[c] = '\0';
}

i32 find_ch_index(i8 string[], i8 ch) {
    i32 i = 0;
    while (string[i] != '\0') if (string[i++] == ch) return i;
    return -1;
}

void server_cd(server_chair* s, char* arg, char* line){
	if (strcmp(arg, "")==0){
		s->ptr = s->root;
		return;
	}
	file_chair* cpy = s->ptr;
	char* name = file_parse_pointer(cpy, arg);
	strcpy(line, "");
	if (name == NULL || cpy == NULL){
		strcat(line, "Cannot find directory\n");
		return;
	}
	if (cpy->type == FILE_DIR){
		s->ptr = cpy;
		return;
	}
	strcat(line, "File is not a directory\n");
}

void server_ls(server_chair* s, char* line){
	strcpy(line, "node list\n");
	u16 i;
	for (i = 0;i<FILE_EDGE_MAX;++i){
		file_chair* fl = s->ptr->files[i];
		if (fl==NULL)continue;
		if (fl->type==FILE_DIR){
			strcat(line, fl->name);
			strcat(line, "/\n");
			continue;
		}
		strcat(line, fl->name);
		strcat(line,"\n");
	}
}

void server_mkdir(server_chair* s, char* arg, char* line){
	file_chair* cpy = s->ptr;
	char* name = file_parse_pointer(cpy, arg);
	if (name == NULL){
		strcpy(line, "");
		strcat(line, "Directory not possible\n");
		return;
	}
	file_chair* node = file_init(FILE_DIR, name, NULL);
	file_connect(cpy, node);
}

void server_link(server_chair* s, char* arg, char* line){
	file_chair* cpy = s->ptr;
	char* name = file_parse_pointer(cpy, arg);
	if (name == NULL || cpy==s->ptr){
		strcpy(line, "");
		strcat(line, "File not found\n");
		return;
	}
	file_connect(s->ptr, cpy);
}

void server_rm(server_chair* s, char* arg, char* line){
	file_chair* cpy = s->ptr;
	char* name = file_parse_pointer(cpy, arg);
	strcpy(line, "");
	if (name == NULL){
		strcat(line, "Cannot remove directory\n");
	}
	if (cpy == s->ptr){
		strcat(line, "Cannot remove current directory\n");
		return;
	}
	file_remove(cpy);
}

