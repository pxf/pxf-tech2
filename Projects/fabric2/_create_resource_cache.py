import os

IGNORE_FILTER = ['.svn']
DATA_FOLDERS=['data', 'jam']

def get_resource_list(root):
    resource_list = []
    for root, dirs, files in os.walk(root):
        if any([ignore in root for ignore in IGNORE_FILTER]):
            continue
        for file in files:
            path = os.path.join(root, file)
            resource_list.append(path,)
    return resource_list

def mangle_filename(path):
    return path.replace(os.path.sep, "_").replace(".", "_")
    
def create_data_file(resources):
    data_file = "#ifndef _PRELOADEDRESOURCES_\n"\
                "#define _PRELOADEDRESOURCES_\n\n"
    
    data_file += "struct PreloadedFile\n"\
                 "{\n"\
                 "\tconst char * path;\n"\
                 "\tunsigned size;\n"\
                 "\tconst unsigned char* data;\n"\
                 "};\n\n"

    def hexify(name, data):
        chunk_size = len(data)
        hex_chunk = ["0x%x"%byte + ("\n\t" if index % 20 == 0 else "") 
                     for index, byte in enumerate(data)]
        hex_data = "const unsigned char %s[] = {" % name
        hex_data += ",".join(hex_chunk)
        hex_data += "};\n"
        return hex_data, chunk_size

    file_size_map = {}
    for file in resources:
        mangled_name = mangle_filename(file)
        raw_data = bytearray(open(file, "rb").read())
        hex_chunk, chunk_size = hexify(mangled_name, raw_data)
        file_size_map[file] = chunk_size
        data_file += hex_chunk
        
    data_file += "\n"
    data_file += "PreloadedFile preloaded_files[] = {"
    for file in resources:
        double_sep = os.path.sep + os.path.sep
        data_file += '{"%s", %d, %s},\n' % (file.replace(os.path.sep, "/")
                                           ,file_size_map[file]
                                           ,mangle_filename(file))
    data_file += "};\n"
    data_file += "#endif\n"

    return data_file

def main():
    resources = []
    for datadirs in DATA_FOLDERS:
        resources += get_resource_list(datadirs)
    print resources
    data_file = create_data_file(resources)
    with open("PreloadedResources.h", "w") as f:
        f.write(data_file)
    print("Done.")

if __name__ == '__main__':
    main()