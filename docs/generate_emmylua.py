from bdb import Breakpoint
import re

import generate_util as gu
import parse_source as ps

replace_table = {
    "uint8_t": "integer",
    "uint16_t": "integer",
    "uint32_t": "integer",
    "uint64_t": "integer",
    "int8_t": "integer",
    "int16_t": "integer",
    "int32_t": "integer",
    "int64_t": "integer",
    "size_t": "integer",
    "ImU32": "integer",
    "vector": "Array",
    "array": "Array",
    "unordered_map": "table",
    "game_table": "table",
    ", identity_hasher<>": "",
    "const char*": "string",
    "wstring": "string",
    "u16string": "string",
    "char16_t": "string",
    "string_view": "string",
    "pair": "tuple",
    "std::": "",
    "sol::": "",
    " = nullopt": "",
    "void": "",
    "constexpr": "",
    "...va:": "...ent_type:",
    "// Access via": "ImGuiIO",
    "set<": "Array<",
    "span<": "Array<",
    "&": "",
    "const ": "",
    "ShopType": "SHOP_TYPE",
    "EmittedParticlesInfo": "Array<Particle>",
    "object": "any",
    "ImVec2": "Vec2",
}

reFloat = re.compile(r"\bfloat\b")
reInt = re.compile(r"\bint\b")
reOptional = re.compile(r"optional<(.+?)>")
reBool = re.compile(r"bool\b")
reMap = re.compile(r"\bmap<")
reArr = re.compile(r"(\bArray<(?:(\w+<.+>)|(\w+(?:<.+>)?))(?:, )? ?(.*)>)")


def replace_all(text):
    text = reFloat.sub("number", text)
    text = reInt.sub("integer", text)
    text = reBool.sub("boolean", text)
    text = reOptional.sub(r"\1?", text)
    text = reMap.sub("table<", text)
    if "Array<" in text:
        newText = text
        dimension = 1
        while True:
            m = reArr.search(newText)
            if m:
                if m[2]:
                    dimension += 1
                    newText = m[2]
                elif m[3]:
                    newText = f"{m[3]}" + ("[]" * dimension)
                    text = reArr.sub(newText, text)
                    break
            else:
                break

    for repl, wth in replace_table.items():
        pos = text.find(repl)
        br2 = text.find("`", pos + len(repl))
        br1 = text.rfind("`", 0, pos)
        if pos > 0 and br1 >= 0 and br2 > 0:
            continue
        text = text.replace(repl, wth)

    return text


ps.configure_parse(replace_all, "emmy_lua.pickle")
ps.run_parse()


reGetParam = re.compile(r"(?!const)(\b[^ ]+) *([^,]+),?")
reRemoveDefault = re.compile(r" = .*")
reHandleConst = re.compile(r"const (\w+) (\w+)")

def add_callback_signature(params, cb_signatures):
    if len(cb_signatures) == 1:
        cb_signature = next(iter(cb_signatures.values()))
        cb_params = cpp_params_to_emmy_lua_fun(cb_signature["param"])
        ret = replace_all(cb_signature["return"])
        params = params.replace("function", f"fun({cb_params}): {ret}")
    else:
        for name, cb_signature in cb_signatures.items():
            cb_params = cpp_params_to_emmy_lua_fun(cb_signature["param"])
            ret = replace_all(cb_signature["return"])
            params = params.replace(f"function {name}", f"fun({cb_params}): {ret}")
    return params

def cpp_params_to_emmy_lua(params_text, cb_signatures=None):
    return_typed = ""
    return_normal = ""
    params_iterator = reGetParam.finditer(params_text)
    for param_match in params_iterator:
        p_type = replace_all(param_match.group(1))
        p_name = reRemoveDefault.sub("", param_match.group(2))
        if p_type == "variadic_args":
            return_typed += f"\n---@vararg any"
            return_normal += f"..."
        else:
            if m := reHandleConst.match(p_name):
                p_type = m.group(1)
                p_name = m.group(2)
            if cb_signatures and p_type == "function":
                if len(cb_signatures) == 1:
                    cb_signature = next(iter(cb_signatures.values()))
                    params = cpp_params_to_emmy_lua_fun(cb_signature["param"])
                    ret = replace_all(cb_signature["return"])
                    p_type = f"fun({params}): {ret}"
                elif p_name in cb_signatures:
                    cb_signature = cb_signatures[p_name]
                    params = cpp_params_to_emmy_lua_fun(cb_signature["param"])
                    ret = replace_all(cb_signature["return"])
                    p_type = f"fun({params}): {ret}"
            return_typed += f"\n---@param {p_name} {p_type}"
            return_normal += p_name
        return_normal += ", "
    return_normal = return_normal[0:-2]
    return return_typed, return_normal


def cpp_params_to_emmy_lua_fun(params_text):
    params = replace_all(params_text).strip().split(",")
    params = [
        ": ".join([part.strip() for part in param.rsplit(" ", 1)[::-1]])
        for param in params
    ]
    return ", ".join(params)


reTuple = re.compile(r"tuple<(.*?)>")


def fix_return(ret):
    ret = replace_all(ret)
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

reMarkdownLink = re.compile(r"(\[\w+\])\((#\w+?)\)")
def print_comment(lf):
    if lf["comment"]:
        for com in lf["comment"]:
            com = com.replace("<br/>", "")
            com = reMarkdownLink.sub(r"\1(https://spelunky-fyi.github.io/overlunky/\2)", com)
            print(f"---{com}")


def print_af(lf, af):
    if lf["comment"] and "NoDoc" in lf["comment"][0]:
        return
    ret = replace_all(af["return"]) or "nil"
    name = lf["name"]
    typed_params, params = cpp_params_to_emmy_lua(af["param"])
    typed_params.strip()
    typed_params = replace_all(typed_params)
    print_comment(lf if lf["comment"] else af)
    print_func(name, params, ret, typed_params)


gu.setup_stdout("game_data/spel2.lua")

print(
    """---@diagnostic disable: unused-function,lowercase-global,missing-return,duplicate-doc-alias
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

print("\n-- Functions\n")
print(
    """
---Formatting function, use e.g. as f "my_var = {my_var}"
---@param f_string string
---@return string
function f(f_string) end
---Formatting function, use e.g. as f "my_var = {my_var}"
---@param f_string string
---@return string
function F(f_string) end
"""
)

for lf in ps.funcs:
    if len(ps.rpcfunc(lf["cpp"])):
        for af in ps.rpcfunc(lf["cpp"]):
            print_af(lf, af)
    elif not (lf["name"].startswith("on_") or lf["name"] in ps.not_functions):
        if lf["comment"] and "NoDoc" in lf["comment"][0]:
            continue
        m = re.search(r"\(([^\{]*)\)\s*->\s*([^\{]*)", lf["cpp"])
        m2 = re.search(r"\(([^\{]*)\)", lf["cpp"])
        ret = "nil"
        typed_params = ""
        params = ""
        if m:
            ret = replace_all(m.group(2)).strip() or "nil"
        if m or m2:
            params = (m or m2).group(1)
            typed_params, params = cpp_params_to_emmy_lua(
                params, lf["cb_signature"] if "cb_signature" in lf else ""
            )
            typed_params = replace_all(typed_params).strip()
        name = lf["name"]
        print_comment(lf)
        print_func(name, params, ret, typed_params)


print("\n--## Types\n")
for type in ps.types:
    print("---@class " + type["name"], end="")
    if type["base"]:
        bases = type["base"].split(",")
        print(" : " + bases[-1], end="")
    print()
    index = 0
    for var in type["vars"]:
        if "function" in var and var["function"]:
            # if var["name"] == "clear_behaviors":
            #    gu.breakpoint()
            signature = var["signature"]
            m = re.search(r"\s*(.*)\s+([^\(]*)\((.*)\)", signature)
            if m:
                ret = fix_return(m.group(1)) or "nil"
                if ret.startswith("static"):
                    continue
                name = m.group(2)
                params = cpp_params_to_emmy_lua_fun(m.group(3))
                var_name = var["name"]
                if "overloads" in type and var_name in type["overloads"]:
                    type["overloads"][var_name].append(
                        {
                            "name": name,
                            "param": params,
                            "ret": ret,
                            "orig_params": var["signature"],
                        }
                    )
                    index += 1
                    continue
                elif (
                    index + 1 < len(type["vars"])
                    and type["vars"][index + 1]["name"] == var_name
                ):
                    if "overloads" not in type:
                        type["overloads"] = {}
                    type["overloads"][var_name] = [
                        {
                            "name": name,
                            "param": params,
                            "ret": ret,
                            "orig_params": var["signature"],
                        }
                    ]
                    print(f"    ---@field {name} {type['name']}_{name}")
                    index += 1
                    continue
                elif params:
                    if "cb_signature" in var and var["cb_signature"] and "function" in params:
                        params = add_callback_signature(params, var["cb_signature"])
                    signature = f"---@field {name} fun(self, {params}): {ret}"
                else:
                    signature = f"---@field {name} fun(self): {ret}"
            signature = signature.strip()
            print("    " + signature)
        elif "signature" in var:
            [var_type, var_name] = var["signature"].rsplit(" ", 1)
            arr_m = reArr.match(var_type)
            arr_size = arr_m[4] if arr_m else None
            var_type = replace_all(var_type)
            if arr_size:
                var_type = f"{var_type} @size: {arr_size}"
            elif var_name.endswith("]"):
                if var_type == "char":
                    var_type = "string"
                else:
                    arr_size = var_name[var_name.find("[") + 1 : -1]
                    var_type = f"{var_type}[] @size: {arr_size}"
                var_name = var_name.split("[")[0]
            print(f"    ---@field {var_name} {var_type.strip()}")
        else:
            var_name = var["name"]
            var_cpp_type = var["type"]
            print(f"    ---@field {var_name} any @{var_cpp_type}")
        index += 1
    print()
    if "overloads" in type:
        for overload_name, funcs in type["overloads"].items():
            class_name = f"{type['name']}_{overload_name}"
            print(f"---@class {class_name}")
            m = re.search(r"\s*(.*)\s+([^\(]*)\((.*)\)", funcs[-1]["orig_params"])
            typed_params, params = cpp_params_to_emmy_lua(m.group(3))
            print(typed_params.strip())
            index = 0
            for overload in funcs:
                if index + 1 < len(funcs):
                    overload_param = (
                        f"self, {overload['param']}"
                        if overload["param"] != ""
                        else "self"
                    )
                    print(f"---@overload fun({overload_param}): {overload['ret']}")
                else:
                    params = f"self, {params}" if params != "" else "self"
                    print(f"local function {class_name}({params}) end\n")
                index += 1

print("\n--## Constructors")

for name, overloads in ps.constructors.items():
    print(f"\n{name} = nil")
    for ctor in overloads:
        typed_params, params = cpp_params_to_emmy_lua(ctor["signature"])
        typed_params.strip()
        typed_params = replace_all(typed_params)

        new_name = f"{name}.new"
        params = f"self, {params}" if params != "" else "self"

        print_comment(ctor)
        print_func(new_name, params, name, typed_params)


print("\n--## Enums\n")
enumStr = ""
data = open("./game_data/lua_enums.txt", "r", encoding="latin-1").read()
match_i = re.finditer(r"\n([A-Z_]+?) = {\n(?! *__)[\s\S]+?\n}", data)

for match in match_i:
    enumStr += f"\n{match.group(0)[1:]}\n---@alias {match.group(1)} integer"

print(enumStr)

# EXTRA THINGS
print(
    """local MAX_PLAYERS = 4

---@alias in_port_t number
---@class Logic

---@alias OnlinePlayerShort any
---@alias UdpServer any
---@alias Texture any
---@alias SpearDanglerAnimFrames any
---@alias OnlineLobbyScreenPlayer any
---@alias SoundCallbackFunction function"""
)

print("\n--## Aliases\n")
print("---@alias IMAGE number")
for alias in ps.aliases:
    name = alias["name"]
    type = alias["type"]
    print(f"---@alias {name} {type}")


gu.cleanup_stdout()


# Replace some things
final_replace_stuff = {
    "    ---@field menu_text_opacity number\n    ---@field menu_text_opacity number": "---@field menu_text_opacity number",
    "---@field find_all_short_tile_codes fun(self, layer: LAYER, short_tile_code: SHORT_TILE_CODE): Array<tuple<integer, integer, LAYER>>": "---@field find_all_short_tile_codes fun(self, layer: LAYER, short_tile_code: SHORT_TILE_CODE): integer[][]",
    """---@field keysdown boolean       [] @size: 512
    ---@field keydown any @keydown
    ---@field keypressed any @keypressed
    ---@field keyreleased any @keyreleased""": """---@field keysdown boolean       [] @size: 512. Note: lua starts indexing at 1, you need `keysdown[string.byte('A') + 1]` to find the A key.
    ---@field keydown fun(key: number | string): boolean
    ---@field keypressed fun(key: number | string, repeat?: boolean ): boolean
    ---@field keyreleased fun(key: number | string): boolean""",
    "---@field gamepad any @sol::property([](){g_WantUpdateHasGamepad=true;returnget_gamepad()/**/;})": "---@field gamepad Gamepad",
    "---@field user_data fun(self): nil": "---@field user_data table?",
    """---@param number> p tuple<number,
---@return Vec2
function Vec2.new(self, number> p) end""": "",
}

with open("./game_data/spel2.lua", "r") as file:
    declarations_text = file.read()

for find, replacement in final_replace_stuff.items():
    declarations_text = declarations_text.replace(find, replacement)

with open("./game_data/spel2.lua", "w") as file:
    file.write(declarations_text)
