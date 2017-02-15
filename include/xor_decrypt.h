#ifndef XOR_DECRYPT_H
#define XOR_DECRYPT_H

#include "filebuf.h"

void xor_decrypt(struct file_buffer *file, size_t offset);
void xor3_decrypt(struct file_buffer *file, size_t offset);

#endif
