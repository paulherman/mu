#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "account.h"
#include "player.h"

enum packet {
  PACKET_CS_LOGIN,
  PACKET_CS_GET_CHARACTER,
  PACKET_SC_LOGIN_FAILURE,
  PACKET_SC_LOGIN_SUCCESS,
  PACKET_SC_GET_CHARACTER,
};

struct packet_cs_login {
  char username[MAX_USERNAME_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
};

struct packet_cs_get_character {
  uint64_t entity_index;
  uint8_t character_index;
};

enum login_failure {
  LOGIN_FAILURE_WRONG_PASSWORD,
  LOGIN_FAILURE_USERNAME_INEXISTENT,
  LOGIN_FAILURE_ALREADY_ONLINE,
};

struct packet_sc_login_failure {
  int8_t reason;
}

struct packet_sc_login_success {
  uint64_t entity_index; // Index in the entity list
  uint8_t num_characters;
};

struct packet_sc_get_character {
  char name[MAX_LENGTH_CHARACTER_NAME];
  int character_class;
};

#endif
