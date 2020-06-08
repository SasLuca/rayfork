import re

path = "../source/"
libs_path = "../source/libs/"


def find_all(src: str, target: str):
    return [it.start() for it in re.finditer(target, src)]


def read_file(f: str):
    if '/' in f: return f"\n\n#pragma region include {f} \n\n" + open(libs_path + f, "r").read() + "\n\n#pragma endregion\n\n"
    else: return f"\n\n#pragma region include {f} \n\n" + open(path + f, "r").read() + "\n\n#pragma endregion\n\n"


includes = [ "rayfork_base.h", "rayfork_internal_base.h", "rayfork_gfx_backend_gl.h",
             "cgltf/cgltf.h", "par/par_shapes.h", "stb/stb_image.h", "stb/stb_image_resize.h",
             "stb/stb_perlin.h", "stb/stb_rect_pack.h", "stb/stb_truetype.h", "tinyobjloader-c/tinyobj_loader_c.h" ]

result = read_file("rayfork.h") + \
         "\n#ifdef RAYFORK_IMPLEMENTATION\n" + \
         read_file("rayfork.c") + \
         read_file("rayfork_gfx_backend_gl.c") + \
         "\n#endif //RAYFORK_IMPLEMENTATION"

result = re.sub(f"#include \"rayfork.h\"", "//#include \"rayfork.h\"", result)

for inc in includes:
    result = result.replace(f"#include \"{inc}\"", read_file(inc))

for inc in includes:
    to_replace = f"#include \"{inc}\""
    result = result.replace(to_replace, f"//{to_replace}")

open("../generated/rayfork.h", "w").write(result)