import re

# redirect stdout to script-api.md
import sys

sys.stdout = open("game_data/spel2.lua", "w")

header_files = [
    "../src/game_api/math.hpp",
    "../src/game_api/rpc.hpp",
    "../src/game_api/spawn_api.hpp",
    "../src/game_api/script.hpp",
    "../src/game_api/color.hpp",
    "../src/game_api/entity.hpp",
    "../src/game_api/movable.hpp",
    "../src/game_api/game_manager.hpp",
    "../src/game_api/state.hpp",
    "../src/game_api/state_structs.hpp",
    "../src/game_api/prng.hpp",
    "../src/game_api/entities_floors.hpp",
    "../src/game_api/entities_activefloors.hpp",
    "../src/game_api/entities_mounts.hpp",
    "../src/game_api/entities_monsters.hpp",
    "../src/game_api/entities_chars.hpp",
    "../src/game_api/entities_items.hpp",
    "../src/game_api/entities_fx.hpp",
    "../src/game_api/entities_liquids.hpp",
    "../src/game_api/entities_backgrounds.hpp",
    "../src/game_api/entities_decorations.hpp",
    "../src/game_api/entities_logical.hpp",
    "../src/game_api/sound_manager.hpp",
    "../src/game_api/render_api.hpp",
    "../src/game_api/particles.hpp",
    "../src/game_api/savedata.hpp",
    "../src/game_api/level_api.hpp",
    "../src/game_api/level_api_types.hpp",
    "../src/game_api/items.hpp",
    "../src/game_api/screen.hpp",
    "../src/game_api/screen_arena.hpp",
    "../src/game_api/online.hpp",
    "../src/game_api/strings.hpp",
    "../src/game_api/script/usertypes/level_lua.hpp",
    "../src/game_api/script/usertypes/gui_lua.hpp",
    "../src/game_api/script/usertypes/vanilla_render_lua.hpp",
    "../src/game_api/script/usertypes/save_context.hpp",
    "../src/game_api/script/usertypes/hitbox_lua.hpp",
    "../src/game_api/script/usertypes/socket_lua.hpp",
    "../src/imgui/imgui.h",
    "../src/game_api/script/usertypes/level_lua.cpp",
    "../src/game_api/script/usertypes/gui_lua.cpp",
]
api_files = [
    "../src/game_api/script/script_impl.cpp",
    "../src/game_api/script/script_impl.hpp",
    "../src/game_api/script/lua_vm.cpp",
    "../src/game_api/script/lua_vm.hpp",
    "../src/game_api/script/lua_backend.cpp",
    "../src/game_api/script/lua_backend.hpp",
    "../src/game_api/script/usertypes/save_context.cpp",
    "../src/game_api/script/usertypes/state_lua.cpp",
    "../src/game_api/script/usertypes/prng_lua.cpp",
    "../src/game_api/script/usertypes/entity_lua.cpp",
    "../src/game_api/script/usertypes/entities_chars_lua.cpp",
    "../src/game_api/script/usertypes/entities_floors_lua.cpp",
    "../src/game_api/script/usertypes/entities_activefloors_lua.cpp",
    "../src/game_api/script/usertypes/entities_mounts_lua.cpp",
    "../src/game_api/script/usertypes/entities_monsters_lua.cpp",
    "../src/game_api/script/usertypes/entities_items_lua.cpp",
    "../src/game_api/script/usertypes/entities_fx_lua.cpp",
    "../src/game_api/script/usertypes/entities_liquids_lua.cpp",
    "../src/game_api/script/usertypes/entities_backgrounds_lua.cpp",
    "../src/game_api/script/usertypes/entities_decorations_lua.cpp",
    "../src/game_api/script/usertypes/entities_logical_lua.cpp",
    "../src/game_api/script/usertypes/particles_lua.cpp",
    "../src/game_api/script/usertypes/level_lua.cpp",
    "../src/game_api/script/usertypes/sound_lua.cpp",
    "../src/game_api/script/usertypes/player_lua.cpp",
    "../src/game_api/script/usertypes/gui_lua.cpp",
    "../src/game_api/script/usertypes/gui_lua.hpp",
    "../src/game_api/script/usertypes/vanilla_render_lua.cpp",
    "../src/game_api/script/usertypes/vanilla_render_lua.hpp",
    "../src/game_api/script/usertypes/drops_lua.cpp",
    "../src/game_api/script/usertypes/texture_lua.cpp",
    "../src/game_api/script/usertypes/flags_lua.cpp",
    "../src/game_api/script/usertypes/char_state_lua.cpp",
    "../src/game_api/script/usertypes/hitbox_lua.cpp",
    "../src/game_api/script/usertypes/screen_lua.cpp",
    "../src/game_api/script/usertypes/screen_arena_lua.cpp",
    "../src/game_api/script/usertypes/socket_lua.cpp",
]
rpc = []
classes = []
events = []
funcs = []
types = []
known_casts = []
aliases = []
lualibs = []
enums = []
constructors = []
replace = {
    #"nil": "void",
    #"bool": "boolean",
    "uint8_t": "integer",
    "uint16_t": "integer",
    "uint32_t": "integer",
    "uint64_t": "integer",
    "int8_t": "integer",
    "int16_t": "integer",
    "int32_t": "integer",
    "int64_t": "integer",
    "ImU32": "integer",
    "vector": "Array",
    "array": "Array",
    "unordered_map": "table",
    "const char*": "string",
    "wstring": "string",
    "u16string": "string",
    "char16_t": "string",
    "string_view": "string",
    "pair": "tuple",
    "std::": "",
    "sol::": "",
    #"AABB&: const": "AABB",
    "function": "fun(): any",
    " = nullopt": "",
    #"const Color&": "color: Color",
    "void": "",
    "constexpr": "",
    #"static": "",
    #"variadic_args va": "...ent_type: number[]",
    "...va:": "...ent_type:",
    "// Access via": "ImGuiIO",
    "set<": "Array<",
    "&": "",
    "const string": "string",
    "ShopType": "SHOP_TYPE",

}
comment = []
not_functions = [
    "players",
    "state",
    "game_manager",
    "online",
    "savegame",
    "options",
    "meta",
    "prng",
]
cpp_type_exceptions = []
skip = False


def getfunc(name):
    for func in funcs:
        if func["name"] == name:
            return func
    return False


def rpcfunc(name):
    ret = []
    for func in rpc:
        if func["name"] == name:
            ret.append(func)
    return ret

#old reArr: r"(Array<(?:(?:\w*<\w*, \d>)|(?:\w+))), [^>]*?(>+)"
reArr = re.compile(r"(\bArray<(?:(\w+<.+>)|(\w+(?:<.+>)?))(?:, )?.*>)")
reTuple = re.compile(r"tuple<(.*?)>")
reOptional = re.compile(r"optional<(.+?)>")
reBool = re.compile(r"bool\b")
reMap = re.compile(r"\bmap<")
reFloat = re.compile(r"\bfloat\b")
reInt = re.compile(r"\bint\b")
def replace_all(text, dic):
    for i, j in dic.items():
        pos = text.find(i)
        br2 = text.find('`', pos + len(i))
        br1 = text.rfind('`', 0, pos)
        if pos > 0 and br1 >= 0 and br2 > 0:
            continue
        text = text.replace(i, j)
    text = reFloat.sub("number", text)
    text = reInt.sub("integer", text)
    text = reMap.sub("table<", text)
    if "Array<" in text: #Array<Array<float, 2>, MAX_PLAYERS>
        newText = text
        dimension = 1
        while True:
            m = reArr.search(newText)
            if m:
                if m[2]:
                    dimension += 1
                    newText = m[2]
                elif m[3]:
                    newText = f"{m[3]}" + ("[]"*dimension)
                    text = reArr.sub(newText, text)
                    break
            else:
                break
            #newText = reArr.sub(r"FixedSizeArray\1", text)
            #if newText == text:
            #    break
            #text = newText

    #text = reTuple.sub(r"LuaMultiReturn<[\1]>", text)
    text = reBool.sub("boolean", text)
    text = reOptional.sub(r"\1?", text)
    #match = re.search(r"(Array<.*), .*>", text)
    #if match:
    #    text = text.replace(match.group(0), match.group(1) + ">")
    #else:
    #    match = re.search(r"tuple<(.*)>", text)
    #    if match:
    #       text = text.replace(match.group(0), f"[{match.group(1)}]")
    return text

reGetParam = re.compile(r"(?!const)(\b[^ ]+) *([^,]+),?")#r"([^ ]+) *([^,]+),?")
reRemoveDefault = re.compile(r" = .*")
reHandleConst = re.compile(r"const (\w+) (\w+)")
def cpp_params_to_emmy_lua(params_text):
    return_typed = ""
    return_normal = ""
    params_iterator = reGetParam.finditer(params_text)
    for param_match in params_iterator:
        p_type = param_match.group(1)
        p_name = param_match.group(2)
        p_name = reRemoveDefault.sub("", p_name)
        if p_type == "sol::variadic_args":
            return_typed += f"\n---@vararg any"
            return_normal += f"..."
        else:
            return_typed += "\n---@"
            if m := reHandleConst.match(p_name):
                p_type = m.group(1)
                p_name = m.group(2)
            return_typed += f"param {p_name} {p_type}"
            return_normal += p_name
        return_normal += ", "
    return return_typed, return_normal[0:-2]

def cpp_params_to_emmy_lua_fun(params_text):
    ret = ""
    params_iterator = reGetParam.finditer(params_text)
    for param_match in params_iterator:
        p_type = param_match.group(1)
        p_name = param_match.group(2)
        p_name = reRemoveDefault.sub("", p_name)
        if m := reHandleConst.match(p_name):
            p_type = m.group(1)
            p_name = m.group(2)
        ret += f"{p_name}: {p_type}, "
    return ret[:-2]


reConstructorFix = re.compile(r"const (\w+)(?: \w+)?")
def fix_constructor_param(params_text):
    return reConstructorFix.sub(r"\1 \1", params_text)

def fix_return(ret):
    tuple_contents = reTuple.match(ret)
    if tuple_contents:
        ret = f"{tuple_contents.group(1)}"
    else:
        ret = f"{ret}"
    return ret

def print_func(name, params, ret, typed_params):
    ret = fix_return(ret)
    fun = f"{typed_params}\n---@return {ret}\nfunction {name}({params}) end".strip()
    print(fun)

def print_comment(lf):
    if lf["comment"]:
        for com in lf["comment"]:
            print(f"---{com}")

def print_af(lf, af):
    if lf["comment"] and lf["comment"][0] == "NoDoc":
        return
    ret = replace_all(af["return"], replace) or "nil"
    name = lf["name"]
    typed_params, params = cpp_params_to_emmy_lua(af["param"])
    typed_params.strip()
    typed_params = replace_all(typed_params, replace)
    print_comment(lf)
    print_func(name, params, ret, typed_params)


for file in header_files:
    comment = []
    data = open(file, "r").read().split("\n")
    skip = 0
    for line in data:
        line = line.replace("*", "")
        skip += line.count("{") - line.count("}")
        c = re.search(r"/// ?(.*)$", line)
        if c:
            comment.append(c.group(1))
        m = re.search(r"\s*(.*)\s+([^\(]*)\(([^\)]*)", line)
        if m:
            if skip == 0 or file.endswith("script.hpp"):
                rpc.append(
                    {
                        "return": m.group(1),
                        "name": m.group(2),
                        "param": m.group(3),
                        "comment": comment,
                    }
                )
        else:
            comment = []

for file in header_files:
    if file.endswith("script.hpp"):
        continue
    data = open(file, "r").read().split("\n")
    brackets_depth = 0
    in_union = False
    in_anonymous_struct = False
    class_name = None
    comment = []
    member_funs = {}
    member_vars = []
    for line in data:
        line = replace_all(line, replace)
        line = line.replace("*", "")
        if not class_name and ("struct" in line or "class" in line):
            m = re.match(r"(struct|class)\s+(\S+)", line)
            if m:
                class_name = m[2]
        elif class_name:
            prev_brackets_depth = brackets_depth
            brackets_depth += line.count("{") - line.count("}")

            if brackets_depth == 1:
                if line.strip() == "union":
                    in_union = True
            if brackets_depth == 2 and in_union:
                if line.strip() == "struct":
                    in_anonymous_struct = True

            if brackets_depth < prev_brackets_depth:
                if brackets_depth == 2:
                    in_anonymous_struct = False
                if brackets_depth == 1:
                    in_union = False

            if (
                brackets_depth == 1
                or (brackets_depth == 2 and in_union)
                or (brackets_depth == 3 and in_anonymous_struct)
            ):
                m = re.search(r"/// ?(.*)$", line)
                if m:
                    comment.append(m[1])
                else:
                    m = re.search(
                        r"^\s*(?::|\/\/)", line
                    )  # skip lines that start with a colon (constructor parameter initialization) or are comments
                    if m:
                        continue

                    m = re.search(r"\s*(virtual\s)?(.*)\s+([^\(]*)\(([^\)]*)", line)
                    if m:
                        name = m[3]
                        # move ctor is useless for Lua
                        is_move_ctr = re.fullmatch(fr"\s*{name}\s*&&[^,]*", m[4]) and not m[2]
                        if not is_move_ctr:
                            if name not in member_funs:
                                member_funs[name] = []
                            member_funs[name].append(
                                {
                                    "return": m[2],
                                    "name": m[3],
                                    "param": m[4],
                                    "comment": comment,
                                }
                            )
                        comment = []

                    m = re.search(
                        r"\s*([^\;\{]*)\s+([^\;^\{}]*)\s*(\{[^\}]*\})?\;", line
                    )
                    if m:
                        if m[1].endswith(",") and not (m[2].endswith(">") or m[2].endswith(")")): #Allows things like imgui ImVec2 'float x, y' and ImVec4 if used, 'float x, y, w, h'. Match will be '[1] = "float x," [2] = "y"'. Some other not exposed variables will be wrongly matched (as already happens).
                            types_and_vars = m[1]
                            vars_match = re.search(r"(?: *\w*,)*$", types_and_vars)
                            vars_except_last = vars_match.group() #Last var is m[2]
                            start, end = vars_match.span()
                            vars_type = types_and_vars[:start]
                            for m_var in re.findall(r"(\w*),", vars_except_last):
                                member_vars.append(
                                    {"type": vars_type, "name": m_var, "comment": comment}
                                )
                            member_vars.append(
                                {"type": vars_type, "name": m[2], "comment": comment}
                            )
                        else:
                            member_vars.append(
                                {"type": m[1], "name": m[2], "comment": comment}
                            )
                        comment = []
            elif brackets_depth == 0:
                classes.append(
                    {
                        "name": class_name,
                        "member_funs": member_funs,
                        "member_vars": member_vars,
                    }
                )
                class_name = None
                comment = []
                member_funs = {}
                member_vars = []

for file in api_files:
    comment = []
    data = open(file, "r").read().split("\n")
    for line in data:
        line = line.replace("*", "")
        m = re.search(r'lua\[[\'"]([^\'"]*)[\'"]\];', line)
        if m:
            events.append({"name": m.group(1), "comment": comment})
        else:
            comment = []
        c = re.search(r"/// ?(.*)$", line)
        if c:
            comment.append(c.group(1))
        else:
            comment = []

for file in api_files:
    comment = []
    data = open(file, "r").read().split("\n")
    for line in data:
        line = line.replace("*", "")
        m = re.search(r'lua\[[\'"]([^\'"]*)[\'"]\]\s+=\s+(.*?)(?:;|$)', line)
        if m and not m.group(1).startswith("__"):
            if not getfunc(m.group(1)):
                funcs.append(
                    {"name": m.group(1), "cpp": m.group(2), "comment": comment}
                )
            comment = []
        c = re.search(r"/// ?(.*)$", line)
        if c:
            comment.append(c.group(1))

for file in api_files:
    data = open(file, "r").read()
    data = data.replace("\n", "")
    data = re.sub(r" ", "", data)
    m = re.findall(r'new_usertype\<([^\>]*?)\>\s*\(\s*"([^"]*)",(.*?)\);', data)
    for type in m:
        cpp_type = type[0]
        name = type[1]
        attr = type[2]
        base = ""
        bm = re.search(r"sol::bases<([^\]]*)>", attr)
        if bm:
            base = bm.group(1)
        attr = attr.replace('",', ",")
        attr = attr.split('"')
        vars = []

        underlying_cpp_type = next(
            (item for item in classes if item["name"] == cpp_type), dict()
        )
        if "member_funs" not in underlying_cpp_type:
            if cpp_type in cpp_type_exceptions:
                underlying_cpp_type = {"name": cpp_type, "member_funs": {}, "member_vars": {}}
            else:
                raise RuntimeError(
                        f"No member_funs found in \"{cpp_type}\" while looking for usertypes in file \"{file}\". Did you forget to include a header file at the top of the generate script? (if it isn't the problem then add it to cpp_type_exceptions list)"
                )

        for var in attr:
            if not var:
                continue
            var = var.split(",")
            if var[0] == "sol::base_classes" or var[0] == "sol::no_constructor":
                continue
            if "table_of" in var[1]:
                var[1] = var[1].replace("table_of(", "") + "[]"
            if var[1].startswith("sol::readonly"):
                var[1] = var[1].replace("sol::readonly(", "")
                var[1] = var[1][:-1]
            if var[1].startswith("std::move"):
                var[1] = var[1].replace("std::move(", "")
                var[1] = var[1][:-1]

            var_name = var[0]
            cpp = var[1]

            if var[1].startswith("sol::property"):
                param_match = re.match(fr"sol::property\(\[\]\({underlying_cpp_type['name']}&(\w+)\)", cpp)
                if param_match:
                    type_var_name = param_match[1]
                    m_var_return = re.search(fr"return[^;]*{type_var_name}\.([\w.]+)", cpp)
                    if m_var_return:
                        cpp_name = m_var_return[1]
                        cpp_name = cpp_name.replace(".", "::")
                        cpp = f"&{underlying_cpp_type['name']}::{cpp_name}"
                else:
                    cpp_name = cpp
            else:
                cpp_name = cpp[cpp.find("::") + 2 :] if cpp.find("::") >= 0 else cpp

            if var[0].startswith("sol::constructors"): #TODO: New list of constructors
                for fun in underlying_cpp_type["member_funs"][cpp_type]:
                    param = fun["param"]
                    if "const" in param:
                        param = fix_constructor_param(param)
                    elif param == fun["name"]:
                        continue
                    constructor = next(
                        (item for item in constructors if item["name"] == cpp_type), dict()
                    )
                    if "name" in constructor:
                        constructor["list"].append(
                            {
                                "name": cpp_type,
                                "param": param,
                                "comment": fun["comment"],
                            }
                        )
                    else:
                        constructors.append(
                            {
                                "name": cpp_type,
                                "list": [
                                    {
                                        "name": cpp_type,
                                        "param": param,
                                        "comment": fun["comment"],
                                    }
                                ]
                            }
                        )
            elif cpp_name in underlying_cpp_type["member_funs"]:
                for fun in underlying_cpp_type["member_funs"][cpp_name]:
                    ret = fun["return"]
                    param = fun["param"]
                    param = cpp_params_to_emmy_lua_fun(param) #TODO
                    sig = f"{ret} {var_name}({param})"
                    vars.append(
                        {
                            "name": var_name,
                            "type": cpp,
                            "signature": sig,
                            "orig_params": fun["param"],
                            "comment": fun["comment"],
                        }
                    )
            else:
                underlying_cpp_var = next(
                    (
                        item
                        for item in underlying_cpp_type["member_vars"]
                        if item["name"] == cpp_name or (item["name"].endswith("]") and f"{cpp_name}[" in item["name"])
                    ),
                    dict(),
                )
                if underlying_cpp_var:
                    type = underlying_cpp_var["type"]
                    sig = ""
                    if underlying_cpp_var["name"].endswith("]"):
                        if type == "char":
                            sig = f"---@field {var_name} string"
                        else:
                            arr_size = underlying_cpp_var["name"][underlying_cpp_var["name"].find("[")+1:-1]
                            sig = f"---@field {var_name} {type}[] @size: {arr_size}"
                    else:
                        sig = f"---@field {var_name} {type}"
                    vars.append(
                        {
                            "name": var_name,
                            "type": cpp,
                            "signature": sig,
                            "comment": underlying_cpp_var["comment"],
                        }
                    )
                else:
                    m_return_type = re.search(r"->(\w+){", var[1]) #Use var[1] instead of cpp because it could be replaced on the sol::property stuff
                    if m_return_type:
                        type = replace_all(m_return_type[1], replace)
                        sig = f"---@field {var_name} {type}"
                        vars.append({"name": var_name, "type": cpp, "signature": sig})
                    else:
                        vars.append({"name": var_name, "type": cpp})
        types.append({"name": name, "vars": vars, "base": base})

for file in api_files:
    with open(file) as fp:
        line = fp.readline()
        while line:
            m = re.search(r'lua\["Entity"\]\["(as_.*)"\]', line)
            if m != None:
                known_casts.append(m.group(1))
            line = fp.readline()
known_casts.sort()

for file in api_files:
    comment = []
    data = open(file, "r").read().split("\n")
    for line in data:
        line = line.replace("*", "")
        m = re.findall(r"new_usertype\<(.*?)\>", line)
        if m:
            type = m[0]
            type_to_mod = next((item for item in types if item["name"] == type), dict())
            if type_to_mod:
                type_to_mod["comment"] = comment
            comment = []
        if line == "":
            comment = []
        c = re.search(r"/// ?(.*)$", line)
        if c:
            comment.append(c.group(1))

#DELETED ENUM STUFF, we will get enums with spel2.lua
#TODO: get some enums that have comments, like ON or SPAWN_TYPE

for file in api_files:
    data = open(file, "r").read()
    data = data.replace("\n", "")
    data = re.sub(r" ", "", data)
    m = re.search(r"open_libraries\s*\(([^\)]*)\)", data)
    if m:
        libs = m.group(1).split(",")
        for lib in libs:
            lualibs.append(lib.replace("sol::lib::", ""))

data = open("../src/game_api/aliases.hpp", "r").read().split("\n")
for line in data:
    if not line.endswith("NoAlias"):
        m = re.search(r"using\s*(\S*)\s*=\s*(\S*)", line)
        if m:
            name = m.group(1)
            type = replace_all(m.group(2), replace)
            aliases.append({"name": name, "type": type})

#print("## Global variables")
#print("""These variables are always there to use.""")
#for lf in funcs:
#
#    if lf["name"] in not_functions:
#        print(
#            "### [`"
#            + lf["name"]
#            + "`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q="
#            + lf["name"]
#            + ")"
#        )
#        for com in lf["comment"]:
#            print(com)

print(
    """---@diagnostic disable: unused-function,lowercase-global
---@class Meta
---@field name string
---@field version string
---@field description string
---@field author string

---@type Meta
meta = nil

---@type StateMemory
state = nil
---@type GameManager
game_manager = nil
---@type Online
online = nil
---@type Player[]
players = nil
---@type SaveData
savegame = nil
---@type any
options = nil
---@type PRNG
prng = nil"""
)

#deprecated_funcs = [
#    func for func in funcs if func["comment"] and func["comment"][0] == "Deprecated"
#]
funcs = [
    func
    for func in funcs
    if not func["comment"] or not func["comment"][0] == "Deprecated"
]

print("\n-- Functions\n")

for lf in funcs:
    if len(rpcfunc(lf["cpp"])):
        for af in rpcfunc(lf["cpp"]):
            print_af(lf, af)
    elif not (lf["name"].startswith("on_") or lf["name"] in not_functions):
        if lf["comment"] and lf["comment"][0] == "NoDoc":
            continue
        m = re.search(r"\(([^\{]*)\)\s*->\s*([^\{]*)", lf["cpp"])
        m2 = re.search(r"\(([^\{]*)\)", lf["cpp"])
        ret = "nil"
        typed_params = ""
        params = ""
        if m:
            ret = replace_all(m.group(2), replace).strip() or "nil"
        if m or m2:
            params = (m or m2).group(1)
            typed_params, params = cpp_params_to_emmy_lua(params) #TODO
            typed_params = replace_all(typed_params, replace).strip()
        name = lf["name"]
        print_comment(lf)
        print_func(name, params, ret, typed_params)


#print("//## Deprecated Functions")
#print(
#    "//#### These functions still exist but their usage is discouraged, they all have alternatives mentioned here so please use those!"
#)
#
#for lf in events:
#    if lf["name"].startswith("on_"):
#        print(
#            "### [`"
#            + lf["name"]
#            + "`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q="
#            + lf["name"]
#            + ")"
#        )
#        for com in lf["comment"]:
#            print(com)
#
#for lf in deprecated_funcs:
#    lf["comment"].pop(0)
#    if len(rpcfunc(lf["cpp"])):
#        for af in rpcfunc(lf["cpp"]):
#            print_af(lf, af)
#    elif not (lf["name"].startswith("on_") or lf["name"] in not_functions):
#        if lf["comment"] and lf["comment"][0] == "NoDoc":
#            continue
#        m = re.search(r"\(([^\{]*)\)\s*->\s*([^\{]*)", lf["cpp"])
#        m2 = re.search(r"\(([^\{]*)\)", lf["cpp"])
#        ret = "nil"
#        param = ""
#        if m:
#            ret = replace_all(m.group(2), replace).strip() or "nil"
#        if m or m2:
#            param = (m or m2).group(1)
#            param = replace_all(param, replace).strip()
#        name = lf["name"]
#        fun = f"{ret} {name}({param})".strip()
#        search_link = "https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=" + name
#        print(f"### [`{name}`]({search_link})")
#        print(f"`{fun}`<br/>")
#        for com in lf["comment"]:
#            print(com)


print("\n--## Types\n")
for type in types:
    print("---@class " + type["name"], end="")
    if type["base"]:
        bases = type["base"].split(",")
        print(" : " + bases[-1], end="")
    print()
    index = 0
    for var in type["vars"]:
        if "signature" in var:
            signature = var["signature"]
            m = re.search(r"\s*(.*)\s+([^\(]*)\(([^\)]*)", var["signature"])
            if m:
                ret = fix_return(replace_all(m.group(1), replace)) or "nil"
                if ret.startswith("static"):
                    continue
                name = m.group(2)
                param = replace_all(m.group(3), replace)
                var_name = var["name"]
                if "overloads" in type and var_name in type["overloads"]:
                    type["overloads"][var_name].append({"name": name, "param": param, "ret": ret, "orig_params": var["orig_params"]})
                    index += 1
                    continue
                elif index+1 < len(type["vars"]) and type["vars"][index+1]["name"] == var_name:
                    if "overloads" in type:
                        type["overloads"][var_name] = [{ "name": name, "param": param, "ret": ret, "orig_params": var["orig_params"] }]
                    else:
                        type["overloads"] = { var_name: [{ "name": name, "param": param, "ret": ret, "orig_params": var["orig_params"] }] }
                    print(f"    ---@field {name} {type['name']}_{name}")
                    index += 1
                    continue
                else:
                    signature = f"---@field {name} fun(self, {param}): {ret}"
            signature = signature.strip()
            print("    " + signature)
        else:
            var_name = var["name"]
            var_type = var["type"]
            print(f"    ---@field {var_name} any @{var_type}")
        index += 1
    print()
    if "overloads" in type:
        for overload_name, funcs in type["overloads"].items():
            class_name = f"{type['name']}_{overload_name}"
            print(f"---@class {class_name}")
            typed_params, params = cpp_params_to_emmy_lua(funcs[-1]["orig_params"])
            print(typed_params.strip())
            index = 0
            for overload in funcs:
                if index+1 < len(funcs):
                    overload_param = f"self, {overload['param']}" if overload["param"] != "" else "self"
                    print(f"---@overload fun({overload_param}): {overload['ret']}")
                else:
                    params = f"self, {params}" if params != "" else "self"
                    print(f"local function {class_name}({params}) end\n")
                index += 1

print("\n--## Constructors")

for constructor in constructors:
    print(f"\n{constructor['name']} = nil")
    for const in constructor["list"]:
        typed_params, params = cpp_params_to_emmy_lua(const["param"])
        typed_params.strip()
        typed_params = replace_all(typed_params, replace)

        name = f"{constructor['name']}.new"
        params = f"self, {params}" if params != "" else "self"

        print_comment(const)
        print_func(name, params, constructor["name"], typed_params)


print("\n--## Enums\n")
enumStr = ""
data = open("./game_data/lua_enums.txt", "r", encoding="latin-1").read()
match_i = re.finditer(r"\n([A-Z_]+?) = {\n(?! *__)[\s\S]+?\n}", data)

for match in match_i:
    enumStr += f"\n{match.group(0)[1:]}\n---@alias {match.group(1)} integer"

print(enumStr)

#EXTRA THINGS
print(
"""local MAX_PLAYERS = 4

---@alias in_port_t number
---@class Logic

---@alias OnlinePlayerShort any
---@alias UdpServer any
---@alias Texture any
---@alias SpearDanglerAnimFrames any
---@alias OnlineLobbyScreenPlayer any
---@alias SoundCallbackFunction fun(): any"""
)

print("\n--## Aliases\n")
print("---@alias IMAGE number")
for alias in aliases:
    name = alias["name"]
    type = alias["type"]
    print(f"---@alias {name} {type}")

sys.stdout.close()
sys.stdout = sys.__stdout__

#Replace some things
final_replace_stuff = {
"    ---@field menu_text_opacity number\n    ---@field menu_text_opacity number": "---@field menu_text_opacity number",
"---@field find_all_short_tile_codes fun(self, layer: LAYER, short_tile_code: SHORT_TILE_CODE): Array<tuple<integer, integer, LAYER>>":
"---@field find_all_short_tile_codes fun(self, layer: LAYER, short_tile_code: SHORT_TILE_CODE): integer[][]",

"""---@field keysdown boolean       [] @size: 512
    ---@field keydown any @keydown
    ---@field keypressed any @keypressed
    ---@field keyreleased any @keyreleased""":
"""---@field keysdown boolean       [] @size: 512. Note: lua starts indexing at 1, you need `keysdown[string.byte('A') + 1]` to find the A key.
    ---@field keydown fun(key: number | string): boolean
    ---@field keypressed fun(key: number | string, repeat?: boolean ): boolean
    ---@field keyreleased fun(key: number | string): boolean""",
"---@field gamepad any @sol::property([](){g_WantUpdateHasGamepad=true;returnget_gamepad()/**/;})": "---@field gamepad Gamepad"
}

with open('./game_data/spel2.lua', 'r') as file :
  declarations_text = file.read()

for find, replacement in final_replace_stuff.items():
    declarations_text = declarations_text.replace(find, replacement)

with open('./game_data/spel2.lua', 'w') as file:
  file.write(declarations_text)
