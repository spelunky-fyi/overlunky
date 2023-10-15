import re

import parse_source as ps

#should be the same as in generate.py
replace_table = {
    "uint8_t": "int",
    "uint16_t": "int",
    "uint32_t": "int",
    "uint64_t": "int",
    "int8_t": "int",
    "int16_t": "int",
    "int32_t": "int",
    "int64_t": "int",
    "ImU32": "int",
    "in_port_t": "int",
    "size_t": "int",
    "custom_vector<": "vector<",
    "span<": "array<",
    "unordered_map<": "map<",
    "game_map<": "map<",
    "custom_map<": "map<",
    ", identity_hasher<>": "",
    "char*": "string",
    "wstring": "string",
    "u16string": "string",
    "string_view": "string",
    "char16_t*": "string",
    "char16_t": "char",
    "pair<": "tuple<",
    "std::": "",
    "sol::": "",
    "void": "",
    "constexpr": "",
    "const": "",
    "static": "",
    "variadic_args va": "ENT_TYPE, ENT_TYPE...",
    "EmittedParticlesInfo": "array<Particle>",
    "ImVec2": "Vec2",
    "SoundCallbackFunction": "function",
    "object ": "any ",
}

def replace_all(text):
    for repl, wth in replace_table.items():
        pos = text.find(repl)
        br2 = text.find("`", pos + len(repl))
        br1 = text.rfind("`", 0, pos)
        if pos > 0 and br1 >= 0 and br2 > 0:
            continue
        text = text.replace(repl, wth)
    return text

ps.configure_parse(replace_all, "slate.pickle")
ps.run_parse()

# types that does not need explanation, ideally we don't want to expand this list too much
known_types = [
    "array",
    "table",
    "optional",
    "mixed",
    "int",
    "float",
    "string",
    "function",
    "bool",
    "tuple",
    "variadic_args",
    "MAX_PLAYERS",
    "any",
    "IMAGE",
    "VTABLE_OFFSET",
]

unknown_types = []

def check_types(ret):
    parts = re.findall(r"(?:(?<=^)|(?<=,|<|\()|(?<=,\s))[a-zA-Z_][\w]+(?=$|[ ,<>])", ret)
    if(ret and ret != "nil"):
        for  part in parts:
            add_part = True
            for type in ps.types:
                if (part == type["name"]):
                    add_part = False
            for enum in ps.enums:
                if (part == enum["name"]):
                    add_part = False
            for alias in ps.aliases:
                if (part == alias["name"]):
                    add_part = False
            if(add_part == True and not part in unknown_types and not part in known_types):
                unknown_types.insert(0, part)

for lf in ps.funcs + ps.deprecated_funcs:
    if not (lf["name"].startswith("on_") or lf["name"] in ps.not_functions):
        if lf["comment"] and "NoDoc" in lf["comment"][0]:
            continue
        m = re.search(r"\(([^\{]*)\)\s*->\s*([^\{]*)", lf["cpp"])
        m2 = re.search(r"\(([^\{]*)\)", lf["cpp"])
        ret = "nil"
        param = ""
        if m:
            ret = m.group(2) or "nil"
        if m or m2:
            param = (m or m2).group(1)
            
        check_types(ret)
        check_types(param)

for type in ps.types:
    if "comment" in type and "NoDoc" in type["comment"]:
        continue
    type_name = type["name"]

    ctors = (
        [] if type_name not in ps.constructors else ps.constructors[type_name]
    )
    for var in ctors + type["vars"]:
        if "comment" in var and "NoDoc" in var["comment"]:
            continue
        var_name = var["name"]
        if "signature" in var:
            signature = var["signature"].strip()
            ret = ""
            if var_name == type_name:
                ret = type_name
            else:
                n = re.search(r"^\s*([^\( ]*)(\(([^\)]*))", signature)
                m = re.search(
                    r"\s*([^\(]*)\s+([^\( ]*)(\(([^\)]*))?", signature
                )

                param = ""

                if n:
                    if n.group(2):
                        param = n.group(2)
                elif m:
                    ret = m.group(1) or "nil"
                    if m.group(3):
                        param = m.group(3)

            if(param):
                check_types(param)
            if(ret):
                check_types(ret)

if(len(unknown_types) > 0):
    print("Found types not documented in the API")
    print(unknown_types)
    exit(1)
else:
    print("No Problems found")
