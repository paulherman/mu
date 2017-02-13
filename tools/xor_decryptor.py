#! /usr/bin/env python3

import sys
import os
from ctypes import c_ubyte

keys = [0xd1, 0x73, 0x52, 0xf6, 0xd2, 0x9a, 0xcb, 0x27, 0x3e, 0xaf, 0x59, 0x31, 0x37, 0xb3, 0xe7, 0xa2]
initial_key = 0x5e
delta_key = 0x3d

if __name__ == '__main__':
    for path in sys.argv[1:]:
        if os.path.isfile(path): 
            with open(path, 'rb') as input_file:
                data = input_file.read()

                dec_data = []
                position = 0
                key = initial_key
                for b in data:
                    dec_data.append(c_ubyte(int(b) ^ keys[position % len(keys)] - key))
                    key = int(b) + delta_key
                dec_data = bytes(map(lambda b: b.value, dec_data))

                output_path, extension = os.path.splitext(path)
                output_path = output_path + '_dec' + extension

                with open(output_path, 'wb') as output_file:
                    output_file.write(data)
        else:
            print('File {} does not exist.'.format(path))
