import os
import re


def remove_comments(string):
    string = re.sub(re.compile("/\*.*?\*/", re.DOTALL), "",
                    string)  # remove all occurrences streamed comments (/*COMMENT */) from string
    string = re.sub(re.compile("//.*?\n"), "",
                    string)  # remove all occurrence single-line comments (//COMMENT\n ) from string
    return string


def is_camel_case(s):
    return s != s.lower() and s != s.upper() and "_" not in s


pattern = re.compile(r'(?<!^)(?=[A-Z])')


def to_snake_case(it: str) -> str:
    it = it.replace('UV', 'Uv') if 'UV' in it else it
    it = it.replace('VAO', 'Vao') if 'VAO' in it else it
    it = it.replace('VBO', 'Vbo') if 'VBO' in it else it
    it = it.replace('MVP', 'Mvp') if 'MVP' in it else it
    it = it.replace('POT', 'Pot') if 'POT' in it else it
    it = it.replace('2D', '2d')
    return pattern.sub('_', it).lower()


f = open("../rayfork/include/old_rayfork.h", "r").read()
words = list(set(re.findall(r'\w+', re.sub(r'<"+?>', '', remove_comments(f)))))

result = []

for w in words:
    if is_camel_case(w) and ('gl' not in w) and ('GL' not in w) and (not w[0].isupper()) and w[0] != '0':
        result += w
        print(f'{w} -> {to_snake_case(w)}')
        #f = f.replace(w, to_snake_case(w))

#open("rayfork/rayfork.h", "w+").write(f)
