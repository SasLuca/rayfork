import os
import re

paths = [ "../source/", "../source/gfx/", "../source/libs/" ]

includes = [ "rayfork_public_base.h", "rayfork_internal_base.h", "rayfork_gfx_backend_gl.h",
             "rayfork_gfx_backend_gl.c", "cgltf/cgltf.h", "par/par_shapes.h", "stb/stb_image.h", "stb/stb_image_resize.h",
             "stb/stb_perlin.h", "stb/stb_rect_pack.h", "stb/stb_truetype.h", "tinyobjloader-c/tinyobj_loader_c.h" ]


def find_all(src: str, target: str):
    return [it.start() for it in re.finditer(target, src)]


def read_code_file(f: str):
    for p in paths:
        file_path = p + f
        if os.path.exists(file_path):
            return f"\n#pragma region include {f}\n" + open(file_path, "r").read() + "\n#pragma endregion\n"
    return ""


def file_write(f: str, contents: str):
    os.makedirs(os.path.dirname(f), exist_ok=True)
    open(f, "w").write(contents)


def include_files(f):
    for inc in includes:
        f = f.replace(f"#include \"{inc}\"", read_code_file(inc))

    for inc in includes:
        to_replace = f"#include \"{inc}\""
        f = f.replace(to_replace, f"//{to_replace}")

    return f

header = read_code_file("rayfork.h")
impl = read_code_file("rayfork.c")
license = open("../LICENSE", "r").read()

header = include_files(header)
impl = include_files(impl)

# Generate single header
sh_impl = re.sub(f"#include \"rayfork.h\"", "//#include \"rayfork.h\"", impl) # comment #include "raylib.h" in the single header version
file_write("../generated/single-header/rayfork.h", f"{header}\n#ifdef RAYFORK_IMPLEMENTATION\n{sh_impl}\n#endif //RAYFORK_IMPLEMENTATION\n/*\n{license}\n*/")

# Generate rayfork-es
rf_es_suffix = open("../source/rayfork_es_suffix.h", "r").read()
sh_impl = re.sub(f"#include \"rayfork.h\"", "//#include \"rayfork.h\"", impl) # comment #include "raylib.h" in the single header version
file_write("../generated/rayfork-es/rayfork_es.h", f"\n{header}\n#ifdef RAYFORK_IMPLEMENTATION\n{sh_impl}\n#endif //RAYFORK_IMPLEMENTATION\n{rf_es_suffix}\n/*\n{license}\n*/")

# Generate single header + source
file_write("../generated/single-header-and-source/rayfork.h", f"{header}\n/*\n{license}\n*/")
file_write("../generated/single-header-and-source/rayfork.c", f"{impl}\n/*\n{license}\n*/")