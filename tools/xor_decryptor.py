#! /usr/bin/env python3

import sys
import os
from ctypes import c_ubyte

keys = [0xd1, 0x73, 0x52, 0xf6, 0xd2, 0x9a, 0xcb, 0x27, 0x3e, 0xaf, 0x59, 0x31, 0x37, 0xb3, 0xe7, 0xa2]

def xor_decrypt(data):
    dec_data = []
    position = 0
    key = 0x5e 
    for i in range(len(data)):
        b = data[i]
        dec_key = keys[i % len(keys)]
        dec_b = (b ^ dec_key) - key
        dec_data.append(dec_b % 256)
        key = (b + 0x3d) % 256 
    return bytes(dec_data)

def xor_decrypt_file(path, output_path):
    if os.path.isfile(path): 
        with open(path, 'rb') as input_file:
            data = xor_decrypt(input_file.read())
            with open(output_path, 'wb') as output_file:
                output_file.write(data)
    else:
        raise Exception('File {} does not exist.'.format(path))

if __name__ == '__main__':
    for path in sys.argv[1:]:
        output_path, extension = os.path.splitext(path)
        output_path = output_path + '_dec' + extension
        xor_decrypt_file(path, output_path)
        print('Decrypted {} to {}'.format(path, output_path))
