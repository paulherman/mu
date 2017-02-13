#include "xor_decrypt.h"


void xor_decrypt(struct file_buffer *file, size_t offset) {
  const uint8_t keys[16] = {
    0xd1, 0x73, 0x52, 0xf6, 0xd2, 0x9a, 0xcb, 0x27, 0x3e, 0xaf, 0x59, 0x31, 0x37, 0xb3, 0xe7, 0xa2
  };

	uint8_t key = 0x5E;
  for (size_t i = offset; i < file->length; i++) {
		uint8_t encode = file->data[i];
    file->data[i] = (file->data[i] ^ keys[i % 16]) - key;
		key = encode + 0x3D;
	}
}
