#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "entity.h"
#include "item.h"
#include "account.h"

void client_login_success(struct account *account);
void client_login_failure();


#endif
