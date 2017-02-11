#include <assert.h>
#include "protocol.h"

const char *packet_names[NUM_PACKAGE_TYPES] = {
  "PACKET_CS_LOGIN",
  "PACKET_CS_PING",
};

size_t packet_buf_length(int32_t packet_type) {
  size_t len = sizeof(int32_t);
  switch (packet_type) {
    case PACKET_CS_LOGIN:
      len += sizeof(struct packet_cs_login);
      break;
    case PACKET_CS_PING:
      len += sizeof(struct packet_cs_ping);
      break;
  }
  return len;
}

int32_t packet_get_type(void *data, size_t length) {
  assert(length >= sizeof(int32_t));
  return *(int32_t *)data;
}
