def get_funcs_that_depend_on(fn, lines):
    result = []
    occs = [lines.index(j) for j in lines if ("|-FunctionDecl" not in j and fn in j)]
    for occ in occs:
        f = [j for j in lines[:occ] if j.startswith("|-FunctionDecl")][-1]
        fnarr = [name for name in f.split(" ") if name.startswith("stb")]
        if len(fnarr) != 0:
            result.append(fnarr[0])

    return result


def main():
    f = open("./stb_truetype_ast.txt").read()
    lines = f.splitlines()
    mi = [lines.index(it) for it in lines if "malloc" in it]

    malloc_funcs = []

    for i in mi:
        func = [j for j in lines[:i] if j.startswith("|-FunctionDecl")][-1]
        funcnamearr = [name for name in func.split(" ") if name.startswith("stb")]
        if len(funcnamearr) != 0:
            malloc_funcs.append(funcnamearr[0])

    nodups = True

    funcs = malloc_funcs

    while nodups:
        temp_funcs = []
        for i in funcs:
            fns = get_funcs_that_depend_on(i, lines)
            temp_funcs += list(set(fns))
        if set(temp_funcs) == set(funcs):
            nodups = False
        else:
            funcs += temp_funcs
            funcs = list(set(temp_funcs))

    print(funcs)


main()