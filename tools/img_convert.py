#! /usr/bin/env python3
import sys
import os

known_extensions = {
    '.ozj': {
        'extension': '.jpg',
        'skip': 24
    }, '.ozt': {
        'extension': '.tga',
        'skip': 4
    }, '.ozb': {
        'extension': '.bmp',
        'skip': 4
    }
}

def convert_img(path, output_path_without_extension):
    if os.path.isfile(path): 
        output_path, extension = os.path.splitext(path)
        extension = extension.lower()
        if extension in known_extensions:
            extension_info = known_extensions[extension]
            output_path = output_path_without_extension + extension_info['extension']
            skip_length = extension_info['skip']

            with open(path, 'rb') as input_file:
                data = input_file.read()
                with open(output_path, 'wb') as output_file:
                    output_file.write(data[skip_length:])
                    return output_path
        else:
            raise Exception('File {} has an unrecognized format.'.format(path))
    else:
        raise Exception('File {} does not exist.'.format(path))

if __name__ == '__main__':
    for path in sys.argv[1:]:
        output_path_without_extension, extension = os.path.splitext(path)
        output_path = convert_img(path, output_path_without_extension)
        print('Converted {} to {}'.format(path, output_path))
