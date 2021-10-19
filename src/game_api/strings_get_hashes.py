import re

import sys

sys.stdout = open("string_hashes.cpp", "w")

strings_path = "../../docs/game_data/strings00_hashed.str"

print("#include \"strings.hpp\"")
print("")
print("const std::map<uint32_t, STRINGID> string_hashes = {")

data = open(strings_path, "r").read().split("\n")
line_number = 0
for line in data:
    if line == "":
        continue
    if line[0] == '#':
        continue
    print("    {" + line[0:10] + ", " + str(line_number) + "},")
    line_number += 1

print("};")
