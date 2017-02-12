#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "const.h"

struct account {
  struct entity_player *characters[MAX_NUM_CHARACTERS];
  char username[MAX_USERNAME_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
};

#endif
