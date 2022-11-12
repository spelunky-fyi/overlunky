import re
import os

from generate_util import *
from parse_cache import *

ENABLE_CACHING = True
CACHE_FILE = None

if not os.path.exists(".db"):
    os.makedirs(".db")

header_files = [
    "../src/game_api/math.hpp",
    "../src/game_api/rpc.hpp",
    "../src/game_api/drops.hpp",
    "../src/game_api/spawn_api.hpp",
    "../src/game_api/script.hpp",
    "../src/game_api/color.hpp",
    "../src/game_api/entity.hpp",
    "../src/game_api/entity_db.hpp",
    "../src/game_api/entity_structs.hpp",
    "../src/game_api/movable.hpp",
    "../src/game_api/movable_behavior.hpp",
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
    "../src/game_api/texture.hpp",
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
    "../src/game_api/settings_api.hpp",
    "../src/game_api/script/usertypes/level_lua.hpp",
    "../src/game_api/script/usertypes/gui_lua.hpp",
    "../src/game_api/script/usertypes/vanilla_render_lua.hpp",
    "../src/game_api/script/usertypes/save_context.hpp",
    "../src/game_api/script/usertypes/hitbox_lua.hpp",
    "../src/game_api/script/usertypes/socket_lua.hpp",
    "../src/imgui/imgui.h",
    "../src/game_api/script/usertypes/level_lua.cpp",
    "../src/game_api/script/usertypes/gui_lua.cpp",
    "../src/game_api/steam_api.hpp",
    "../src/game_api/search.hpp",
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
    "../src/game_api/script/usertypes/game_manager_lua.cpp",
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
    "../src/game_api/script/usertypes/behavior_lua.cpp",
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
    "../src/game_api/script/usertypes/steam_lua.cpp",
]
rpc = []
classes = []
events = []
funcs = []
deprecated_funcs = []
pre_gathered_vars = {}
types = []
known_casts = []
aliases = []
lualibs = []
enums = []
constructors = {}

cpp_type_exceptions = []
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

replace_fun = None

reConstructorFix = re.compile(r"const (\w+)(?: \w+)?(&&|&)?")


def fix_constructor_param(params_text):
    return reConstructorFix.sub(r"\1", params_text)


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


def configure_parse(replace_function, cache_file):
    global replace_fun, CACHE_FILE
    replace_fun = replace_function
    CACHE_FILE = cache_file


def run_parse():
    if not replace_fun:
        print_console("Did not run configure_parse, can not parse source files...")
        return

    global rpc, classes, events, funcs, deprecated_funcs, pre_gathered_vars, types, known_casts, aliases, lualibs, enums, constructors

    cache_file = os.path.join(".db", CACHE_FILE)
    if ENABLE_CACHING and not needs_update(header_files + api_files, cache_file):
        print_console("Files are unchanged, trying to use pickled parse data...")
        unpickled = do_unpickle(cache_file)
        if unpickled != None:
            rpc = unpickled["rpc"]
            classes = unpickled["classes"]
            events = unpickled["events"]
            funcs = unpickled["funcs"]
            deprecated_funcs = unpickled["deprecated_funcs"]
            pre_gathered_vars = unpickled["pre_gathered_vars"]
            types = unpickled["types"]
            known_casts = unpickled["known_casts"]
            aliases = unpickled["aliases"]
            lualibs = unpickled["lualibs"]
            enums = unpickled["enums"]
            constructors = unpickled["constructors"]
            print_console("Using pickled parse data...")
            return
        else:
            print_console("Failed unpickling parse data...")

    comment = []
    skip = False

    print_collecting_info("rpc")
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
                    comment = []
            elif not c:
                comment = []

    print_collecting_info("class")
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
            line = replace_fun(line)
            line = line.replace("*", "")
            if not class_name and ("struct" in line or "class" in line):
                m = re.match(r"(struct|class)\s+(\S+)", line)
                if m and not line.endswith(";"):
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
                            is_move_ctr = (
                                re.fullmatch(r"^[a-zA-Z0-9_]*$", name)
                                and re.fullmatch(rf"\s*{name}\s*&&[^,]*", m[4])
                                and not m[2]
                            )
                            if not is_move_ctr:
                                if name not in member_funs:
                                    member_funs[name] = []
                                param = m[4]
                                if " " not in param:
                                    param = f"{param} {param.lower()}"
                                member_funs[name].append(
                                    {
                                        "return": m[2],
                                        "name": m[3],
                                        "param": param,
                                        "comment": comment,
                                    }
                                )
                            comment = []

                        m = re.search(
                            r"\s*([^\;\{]*)\s+([^\;^\{}]*)\s*(\{[^\}]*\})?\;", line
                        )
                        if m:
                            if m[1].endswith(",") and not (
                                m[2].endswith(">") or m[2].endswith(")")
                            ):  # Allows things like imgui ImVec2 'float x, y' and ImVec4 if used, 'float x, y, w, h'. Match will be '[1] = "float x," [2] = "y"'. Some other not exposed variables will be wrongly matched (as already happens).
                                types_and_vars = m[1]
                                vars_match = re.search(r"(?: *\w*,)*$", types_and_vars)
                                vars_except_last = (
                                    vars_match.group()
                                )  # Last var is m[2]
                                start, end = vars_match.span()
                                vars_type = types_and_vars[:start]
                                for m_var in re.findall(r"(\w*),", vars_except_last):
                                    member_vars.append(
                                        {
                                            "type": vars_type,
                                            "name": m_var,
                                            "comment": comment,
                                        }
                                    )
                                member_vars.append(
                                    {
                                        "type": vars_type,
                                        "name": m[2],
                                        "comment": comment,
                                    }
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

    print_collecting_info("events")
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

    print_collecting_info("functions")
    for file in api_files:
        comment = []
        data = open(file, "r").read().split("\n")
        for line in data:
            line = line.replace("*", "")
            line = line.strip()
            if line == "":
                comment = []

            m = re.search(
                r'lua\[[\'"]([^\'"]*)[\'"]\]\[[\'"]([^\'"]*)[\'"]\]\s+=\s+(.*?)(?:;|$)',
                line,
            )
            if m:
                name = m.group(2)
                if not name.startswith("as_"):  # don't include the casting functions
                    cpp_type = m.group(1)
                    underlying_cpp_type = next(
                        (item for item in classes if item["name"] == cpp_type), dict()
                    )

                    if name not in underlying_cpp_type["member_funs"]:
                        underlying_cpp_type["member_funs"][name] = []

                    cpp = m.group(3)
                    if len(rpcfunc(cpp)):
                        for af in rpcfunc(cpp):
                            underlying_cpp_type["member_funs"][name].append(
                                {
                                    "return": af["return"],
                                    "name": name,
                                    "param": ", ".join(
                                        [p.strip() for p in af["param"].split(",")[1:]]
                                    ),
                                    "comment": af["comment"],
                                }
                            )
                    else:
                        m = re.search(r"\(([^\{]*)\)\s*->\s*([^\{]*)", cpp)
                        m2 = re.search(r"\(([^\{]*)\)", cpp)
                        ret = "nil"
                        param = ""
                        if m:
                            ret = replace_fun(m.group(2)).strip() or "nil"
                        if m or m2:
                            param = (m or m2).group(1)
                            param = replace_fun(param).strip()
                            param = ", ".join([p.strip() for p in param.split(",")[1:]])
                        underlying_cpp_type["member_funs"][name].append(
                            {
                                "return": ret,
                                "name": name,
                                "param": param,
                                "comment": comment,
                            }
                        )
                    if cpp_type not in pre_gathered_vars:
                        pre_gathered_vars[cpp_type] = []
                    pre_gathered_vars[cpp_type].append(f"{name}, {cpp_type}::{name}")
                    comment = []
            else:
                m = re.search(r'lua\[[\'"]([^\'"]*)[\'"]\]\s+=\s+(.*?)(?:;|$)', line)
                if m and not m.group(1).startswith("__"):
                    if not getfunc(m.group(1)):
                        func = {
                            "name": m.group(1),
                            "cpp": m.group(2),
                            "comment": comment,
                        }
                        if not comment or "NoDoc" not in comment[0]:
                            if comment and comment[0] == "Deprecated":
                                deprecated_funcs.append(func)
                            else:
                                funcs.append(func)
                    comment = []

            c = re.search(r"/// ?(.*)$", line)
            if c:
                comment.append(c.group(1))

    print_collecting_info("usertypes")
    for file in api_files:
        data = open(file, "r").read()
        data = data.replace("\n", "")
        data = re.sub(r" ", "", data)
        reParticleHelper = re.compile(r"MakeParticleMemberAccess<(.+?)>\(\)")
        data = reParticleHelper.sub(r"\1", data)
        m = re.findall(
            r'(auto([a-z]+_type)=)?lua\.new_usertype\<([^\>]*?)\>\s*\(\s*"([^"]*)",?(.*?)\);',
            data,
        )
        for type in m:
            container = type[1]
            cpp_type = type[2]
            name = type[3]
            attr = type[4]
            if container:
                extra = []
                n = re.findall(
                    r"(?<!NoDoc)" + container + r'\[("[^"]+")\]=([^;]+);', data
                )
                for var in n:
                    extra.append(",".join(var))
                extra = ",".join(extra)
                if attr:
                    attr = attr + "," + extra
                else:
                    attr = extra

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
                    underlying_cpp_type = {
                        "name": cpp_type,
                        "member_funs": {},
                        "member_vars": {},
                    }
                else:
                    raise RuntimeError(
                        f'No member_funs found in "{cpp_type}" while looking for usertypes in file "{file}". Did you forget to include a header file at the top of the generate script? (if it isn\'t the problem then add it to cpp_type_exceptions list)'
                    )

            if cpp_type in pre_gathered_vars:
                attr += pre_gathered_vars[cpp_type]

            skip = False
            for var in attr:
                if skip:
                    skip = False
                    continue
                if not var:
                    continue
                var = var.split(",")
                if var[0] == "sol::base_classes" or var[0] == "sol::no_constructor":
                    continue
                if "NoDoc" in var[0]:
                    skip = True
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
                    param_match = re.match(
                        rf"sol::property\(\[\]\({underlying_cpp_type['name']}&(\w+)\)",
                        cpp,
                    )
                    if param_match:
                        type_var_name = param_match[1]
                        m_var_return = re.search(
                            rf"return[^;]*{type_var_name}\.([\w.]+)", cpp
                        )
                        if m_var_return:
                            cpp_name = m_var_return[1]
                            cpp_name = cpp_name.replace(".", "::")
                            cpp = f"&{underlying_cpp_type['name']}::{cpp_name}"
                    else:
                        cpp_name = cpp
                else:
                    cpp_name = cpp[cpp.find("::") + 2 :] if cpp.find("::") >= 0 else cpp

                if var[0].startswith("sol::constructors"):
                    for fun in underlying_cpp_type["member_funs"][cpp_type]:
                        param = replace_fun(fun["param"])
                        param = fix_constructor_param(param)

                        if cpp_type not in constructors:
                            constructors[cpp_type] = []
                        constructors[cpp_type].append(
                            {
                                "name": cpp_type,
                                "signature": param,
                                "comment": fun["comment"],
                            }
                        )
                elif cpp_name in underlying_cpp_type["member_funs"]:
                    for fun in underlying_cpp_type["member_funs"][cpp_name]:
                        ret = fun["return"] or "nil"
                        param = fun["param"]
                        sig = f"{ret} {var_name}({param})"

                        vars.append(
                            {
                                "name": var_name,
                                "type": cpp,
                                "signature": sig,
                                "comment": fun["comment"],
                                "function": True,
                            }
                        )
                else:
                    underlying_cpp_var = next(
                        (
                            item
                            for item in underlying_cpp_type["member_vars"]
                            if item["name"] == cpp_name
                            or (
                                item["name"].endswith("]")
                                and f"{cpp_name}[" in item["name"]
                            )
                        ),
                        dict(),
                    )
                    if underlying_cpp_var:
                        type = underlying_cpp_var["type"]
                        sig = f"{type} {var_name}"
                        if underlying_cpp_var["name"].endswith("]"):
                            if type == "char":
                                sig = f"string {var_name}"
                            else:
                                sig += underlying_cpp_var["name"][
                                    underlying_cpp_var["name"].find("[") :
                                ]
                        vars.append(
                            {
                                "name": var_name,
                                "type": cpp,
                                "signature": sig,
                                "comment": underlying_cpp_var["comment"],
                            }
                        )
                    else:
                        m_return_type = re.search(
                            r"->(\w+){", var[1]
                        )  # Use var[1] instead of cpp because it could be replaced on the sol::property stuff
                        if m_return_type:
                            sig = f"{m_return_type[1]} {var_name}"
                            vars.append(
                                {"name": var_name, "type": cpp, "signature": sig}
                            )
                        else:
                            vars.append({"name": var_name, "type": cpp})
            types.append({"name": name, "vars": vars, "base": base})

    print_collecting_info("entities")
    for file in api_files:
        with open(file) as fp:
            line = fp.readline()
            while line:
                m = re.search(r'lua\["Entity"\]\["(as_.*)"\]', line)
                if m != None:
                    known_casts.append(m.group(1))
                line = fp.readline()
    known_casts.sort()

    print_collecting_info("additional usertypes")
    for file in api_files:
        comment = []
        data = open(file, "r").read().split("\n")
        for line in data:
            line = line.replace("*", "")
            m = re.findall(r"new_usertype\<(.*?)\>", line)
            if m:
                type = m[0]
                type_to_mod = next(
                    (item for item in types if item["name"] == type), dict()
                )
                if type_to_mod:
                    type_to_mod["comment"] = comment
                comment = []
            if line == "":
                comment = []
            c = re.search(r"/// ?(.*)$", line)
            if c:
                comment.append(c.group(1))

    print_collecting_info("enums")
    for file in api_files:
        data = open(file, "r").read()
        data = data.replace("\n", "")
        data = re.sub(r" ", "", data)
        m = re.findall(r'create_named_table\s*\(\s*"([^"]*)"\/\/,([^\)]*)', data)
        for type in m:
            name = type[0]
            attr = type[1]
            attr = attr.replace("//", "")
            attr = attr.replace('",', ",")
            attr = attr.split('"')
            vars = []
            for var in attr:
                if not var:
                    continue
                var = var.split(",")
                var[1] = var[1].replace("__", " ")
                var[1] = var[1].replace("\\[", "(")
                var[1] = var[1].replace("\\]", ")")
                vars.append({"name": var[0], "type": var[1]})
            enums.append({"name": name, "vars": vars})

    for file in api_files:
        data = open(file, "r").read()
        data = data.replace("\n", "")
        data = re.sub(r" ", "", data)
        m = re.findall(r'create_named_table\s*\(\s*"([^"]*)",([^\)]*)', data)
        for type in m:
            name = type[0]
            attr = type[1]
            attr = attr.replace('",', ",")
            attr = attr.split('"')
            vars = []
            for var in attr:
                if not var:
                    continue
                var = var.split(",")
                vars.append({"name": var[0], "type": var[1]})
            enums.append({"name": name, "vars": vars})
        data = open(file, "r").read()
        data = data.replace("\n", " ")
        m = re.findall(r"/\*(.*?)\*/", data)
        for extended_enum_info in m:
            extended_enum_info = extended_enum_info.strip()
            enum = extended_enum_info[: extended_enum_info.find(" ")]
            enum_to_mod = next((item for item in enums if item["name"] == enum), dict())
            current_var_to_mod = dict()
            if enum_to_mod:
                sub_matches = extended_enum_info.strip().split("//")
                collected_docs = ""
                for sub_match in sub_matches:
                    var_name = sub_match.strip()
                    var_to_mod = next(
                        (
                            item
                            for item in enum_to_mod["vars"]
                            if item["name"] == var_name
                        ),
                        dict(),
                    )
                    if var_to_mod:
                        if current_var_to_mod:
                            current_var_to_mod["docs"] = collected_docs
                        current_var_to_mod = var_to_mod
                        collected_docs = ""
                    else:
                        collected_docs += var_name + "<br/>"
            if current_var_to_mod:
                current_var_to_mod["docs"] = collected_docs
        for extended_type_info in m:
            extended_type_info = extended_type_info.strip()
            type = extended_type_info[: extended_type_info.find(" ")]
            type_to_mod = next((item for item in types if item["name"] == type), dict())
            current_var_to_mod = dict()
            if type_to_mod:
                sub_matches = extended_type_info.strip().split("//")
                collected_docs = ""
                for sub_match in sub_matches:
                    var_name = sub_match.strip()
                    var_to_mod = next(
                        (
                            item
                            for item in type_to_mod["vars"]
                            if item["name"] == var_name
                        ),
                        dict(),
                    )
                    if var_to_mod:
                        if current_var_to_mod:
                            current_var_to_mod["docs"] = collected_docs
                        current_var_to_mod = var_to_mod
                        collected_docs = ""
                    else:
                        collected_docs += var_name + "<br/>"
            if current_var_to_mod:
                current_var_to_mod["docs"] = collected_docs

    for file in api_files:
        comment = []
        name_next = False
        data = open(file, "r").read().split("\n")
        for line in data:
            line_clean = line.replace(" ", "")
            a = re.findall(
                r'create_named_table\s*\(\s*"([^"]*)"\/\/,([^\)]*)', line_clean
            )
            b = re.findall(r'create_named_table\s*\(\s*"([^"]*)",([^\)]*)', line_clean)
            c = re.findall(r'create_named_table\s*\(\s*"([^"]*)"\)', line_clean)
            m = a or b or ([c] if c else [])
            if m or name_next:
                enum = m[0][0] if m else line.strip('", ')
                enum_to_mod = next(
                    (item for item in enums if item["name"] == enum), dict()
                )
                if enum_to_mod:
                    enum_to_mod["comment"] = comment
                comment = []
                name_next = False
            elif "create_named_table" in line:
                name_next = True
            else:
                name_next = False
            if line == "":
                comment = []
            c = re.search(r"/// ?(.*)$", line)
            if c:
                comment.append(c.group(1))

    print_collecting_info("libraries")
    for file in api_files:
        data = open(file, "r").read()
        data = data.replace("\n", "")
        data = re.sub(r" ", "", data)
        m = re.search(r"open_libraries\s*\(([^\)]*)\)", data)
        if m:
            libs = m.group(1).split(",")
            for lib in libs:
                lualibs.append(lib.replace("sol::lib::", ""))

    print_collecting_info("aliases")
    data = open("../src/game_api/aliases.hpp", "r").read().split("\n")
    for line in data:
        if not line.endswith("NoAlias"):
            m = re.search(r"using\s*(\S*)\s*=\s*(\S*);?", line)
            if m:
                name = m.group(1)
                type = replace_fun(m.group(2))
                aliases.append({"name": name, "type": type})

    do_pickle(
        os.path.join(".db", CACHE_FILE),
        ("rpc", rpc),
        ("classes", classes),
        ("events", events),
        ("funcs", funcs),
        ("deprecated_funcs", deprecated_funcs),
        ("pre_gathered_vars", pre_gathered_vars),
        ("types", types),
        ("known_casts", known_casts),
        ("aliases", aliases),
        ("lualibs", lualibs),
        ("enums", enums),
        ("constructors", constructors),
    )
