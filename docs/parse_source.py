import re
import os
from ast import literal_eval

from generate_util import *
from parse_cache import *

ENABLE_CACHING = True
CACHE_FILE = None

if not os.path.exists(".db"):
    os.makedirs(".db")

# this is common for generator.py and validator.py, not actually used nor should it be used here
replace_table = {
    # standard basic types
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
    "ssize_t": "int",
    "size_t": "int",
    "char*": "string",
    "wstring": "string",
    "u16string": "string",
    "string_view": "string",
    "char16_t*": "string",
    "char16_t": "char",
    "pair<": "tuple<",
    # std containers
    "custom_vector<": "vector<",
    "custom_map<": "map<",
    "custom_unordered_map<": "map<",
    "custom_set<": "set<",
    "custom_unordered_set<": "set<",
    "game_vector<": "vector<",
    "game_map<": "map<",
    "game_unordered_map<": "map<",
    "game_set<": "set<",
    "game_unordered_set<": "set<",
    "unordered_map<": "map<",  # doesn't seam to matter for lua if it's ordered or not
    "unordered_set<": "set<",  # doesn't seam to matter for lua if it's ordered or not
    # special
    "variadic_args va": "ENT_TYPE, ENT_TYPE...",
    "EmittedParticlesInfo": "array<Particle>",
    "ImVec2": "Vec2",
    "SoundCallbackFunction": "function",
    "sol::object": "any",
    "BucketItem": "any",
    "ENTITY_MASK": "MASK",
    # removers
    ", identity_hasher<>": "",
    "std::": "",
    "sol::": "",
    "void": "",
    "constexpr": "",
    "const ": "",
    "static": "",
    "[[nodiscard]]": "",
    "[[maybe_unused]]": "",
    "inline ": "",
    "self_return<&": "",
    ">()": "",
    "unsigned": "",
    "sol::no_constructor": "",
}

header_files = [
    "../src/game_api/math.hpp",
    "../src/game_api/rpc.hpp",
    "../src/game_api/entity_lookup.hpp",
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
    "../src/game_api/illumination.hpp",
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
    "../src/game_api/bucket.hpp",
    "../src/game_api/socket.hpp",
    "../src/game_api/savestate.hpp",
    "../src/game_api/game_patches.hpp",
    "../src/game_api/liquid_engine.hpp",
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
    "../src/game_api/script/usertypes/logic_lua.cpp",
    "../src/game_api/script/usertypes/bucket_lua.cpp",
    "../src/game_api/script/usertypes/color_lua.cpp",
    "../src/game_api/script/usertypes/deprecated_func.cpp",
    "../src/game_api/script/usertypes/spawn_lua.cpp",
    "../src/game_api/script/usertypes/options_lua.cpp",
    "../src/game_api/script/usertypes/game_patches_lua.cpp",
]
vtable_api_files = [
    "../src/game_api/script/usertypes/vtables_lua.cpp",
    "../src/game_api/script/usertypes/theme_vtable_lua.hpp",
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

cpp_type_exceptions = [
    "Players",
    "CutsceneBehavior",
    "CustomCutsceneBehavior",
    "DoorCoords",
]
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

replace_fun_import = None

reSignature = re.compile(r"(?:\bsignature\b.*is|function) `?([\S]*) (\w*?)\((.*?)\)")


def cb_signature_dict(ret, param):
    return {
        "return": ret,
        "param": param,
    }


def custom_split(var):
    result = []
    level = 0
    current = ""
    for char in var:
        if char == "," and level == 0:
            result.append(current.strip())
            current = ""
        else:
            current += char
            if char == "[" or char == "(" or char == "<":
                level += 1
            elif char == "]" or char == ")" or char == ">":
                level -= 1
    result.append(current.strip())
    return result


def get_cb_signature(text):
    signature_ms = reSignature.findall(text)
    if not signature_ms:
        return None

    cb_signatures = {}
    for m in signature_ms:
        cb_signatures[m[1]] = cb_signature_dict(m[0], m[2])
    return cb_signatures


def camel_case_to_snake_case(name):
    return re.sub(r"(?<!^)(?=[A-Z])", "_", name).lower()


def fix_spaces(thing):
    thing = thing.strip()
    thing = re.sub(r"\s{2,}", " ", thing)  # change double spaces into single
    thing = re.sub(r"(?<=\(|\<)\s", "", thing)  # remove spaces after ( or <
    return thing.replace("*", "").replace("&", "")  # remove * and &


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


def replace_fun(text):
    return fix_spaces(replace_fun_import(text))


def configure_parse(replace_function, cache_file):
    global replace_fun_import, CACHE_FILE
    replace_fun_import = replace_function
    CACHE_FILE = cache_file


def run_parse():
    if not replace_fun_import:
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
            skip += line.count("{") - line.count("}")
            c = re.search(r"/// ?(.*)$", line)
            if c:
                comment.append(c.group(1))
            m = re.search(r"\s*(.*)\s+([^\(]*)\(([^\)]*)", line)
            if m:
                if skip == 0 or file.endswith("script.hpp"):
                    param = replace_fun(m.group(3))
                    ret = replace_fun(m.group(1))
                    rpc.append(
                        {
                            "return": ret,
                            "name": m.group(2),
                            "param": param,
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

                        m = re.search(
                            r"\s*(?:virtual\s)?(.*)\s+([\w]+)\(([^\)]*)", line
                        )
                        if m:
                            name = m[2]
                            # move ctor is useless for Lua
                            is_move_ctr = (
                                re.fullmatch(r"^[\w]*$", name)
                                and re.fullmatch(rf"\s*{name}\s*&&[^,]*", m[3])
                                and not m[1]
                            )
                            if not is_move_ctr:
                                if name not in member_funs:
                                    member_funs[name] = []
                                param = m[3]
                                if " " not in param:
                                    param = f"{param} {param.lower()}"
                                member_funs[name].append(
                                    {
                                        "return": replace_fun(m[1]),
                                        "name": m[2],
                                        "param": replace_fun(param),
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
                                            "type": replace_fun(vars_type),
                                            "name": m_var,
                                            "comment": comment,
                                        }
                                    )
                                member_vars.append(
                                    {
                                        "type": replace_fun(vars_type),
                                        "name": m[2],
                                        "comment": comment,
                                    }
                                )
                            else:
                                member_vars.append(
                                    {
                                        "type": replace_fun(m[1]),
                                        "name": m[2],
                                        "comment": comment,
                                    }
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

                    if "member_funs" not in underlying_cpp_type:
                        underlying_cpp_type["member_funs"] = {}

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
                                    "cb_signature": af["cb_signature"]
                                    if "cb_signature" in af
                                    else None,
                                }
                            )
                    else:
                        m = re.search(r"\(([^\{]*)\)\s*->\s*([^\{]*)", cpp)
                        m2 = re.search(r"\(([^\{]*)\)", cpp)
                        ret = "nil"
                        param = ""
                        if m:
                            ret = replace_fun(m.group(2)) or "nil"
                        if m or m2:
                            param = (m or m2).group(1)
                            param = replace_fun(param)
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
                        cb_signature = (
                            get_cb_signature(" ".join(comment)) if comment else None
                        )
                        func = {
                            "name": m.group(1),
                            "cpp": replace_fun(m.group(2)),
                            "comment": comment,
                            "cb_signature": cb_signature,
                            "file": file,
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

    print_collecting_info("vtables")
    vtables_by_usertype = {}
    vtable_override_tables = {}
    for file in vtable_api_files:
        data = open(file, "r").read().split("\n")
        vtables = {}
        vtable_name = None
        skip = False
        for line in data:
            if vtable_def := re.search(
                r'static (\w*) \w*\(lua, lua\["(.*)"\](, "(.*)")?\)', line
            ):
                vtable_name = vtable_def.group(1)
                usertype = vtable_def.group(2)
                table_name = vtable_def.group(4)
                vtable_decl = vtables[vtable_name]
                vtable_decl["usertype"] = usertype
                vtable_decl["override_table"] = table_name

                if table_name not in vtable_override_tables:
                    vtable_override_tables[table_name] = []
                override_table = vtable_override_tables[table_name]
                for entry in vtable_decl["entries"].values():
                    override_table.append(
                        {"name": entry["name"].upper(), "type": str(entry["index"])}
                    )

                vtable_name = None
            elif vtable_name == None and "HookableVTable<" in line:
                vtable_name = line.split()[1]
                vtable_entries = {}
            elif vtable_name != None:
                if entry_math := re.search(r"/// NoDoc", line):
                    skip = True
                    continue
                if entry_math := re.search(r"VTableEntry<(.*)>", line):
                    if skip:
                        skip = False
                        continue
                    [name, index, signature_and_binder] = entry_math.group(1).split(
                        ",", 2
                    )
                    name = name.strip('" ')
                    index = literal_eval(index.strip())
                    if binds := re.search(r"BackBinder<([^>]*)>", signature_and_binder):
                        binds = binds.group(1)
                        binds = "{} {}".format(binds, camel_case_to_snake_case(binds))
                    else:
                        binds = None
                    signature = re.sub(
                        r"\), BackBinder<([^>]*)>", r", \g<1>)", signature_and_binder
                    ).replace("(, ", "(")
                    func_ref = re.search(r"MemFun<&?([\w]+)::([\w]+)>", line)

                    if func_ref and func_ref.group(1) is not None:
                        for item in classes:
                            if item["name"] != func_ref.group(1):
                                continue
                            func_name = func_ref.group(2)
                            # if it throws error at `item["member_funs"][func_name]` then the virtual defined using MemFun was not found in the parsed class/struct code
                            if (
                                item["member_funs"]
                                and item["member_funs"][func_name]
                                and len(item["member_funs"][func_name]) > 0
                            ):
                                # choosing the last one since that's the actual virtual signature most of the time
                                func = item["member_funs"][func_name][-1]
                                vtable_entries[name] = {
                                    "name": name,
                                    "index": index,
                                    "ret": func["return"],
                                    "args": [t for t in func["param"].split(",")],
                                    "binds": binds,
                                    "ref": func_name,
                                }
                    else:
                        signature = re.search(r"([_a-zA-Z][\w]*.*)\((.*)\)", signature)
                        ret = signature.group(1)
                        args = [t for t in signature.group(2).split(",")]
                        vtable_entries[name] = {
                            "name": name,
                            "index": index,
                            "ret": ret,
                            "args": args,
                            "binds": binds,
                        }
                    if ">>;" in line:
                        vtables[vtable_name] = {
                            "name": vtable_name,
                            "entries": vtable_entries,
                        }
                        vtable_name = None
                        vtable_entries = None

        for vtable_decl in vtables.values():
            vtables_by_usertype[vtable_decl["usertype"]] = vtable_decl

    print_collecting_info("usertypes")
    for file in api_files:
        data = open(file, "r").read()
        data = data.replace("\n", "")
        data = fix_spaces(data)
        reParticleHelper = re.compile(r"MakeParticleMemberAccess<(.+?)>\(\)")
        data = reParticleHelper.sub(r"\1", data)
        m = re.findall(
            r'(auto ([a-z]+_type) =)? lua\.new_usertype\<([^\>]*?)\>\s*\(\s*"([^"]*)",?(.*?)\);',
            data,
        )
        for type in m:
            container = type[1]
            cpp_type = type[2]
            name = type[3]
            attr = type[4]
            extra_comments = {}
            if container:
                extra = []
                n = re.findall(
                    r'(/// [\w _#\[\]\-+()<>.,`"\'=]*?)?' + container + r'\[([\w":]+)\] = ([^;]+);', data
                )
                for var in n:
                    if var[0].startswith("/// NoDoc"):
                        continue
                    if var[0]:
                        extra_comments[var[1].replace('"', '')] = var[0][4:]
                    extra.append(",".join(var[1:3]))
                extra = ",".join(extra)
                if attr:
                    attr = attr + "," + extra
                else:
                    attr = extra

            base = ""
            bm = re.search(r"sol::bases<([^\]]*)>", attr)
            if bm:
                base = bm.group(1).replace(" ", "")
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
                var = custom_split(var)
                if len(var) < 2:
                    continue
                if var[0] == "sol::no_constructor":
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
                cpp = replace_fun(var[1]) #TODO: should probably be done later, so the regex doesn't have to relay on some of the changes, also generate_emmylua.py uses some unique formats replacements

                if var[1].startswith("sol::property"): # fix for sol::property
                    param_match = re.match(
                        rf"property\(\[\]\({underlying_cpp_type['name']}&(\w+)\)",
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

                if var_name.startswith("sol::constructors"):
                    for fun in underlying_cpp_type["member_funs"][cpp_type]:
                        param = fun["param"]

                        if cpp_type not in constructors:
                            constructors[cpp_type] = []
                        constructors[cpp_type].append(
                            {
                                "name": cpp_type,
                                "signature": param,
                                "comment": fun["comment"],
                            }
                        )
                elif cpp.startswith("[]("): # lambdas
                    param_match = re.match(r"\[\]\(([\w <>\?&*:,]+)?\) -> ([\w.*&<>\?\[\]:]+)?(?: )?{", cpp)
                    if param_match:
                        ret = param_match.group(2)
                        if ret is None:
                            ret = "nil"

                        sig = param_match.group(1)
                        if sig.startswith(
                            cpp_type
                        ):  # remove the self parameter if present
                            first_param_end = sig.find(",") + 1
                            if first_param_end == 0:
                                sig = ""
                            else:
                                sig = sig[first_param_end:].strip()
                        vars.append(
                            {
                                "name": var_name,
                                "type": cpp,
                                "signature": f"{ret} {var_name}({sig})",
                                "comment": [extra_comments[var_name]] if var_name in extra_comments else [],
                                "function": True,
                                "cb_signature": "",
                            }
                        )
                elif var_name.startswith("sol::base_classes"):
                    bm = re.search(r"bases<([^\]]*)", cpp)
                    if bm:
                        base = bm.group(1).replace(" ", "")
                elif cpp_name in underlying_cpp_type["member_funs"]:
                    for fun in underlying_cpp_type["member_funs"][cpp_name]:
                        ret = fun["return"] or "nil"
                        param = fun["param"]
                        sig = f"{ret} {var_name}({param})"
                        cb_signature = (
                            get_cb_signature(" ".join(fun["comment"]))
                            if fun["comment"]
                            else None
                        )
                        if var_name in extra_comments:
                            fun["comment"].append(extra_comments[var_name])

                        vars.append(
                            {
                                "name": var_name,
                                "type": cpp,
                                "signature": sig,
                                "comment": fun["comment"],
                                "function": True,
                                "cb_signature": cb_signature,
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
                        if var_name in extra_comments:
                            underlying_cpp_var["comment"].append(extra_comments[var_name])
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
                            r"-> ([:<>\w\[\], *]+) {", var[1]
                        )  # Use var[1] instead of cpp because it could be replaced on the sol::property stuff
                        if m_return_type:
                            type = replace_fun(m_return_type[1])
                            sig = f"{type} {var_name}"
                            vars.append(
                                {"name": var_name, "type": cpp, "signature": sig, "comment": [extra_comments[var_name]] if var_name in extra_comments else []}
                            )
                        else:
                            vars.append({"name": var_name, "type": cpp, "comment": [extra_comments[var_name]] if var_name in extra_comments else []})

            if name in vtables_by_usertype:
                vtable = vtables_by_usertype[name]
                override_table = vtable["override_table"]
                vars.append(
                    {
                        "name": "set_pre_virtual",
                        "signature": f"CallbackId set_pre_virtual({override_table} entry, function fun)",
                        "comment": [
                            "Hooks before the virtual function at index `entry`."
                        ],
                        "function": True,
                    }
                )
                vars.append(
                    {
                        "name": "set_post_virtual",
                        "signature": f"CallbackId set_post_virtual({override_table} entry, function fun)",
                        "comment": [
                            "Hooks after the virtual function at index `entry`."
                        ],
                        "function": True,
                    }
                )
                vars.append(
                    {
                        "name": "clear_virtual",
                        "signature": f"nil clear_virtual(CallbackId callback_id)",
                        "comment": [
                            "Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook."
                        ],
                        "function": True,
                    }
                )

                for entry in vtable["entries"].values():
                    entry_name = entry["name"]

                    func_name = entry["ref"] if "ref" in entry else entry["name"]
                    pre_signature = None
                    post_signature = None
                    cpp_comment = []
                    if func_name in underlying_cpp_type["member_funs"]:
                        for fun in underlying_cpp_type["member_funs"][func_name]:
                            ret = replace_fun(fun["return"])
                            ret = f"optional<{ret}>" if ret else "bool"
                            ret = ret if entry_name != "dtor" else "nil"
                            args = replace_fun(fun["param"])
                            args = f"{name} self, {args}" if args else f"{name} self"
                            binds = entry["binds"]
                            if binds:
                                args = f"{args}, {binds}"
                            pre_signature = f"{ret} {entry_name}({args})"
                            post_signature = f"nil {entry_name}({args})"
                            cpp_comment = fun["comment"]
                            if cpp_comment:
                                cpp_comment = ["Virtual function docs:"] + cpp_comment
                            break
                    else:
                        ret = replace_fun(entry["ret"])
                        ret = f"optional<{ret}>" if ret else "bool"
                        ret = ret if entry_name != "dtor" else "nil"
                        args = replace_fun(" ".join(entry["args"]))
                        args = f"{name} self, {args}" if args else f"{name} self"
                        pre_signature = f"{ret} {entry_name}({args})"
                        post_signature = f"nil {entry_name}({args})"

                    vars.append(
                        {
                            "name": f"set_pre_{entry_name}",
                            "signature": f"CallbackId set_pre_{entry_name}(function fun)",
                            "comment": [
                                "Hooks before the virtual function.",
                                f"The callback signature is `{pre_signature}`",
                            ]
                            + cpp_comment,
                            "function": True,
                            "cb_signature": {"fun": cb_signature_dict(ret, args)},
                        }
                    )
                    vars.append(
                        {
                            "name": f"set_post_{entry_name}",
                            "signature": f"CallbackId set_post_{entry_name}(function fun)",
                            "comment": [
                                "Hooks after the virtual function.",
                                f"The callback signature is `{post_signature}`",
                            ]
                            + cpp_comment,
                            "function": True,
                            "cb_signature": {"fun": cb_signature_dict(ret, args)},
                        }
                    )

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

    print_collecting_info("enums") # TODO: should probably improve how this handles comments
    for file in api_files:
        data = open(file, "r").read()
        data = data.replace("\n", "")
        data = re.sub(r" ", "", data)
        m = re.findall(r'create_named_table\s*\(\s*"([^"]*)"\/\/,([^\)]*)', data)
        for type in m:
            name = type[0]
            attr = type[1]
            attr = attr.replace("//,", ",")
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

    for name, values in vtable_override_tables.items():
        enums.append({"name": name, "vars": values})

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
                if len(var) > 1:
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
                sub_matches = extended_enum_info.strip()
                sub_matches = re.split('(?<!:)\s*//\s*', sub_matches)
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
                sub_matches = extended_type_info.strip()
                sub_matches = re.split('(?<!:)\s*//\s*', sub_matches)
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
