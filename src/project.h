#ifndef PROJECT_H
#define PROJECT_H

#include "tab_chair.h"

struct xi_utils;

typedef struct project_structs{
	server_address_space address_space;
	tab_controller controller;
}project_structs;

void project_structs_init(struct xi_utils*);

#endif
