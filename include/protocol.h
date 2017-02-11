#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "account.h"
#include "entity.h"

enum packet {
  PACKET_CS_LOGIN,
  PACKET_CS_PING,
  PACKET_CS_GET_CHARACTER,
  PACKET_SC_LOGIN_FAILURE,
  PACKET_SC_LOGIN_SUCCESS,
  PACKET_SC_GET_CHARACTER,
  PACKET_SC_PING,
  NUM_PACKAGE_TYPES
};

extern const char *packet_names[NUM_PACKAGE_TYPES];

struct packet_cs_login {
  char username[MAX_USERNAME_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
};

struct packet_cs_get_character {
  uint64_t entity_index;
  uint8_t character_index;
};

struct packet_cs_ping {
  uint64_t ticks;
};

enum login_failure {
  LOGIN_FAILURE_WRONG_PASSWORD,
  LOGIN_FAILURE_USERNAME_INEXISTENT,
  LOGIN_FAILURE_ALREADY_ONLINE,
};

struct packet_sc_login_failure {
  int8_t reason;
};

struct packet_sc_login_success {
  uint64_t entity_index; // Index in the entity list
  uint8_t num_characters;
};

struct packet_sc_get_character {
  char name[MAX_ENTITY_NAME_LENGTH];
  int character_class;
};

struct packet_sc_ping {
  uint64_t ticks;
};

size_t packet_buf_length(int packet_type);
int32_t packet_get_type(void *data, size_t length);

#endif
