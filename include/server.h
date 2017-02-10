#ifndef SERVER_H
#define SERVER_H

#include "entity.h"

extern struct entity *entities[MAX_NUM_ENTITIES];

#ifndef SERVER_PORT
#define SERVER_PORT 3000 
#endif

#endif
