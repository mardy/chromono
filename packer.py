#!/usr/bin/env python3
# pack_shaders.py: Convert multiple files to C source code
# Thomas Perl <m@thp.io>; 2013-04-29

import sys
import os
import re
import zlib

if len(sys.argv) < 3:
    raise SystemExit('Missing parameters')

shader_files = sys.argv[1:-1]
output_file = sys.argv[-1]
header_file = output_file.replace('.cpp', '.h')

def mangle(filename):
    return os.path.basename(filename).replace('.', '_')

def compact_shader_line(line):
    pos = line.find(b'//')
    if pos != -1:
        line = line[:pos]
    line = line.strip()
    return line

def pack_shader(data):
    result = []
    for line in data.splitlines():
        line = compact_shader_line(line)
        if line:
            result.append(line)

    return re.sub(b'\\s+', b' ', b'\n'.join(result))

def get_data(filename):
    data = open(filename, 'rb').read()
    original_size = len(data)

    _, extension = os.path.splitext(filename)
    if extension in ('.vsh', '.fsh'):
        data = pack_shader(data)
        data = re.sub(r'\/\*.*\*\/', '', data.decode()).encode()

    uncompressed_size = len(data)
    data = zlib.compress(data, 9)
    compressed_size = len(data)
    return data, uncompressed_size

def update_file(filename, contents):
    fp = open(filename + '.tmp', 'w')
    fp.write(contents)
    fp.close()
    os.rename(filename + '.tmp', filename)

def to_char_array(data):
    COLUMNS = 13

    result = []
    line = []

    for i, c in enumerate(data):
        line.append('0x%02x,' % c)
        if i % COLUMNS == (COLUMNS-1):
            result.append(''.join(line))
            line = []
    result.append(''.join(line))

    return '\n'.join(result)

output = []
header = []

header.append('#pragma once')
header.append('#include "resources_util.h"')
output.append('#include "resources_util.h"')
for filename in shader_files:
    mangled = mangle(filename)
    data, uncompressed_size = get_data(filename)
    fn = os.path.basename(filename)
    compressed_size = len(data)
    data_c_source = to_char_array(data)
    output.append(f'const unsigned char {mangled}_data[] = {{\n{data_c_source}\n}};')
    output.append(f"""Resource {mangled} = {{
    "{mangled}",
    {mangled}_data,
    {compressed_size},
    {uncompressed_size},
}};""")
    header.append(f'extern Resource {mangled};')

update_file(output_file, '\n'.join(output))
update_file(header_file, '\n'.join(header))
