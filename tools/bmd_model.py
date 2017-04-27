#! /usr/bin/env python3
import sys
import json
import os
import math
import struct
from pyassimp import *
from ctypes import *

from xor_decryptor import xor_decrypt

if __name__ == '__main__':
    if len(sys.argv) == 3:
        bmd_path = sys.argv[1]
        output_path = sys.argv[2]
        scene = structs.Scene() 

        if os.path.isfile(bmd_path): 
            with open(bmd_path, 'rb') as input_file:
                data = input_file.read()

                [bmd_type] = struct.unpack_from('I', data)
                if bmd_type == 0x0a444d42:
                    data = data[4:]
                elif bmd_type == 0x0c444d42:
                    [encode_size] = struct.unpack_from('I', data, 4)
                    data = xor_decrypt(data[8:])
                else:
                    raise Exception('File {} is not a valid BMD model - signature {}'.format(path, bmd_type))

                [filename] = struct.unpack_from('32s', data, 0)
                filename = filename.decode('utf-8')

                (num_objects, num_bones, num_animations) = struct.unpack_from('HHH', data, 32)
                data = data[32 + 6:]

                meshes = []
                for i in range(num_objects):
                    (num_positions, num_normals, num_uvs, num_triangles, object_id) = struct.unpack_from('HHHHH', data)
                    offset = 10

                    positions = []
                    for j in range(num_positions):
                        (bone, x, y, z) = struct.unpack_from('Ifff', data, offset)
                        offset += 16
                        positions.append({
                            'bone': bone,
                            'vec': [x, y, -z]
                        })

                    normals = []
                    for j in range(num_normals):
                        (bone, x, y, z, _) = struct.unpack_from('IfffI', data, offset)
                        offset += 20
                        normals.append({
                            'bone': bone,
                            'vec': [x, y, -z]
                        })

                    uvs = []
                    for j in range(num_uvs):
                        (u, v) = struct.unpack_from('ff', data, offset)
                        offset += 8
                        uvs.append([u, 1.0 - v])

                    fixed_positions = []
                    fixed_normals = []
                    fixed_uvs = []
                    vertices = []
                    for j in range(num_triangles):
                        (_, p0, p1, p2, _, n0, n1, n2, _, uv0, uv1, uv2) = struct.unpack_from('12H40x', data, offset)
                        offset += 64

                        p = [p0, p1, p2]
                        n = [n0, n1, n2]
                        t = [uv0, uv1, uv2]
                        for k in range(3):
                            fixed_positions.append(positions[p[k]])
                            fixed_normals.append(normals[n[k]])
                            fixed_uvs.append(uvs[t[k]])
                            vertices.append(3 * j + k)
                        
                    [texture] = struct.unpack_from('32s', data, offset)
                    texture = texture.decode('ascii')
                    offset += 32

                    data = data[offset:]

                    assimp_mesh = structs.Mesh()
                    assimp_mesh.vertices = fixed_positions
                    assimp_mesh.normals = fixed_normals

                    uvws = []
                    for uv in fixed_uvs:
                        uvw = structs.Vector3D()
                        uvw.x = uv[0]
                        uvw.y = uv[1]
                        uvw.z = 0.0
                        uvws.append(uvw)

                    assimp_mesh.mNumUVComponents[0] = 2
                    assimp_mesh.mTextureCoords[0] = (structs.Vector3D * len(fixed_uvs))(*uvws)
                    meshes.append(assimp_mesh)

                pointer_meshes = []
                for mesh in meshes:
                    pointer_meshes.append(pointer(mesh))

                scene.mMeshes = (POINTER(structs.Mesh) * len(meshes))(*pointer_meshes)

                offset = 0
                animations = []
                for i in range(num_animations):
                    (num_frames, empty) = struct.unpack_from('H?', data, offset)
                    offset += 3

                    frames = []
                    if not empty:
                        for j in range(num_frames):
                            (x, y, z) = struct.unpack_from('fff', data, offset)
                            offset += 12
                            frames.append([x, y, z])

                    animations.append({
                        'empty': empty,
                        'frames': frames
                    })

                for i in range(num_bones):
                    pass

            #export(scene, output_path, 'collada')
        else:
            print('File {} does not exist.'.format(path))
    else:
        print('{} <bmd path> <output path>'.format(sys.argv[0]))
