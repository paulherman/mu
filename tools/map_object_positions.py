#! /usr/bin/env python3
import sys
import json
import os
import math
import struct

from xor_decryptor import xor_decrypt

def parse_map_obj(path):
    if os.path.isfile(path): 
        with open(path, 'rb') as input_file:
            data = xor_decrypt(input_file.read())
            if len(data) < 4:
                raise Exception('Map {} has invalid length {}'.format(path, len(data)))

            (map_id, num_objects) = struct.unpack_from('hh', data)

            if num_objects < 0 or 4 + 30 * num_objects != len(data):
                raise Exception('Map {} should have {} objects, but has {}'.format(path, int((len(data) - 4) / 30), num_objects))

            objects = []
            for i in range(num_objects):
                [object_id] = struct.unpack_from('h', data, 4 + i * 30)
                (x, z, y, rx, rz, ry, s) = struct.unpack_from('7f', data, 4 + i * 30 + 2)
                objects.append({
                    'object_id': object_id,
                    'position': [x / 100.0, y / 100.0, -z / 100.0],
                    'rotation': [math.fmod(math.radians(rx), 2 * math.pi), math.fmod(math.radians(ry), 2 * math.pi), math.fmod(math.radians(-rz), 2 * math.pi)],
                    'scale': s / 100.0
                })
            return objects
    else:
        raise Exception('File {} does not exist.'.format(path))

if __name__ == '__main__':
    if len(sys.argv) == 4:
        world_path = sys.argv[1]
        map_id = int(sys.argv[2])
        output_path = sys.argv[3]
        map_obj_path = os.path.join(world_path, 'EncTerrain{}.obj'.format(map_id))

        try:
            converted_map_objects = parse_map_obj(map_obj_path)
            with open(output_path, 'w') as output_file:
                output_file.write(json.dumps(converted_map_objects, indent = 2))
        except Exception as e:
            print(e)
    else:
        print('{} <world_path> <map_id> <output>'.format(sys.argv[0]))
