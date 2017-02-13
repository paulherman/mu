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

if __name__ == '__main__':
    for path in sys.argv[1:]:
        if os.path.isfile(path): 
            output_path, extension = os.path.splitext(path)
            extension = extension.lower()
            if extension in known_extensions:
                extension_info = known_extensions[extension]
                output_path = output_path + extension_info['extension']
                skip_length = extension_info['skip']

                with open(path, 'rb') as input_file:
                    data = input_file.read()
                    with open(output_path, 'wb') as output_file:
                        output_file.write(data[skip_length:])
                        print('Converted file {} to {}'.format(path, output_path))
            else:
                print('File {} has an unrecognized format.'.format(path))
        else:
            print('File {} does not exist.'.format(path))
