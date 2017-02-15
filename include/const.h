#ifndef CONST_H
#define CONST_H

#define SERVER_TIMER_TIME 300
#define MAX_NUM_IDLE_TICKS 10

#ifndef SERVER_ADDRESS
#define SERVER_ADDRESS "0.0.0.0"
#endif

#ifndef SERVER_PORT
#define SERVER_PORT 3000 
#endif

#define CLIENT_TIMER_TIME 300
#define CLIENT_MAX_IDLE_TICKS 10

#define MAX_NUM_CHARACTERS 5
#define MAX_USERNAME_LENGTH 10
#define MAX_PASSWORD_LENGTH 10
#define MAX_NUM_INVENTORY_ITEMS 255
#define MAX_NUM_ENTITIES 10000
#define MAX_ENTITY_NAME_LENGTH 10
#define VIEWPORT_WIDTH 15
#define VIEWPORT_HEIGHT 15
#define MAP_SIZE 256
#define MAX_MAP_PATH_LENGTH 1024
#define MAP_ATT_OLD_FILE_SIZE (65536 + 4)
#define MAP_ATT_NEW_FILE_SIZE (65536 * 2 + 4)
#define MAP_ATT_SERVER_FILE_SIZE (65536 + 3)
#define MAP_TILE_FILE_SIZE (65536 * 3 + 2)

#endif
