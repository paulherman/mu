#ifndef ACCOUNT_H
#define ACCOUNT_H

#define MAX_NUM_CHARACTERS 5
#define MAX_USERNAME_LENGTH 10
#define MAX_PASSWORD_LENGTH 10

struct account {
  struct entity_player *characters[MAX_NUM_CHARACTERS];
  char username[MAX_USERNAME_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
};

#endif
