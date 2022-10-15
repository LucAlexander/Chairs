#include "server_chair.h"

VECTOR_SOURCE(address_stack, u64)
VECTOR_SOURCE(port_list, port_chair)

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
	strcpy(s->rom, ""); // TODO replace
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

