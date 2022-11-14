import re

import os

import generate_util as gu
import parse_source as ps


if not os.path.exists("src/includes"):
    os.makedirs("src/includes")


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
    "vector<": "array<",
    "span<": "array<",
    "unordered_map<": "map<",
    "game_map<": "map<",
    ", identity_hasher<>": "",
    "const char*": "string",
    "wstring": "string",
    "u16string": "string",
    "char16_t": "string",
    "pair<": "tuple<",
    "std::": "",
    "sol::": "",
    "void": "",
    "constexpr": "",
    "static": "",
    "variadic_args va": "int, int...",
    "EmittedParticlesInfo": "array<Particle>",
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


def is_custom_type(name):
    for type in ps.types:
        if type["name"] == name:
            return True
    return False


def link_custom_type(ret):
    parts = re.findall(r"[\w_]+|[^\w_]+", ret)
    ret = ""
    for i, part in enumerate(parts):
        for type in ps.types:
            if (
                part == type["name"]
                and (len(parts) <= i + 1 or not parts[i + 1][0] in ")]")
                and not "`" in ret
            ):
                part = f"[{part}](#{part})"
        for enum in ps.enums:
            if (
                part == enum["name"]
                and (len(parts) <= i + 1 or not parts[i + 1][0] in ")]")
                and not "`" in ret
            ):
                part = f"[{part}](#{part})"
        for alias in ps.aliases:
            if (
                part == alias["name"]
                and (len(parts) <= i + 1 or not parts[i + 1][0] in ")]")
                and not "`" in ret
            ):
                part = f"[{part}](#Aliases)"
        ret += part
    return ret


def include_example(name):
    example = "examples/" + name + ".md"
    if os.path.exists(example):
        gu.print_console("Including " + example)
        file = open(example, "r")
        data = file.read()
        file.close()
        print("\n" + data + "\n")

    example = "examples/" + name + ".lua"
    if os.path.exists(example):
        gu.print_console("Including " + example)
        file = open(example, "r")
        data = file.read()
        file.close()
        print("\n```lua\n" + data + "\n```\n")


printed_funcs = []


def format_af(lf, af):
    ret = replace_all(af["return"]) or "nil"
    ret = ret.replace("<", "&lt;").replace(">", "&gt;")
    ret = link_custom_type(ret)
    name = lf["name"]
    param = replace_all(af["param"])
    param = link_custom_type(param)
    fun = f"{ret} {name}({param})".strip()
    return fun


def print_af(lf, af):
    fun = format_af(lf, af)
    print(f"#### {fun}\n")
    comments = af["comment"]
    if comments:
        comment = " ".join(comments)
        comment = link_custom_type(comment)
        print(comment)
        print()


def print_lf(lf):
    comments = lf["comment"]
    if comments and "NoDoc" in comments[0]:
        return
    name = lf["name"]
    if name in printed_funcs:
        return
    search_link = "https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=" + name
    print(f"\n### {name}\n")
    include_example(name)
    print(f"\n> Search script examples for [{name}]({search_link})\n")
    for af in ps.rpcfunc(lf["cpp"]):
        print_af(lf, af)
    for com in comments:
        com = link_custom_type(com)
        print(com)
    printed_funcs.append(lf["name"])


gu.setup_stdout("src/includes/_home.md")

print("# Overlunky/Playlunky Lua API")
print("## Read this first\n")
print(
    "- We try not to make breaking changes to the API, but some stupid errors or new stuff that was guessed wrong may have to be changed. Sorry!"
)
print(
    "- If you encounter something that doesn't seem quite right, please raise your voice instead of conforming to it outright."
)
print(
    "- This doc is generated from dozens of C++ files by a janky-ass python script however, so there may be weird documentation errors that hopefully don't reflect weird errors in the API."
)
print(
    "- This doc doesn't have a lot of examples, that's why we have [examples/](https://github.com/spelunky-fyi/overlunky/tree/main/examples). There are also [sample mods](https://spelunky.fyi/mods/?q=sample) for things that make more sense in Playlunky."
)
print(
    "- This doc and the examples are written for a person who already knows [how to program in Lua](http://lua-users.org/wiki/TutorialDirectory)."
)
print(
    "- This doc doesn't cover how to actually load scripts. Check the [README](https://github.com/spelunky-fyi/overlunky/#scripts) for instructions."
)
print(
    "- This doc is up to date for the Overlunky [WHIP build](https://github.com/spelunky-fyi/overlunky/releases/tag/whip) and Playlunky [nightly build](https://github.com/spelunky-fyi/Playlunky/releases/tag/nightly). If you're using a stable release from the past, you might find some things here don't work."
)
print(
    "- Set `OL_DEBUG=1` in the same environment where the game is running to keep the Overlunky terminal open for better debug prints. This could be `cmd` or even the system environment variables if playing on Steam. Playlunky will also print the messages to terminal (even from Overlunky) if ran with the `-console` switch."
)

print("\n## External Function Library")
print(
    'If you use a text editor/IDE that has a Lua linter available you can download [spel2.lua](https://raw.githubusercontent.com/spelunky-fyi/overlunky/main/docs/game_data/spel2.lua), place it in a folder of your choice and specify that folder as a "external function library". For example [VSCode](https://code.visualstudio.com/) with the [Lua Extension](https://marketplace.visualstudio.com/items?itemName=sumneko.lua) offers this feature. This will allow you to get auto-completion of API functions along with linting'
)

print("\n# Lua libraries")
print(
    "The following Lua libraries and their functions are available. You can read more about them in the [Lua documentation](https://www.lua.org/manual/5.4/manual.html#6). We're using Lua 5.4 with the [Sol C++ binding](https://sol2.readthedocs.io/en/latest/)."
)
for lib in ps.lualibs:
    print("\n## " + lib + "")
print("\n## json")
include_example("json")
print(
    """To save data in your mod it makes a lot of sense to use `json` to encode a table into a string and decode strings to table. For example this code that saves table and loads it back:

```lua
local some_mod_data_that_should_be_saved = {{
    kills = 0,
    unlocked = false
}}
set_callback(function(save_ctx)
    local save_data_str = json.encode(some_mod_data_that_should_be_saved)
    save_ctx:save(save_data_str)
end, ON.SAVE)

set_callback(function(load_ctx)
    local load_data_str = load_ctx:load()
    if load_data_str ~= "" then
        some_mod_data_that_should_be_saved = json.decode(load_data_str)
    end
end, ON.LOAD)
```"""
)
print("\n## inspect")
include_example("inspect")
print(
    """This module is a great substitute for `tostring` because it can convert any type to a string and thus helps a lot with debugging. Use for example like this:

```lua
local look_ma_no_tostring = {
    number = 15,
    nested_table = {
        array = {
            1,
            2,
            4
        }
    }
}
message(inspect(look_ma_no_tostring))
--[[prints:
{
    number = 15,
    nested_table = {
        array = { 1, 2, 4 }
    }
}
]]
```"""
)
print("\n## format")
include_example("format")
print(
    """This allows you to make strings without having to do a lot of `tostring` and `..` by placing your variables directly inside of the string. Use `F` in front of your string and wrap variables you want to print in `{}`, for example like this:

```lua
for _, player in players do
    local royal_title = nil
    if player:is_female() then
        royal_title = 'Queen'
    else
        royal_title = 'King'
    end
    local name = F'{player:get_name()} aka {royal_title} {player:get_short_name()}'
    message(name)
end
```"""
)

print("\n# Unsafe mode")
print(
    "Setting `meta.unsafe = true` enables the rest of the standard Lua libraries like `io` and `os`, loading dlls with require and `package.loadlib`. Using unsafe scripts requires users to enable the option in the overlunky.ini file which is found in the Spelunky 2 installation directory."
)

print("\n# Modules")
print(
    """You can load modules with `require "mymod"` or `require "mydir.mymod"`, just put `mymod.lua` in the same directory the script is, or in `mydir/` to keep things organized.

Check the [Lua tutorial](http://lua-users.org/wiki/ModulesTutorial) or examples how to actually make modules."""
)

print("\n# Aliases\n")
print(
    """
We use those to clarify what kind of values can be passed and returned from a function, even if the underlying type is really just an integer or a string. This should help to avoid bugs where one would for example just pass a random integer to a function expecting a callback id.

Name | Type
---- | ----"""
)

for alias in ps.aliases:
    name = alias["name"]
    type = alias["type"]
    print(f"{name} | {type}")


gu.setup_stdout("src/includes/_globals.md")

global_types = {
    "meta": "array<string>",
    "state": "StateMemory",
    "game_manager": "GameManager",
    "online": "Online",
    "players": "array<Player>",
    "savegame": "SaveData",
    "options": "array<mixed>",
    "prng": "PRNG",
}

print("# Global variables")
print("""These variables are always there to use.""")
for lf in ps.funcs:
    if lf["name"] in ps.not_functions:
        print("### " + lf["name"] + "\n")
        include_example(lf["name"])
        if lf["name"] in global_types:
            ret = global_types[lf["name"]]
            ret = ret.replace("<", "&lt;").replace(">", "&gt;")
            ret = link_custom_type(ret)
            print("#### " + ret + " " + lf["name"] + "\n")
        print(
            "> Search script examples for ["
            + lf["name"]
            + "](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q="
            + lf["name"]
            + ")\n"
        )
        for com in lf["comment"]:
            com = link_custom_type(com)
            print(com)


print("# Functions")
print(
    "The game functions like `spawn` use [level coordinates](#get_position). Draw functions use normalized [screen coordinates](#screen_position) from `-1.0 .. 1.0` where `0.0, 0.0` is the center of the screen."
)

func_cats = dict()

for func in ps.funcs:
    cat = "Generic functions"
    if any(
        subs in func["name"]
        for subs in [
            "prinspect",
            "messpect",
            "print",
            "message",
            "say",
            "speechbubble",
            "toast",
        ]
    ):
        cat = "Message functions"
    elif any(
        subs in func["name"]
        for subs in ["get_address", "get_rva", "raise", "dump_network"]
    ):
        cat = "Debug functions"
    elif any(subs in func["name"] for subs in ["_option"]):
        cat = "Option functions"
    elif any(subs in func["name"] for subs in ["feat"]):
        cat = "Feat functions"
    elif any(subs in func["name"] for subs in ["tile_code"]):
        cat = "Tile code functions"
    elif any(
        subs in func["name"]
        for subs in ["interval", "timeout", "callback", "set_on", "set_pre", "set_post"]
    ):
        cat = "Callback functions"
    elif any(subs in func["name"] for subs in ["flag"]):
        cat = "Flag functions"
    elif any(subs in func["name"] for subs in ["shop"]):
        cat = "Shop functions"
    elif any(subs in func["name"] for subs in ["_room"]):
        cat = "Room functions"
    elif any(subs in func["name"] for subs in ["spawn"]) or func["name"] in [
        "door",
        "layer_door",
    ]:
        cat = "Spawn functions"
    elif any(
        subs in func["name"]
        for subs in [
            "entity",
            "entities",
            "set_door",
            "get_door",
            "contents",
            "attach",
            "pick_up",
            "drop",
            "backitem",
            "carry",
            "door_at",
            "get_type",
            "kapala",
            "sparktrap",
            "explosion",
            "rope",
            "door",
            "blood",
            "olmec",
            "ghost",
            "jelly",
            "ankh",
            "player",
        ]
    ):
        cat = "Entity functions"
    elif any(subs in func["name"] for subs in ["movable", "behavior"]):
        cat = "Movable Behavior functions"
    elif any(subs in func["name"] for subs in ["theme"]):
        cat = "Theme functions"
    elif any(subs in func["name"] for subs in ["_lut", "_texture"]):
        cat = "Texture functions"
    elif any(subs in func["name"] for subs in ["_input", "_io", "mouse_"]):
        cat = "Input functions"
    elif any(
        subs in func["name"]
        for subs in [
            "bounds",
            "move",
            "position",
            "velocity",
            "distance",
            "size",
            "hitbox",
            "aabb",
            "zoom",
        ]
    ):
        cat = "Position functions"
    elif any(subs in func["name"] for subs in ["particle"]):
        cat = "Particle functions"
    elif any(subs in func["name"] for subs in ["string", "_name"]):
        cat = "String functions"
    elif any(subs in func["name"] for subs in ["udp_"]):
        cat = "Network functions"
    elif any(subs in func["name"] for subs in ["illuminati"]):
        cat = "Lighting functions"
    elif any(subs in func["name"] for subs in ["sound"]):
        cat = "Sound functions"
    if not cat in func_cats:
        func_cats[cat] = []
    func_cats[cat].append(func)

for cat in sorted(func_cats):
    print("\n## " + cat + "\n")
    for lf in sorted(func_cats[cat], key=lambda x: x["name"]):
        if len(ps.rpcfunc(lf["cpp"])):
            print_lf(lf)
        elif not (lf["name"].startswith("on_") or lf["name"] in ps.not_functions):
            if lf["comment"] and "NoDoc" in lf["comment"][0]:
                continue
            m = re.search(r"\(([^\{]*)\)\s*->\s*([^\{]*)", lf["cpp"])
            m2 = re.search(r"\(([^\{]*)\)", lf["cpp"])
            ret = "nil"
            param = ""
            if m:
                ret = replace_all(m.group(2)).strip() or "nil"
            if m or m2:
                param = (m or m2).group(1)
                param = replace_all(param).strip()
            name = lf["name"]
            ret = link_custom_type(ret)
            ret = ret.replace("<", "&lt;").replace(">", "&gt;")
            param = link_custom_type(param)
            fun = f"{ret} {name}({param})".strip()
            search_link = (
                "https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=" + name
            )
            print(f"\n### {name}\n")
            include_example(name)
            print(f"\n> Search script examples for [{name}]({search_link})\n")
            print(f"#### {fun}\n")
            for com in lf["comment"]:
                com = link_custom_type(com)
                print(com.replace("```lua", "\n```lua"))

print("\n## Deprecated functions\n")
print(
    "<aside class='warning'>These functions still exist but their usage is discouraged, they all have alternatives mentioned here so please use those!</aside>"
)

for lf in ps.events:
    if lf["name"].startswith("on_"):
        print("\n### " + lf["name"] + "\n")
        include_example(lf["name"])
        for com in lf["comment"]:
            com = link_custom_type(com)
            print(com)

for lf in ps.deprecated_funcs:
    lf["comment"].pop(0)
    if len(ps.rpcfunc(lf["cpp"])):
        print_lf(lf)
    elif not (lf["name"].startswith("on_") or lf["name"] in ps.not_functions):
        if lf["comment"] and "NoDoc" in lf["comment"][0]:
            continue
        m = re.search(r"\(([^\{]*)\)\s*->\s*([^\{]*)", lf["cpp"])
        m2 = re.search(r"\(([^\{]*)\)", lf["cpp"])
        ret = "nil"
        param = ""
        if m:
            ret = replace_all(m.group(2)).strip() or "nil"
        if m or m2:
            param = (m or m2).group(1)
            param = replace_all(param).strip()
        name = lf["name"]
        fun = f"{ret} {name}({param})".strip()
        search_link = "https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=" + name
        print(f"\n### {name}\n")
        include_example(name)
        print(f"\n> Search script examples for [{name}]({search_link})\n")
        print(f"`{fun}`<br/>")
        for com in lf["comment"]:
            com = link_custom_type(com)
            print(com)


gu.setup_stdout("src/includes/_types.md")

type_cats = dict()

for type in ps.types:
    cat = "Generic types"
    type_cat = "Non-Entity types"

    if "Floor" in type["base"] or type["name"] == "Floor":
        cat = "Floor entities"
        type_cat = "Entity types"
    elif "PowerupCapable" in type["base"] or type["name"] == "PowerupCapable":
        cat = "Monsters, Inc."
        type_cat = "Entity types"
    elif "Movable" in type["base"] or type["name"] == "Movable":
        cat = "Movable entities"
        type_cat = "Entity types"
    elif "Entity" in type["base"] and any(subs in type["name"] for subs in ["Logical"]):
        cat = "Logical entities"
        type_cat = "Entity types"
    elif "Entity" in type["base"] and any(subs in type["name"] for subs in ["BG"]):
        cat = "Background entities"
        type_cat = "Entity types"
    elif "Entity" in type["base"] and any(subs in type["name"] for subs in ["Effect"]):
        cat = "Effect entities"
        type_cat = "Entity types"
    elif "Entity" in type["base"] or type["name"] == "Entity":
        cat = "Generic entities"
        type_cat = "Entity types"

    elif (
        "Screen" in type["base"]
        or type["name"] == "Screen"
        or any(
            subs in type["name"]
            for subs in ["Screen", "UI", "FlyingThing", "SaveRelated"]
        )
    ):
        cat = "Screen types"
    elif "JournalPage" in type["base"] or type["name"] == "JournalPage":
        cat = "Journal types"
    elif any(subs in type["name"] for subs in ["Theme"]):
        cat = "Theme types"
    elif any(subs in type["name"] for subs in ["Context"]):
        cat = "Callback context types"
    elif any(subs in type["name"] for subs in ["SaveData", "Constellation"]):
        cat = "Savegame types"
    elif any(subs in type["name"] for subs in ["Arena"]):
        cat = "Arena types"
    elif any(subs in type["name"] for subs in ["Online"]):
        cat = "Online types"
    elif any(subs in type["name"] for subs in ["Liquid"]):
        cat = "Liquid types"
    elif any(subs in type["name"] for subs in ["Logic"]):
        cat = "Logic types"
    elif any(subs in type["name"] for subs in ["Light", "Illumination"]):
        cat = "Lighting types"
    elif any(
        subs in type["name"] for subs in ["Animation", "EntityDB", "Inventory", "Ai"]
    ):
        cat = "Entity related types"
    elif any(
        subs in type["name"]
        for subs in [
            "StateMemory",
            "Items",
            "GameManager",
            "GameProps",
            "Camera",
            "QuestsInfo",
            "PlayerSlot",
        ]
    ):
        cat = "State types"
    elif any(subs in type["name"] for subs in ["Gamepad", "ImGuiIO", "Input"]):
        cat = "Input types"
    elif any(subs in type["name"] for subs in ["Texture", "Rendering"]):
        cat = "Texture types"
    elif any(subs in type["name"] for subs in ["Particle"]):
        cat = "Particle types"
    elif any(subs in type["name"] for subs in ["DoorCoords", "LevelGen"]):
        cat = "Levelgen types"
    elif any(subs in type["name"] for subs in ["Sound"]):
        cat = "Sound types"
    if not type_cat in type_cats:
        type_cats[type_cat] = dict()
    if not cat in type_cats[type_cat]:
        type_cats[type_cat][cat] = []
    type_cats[type_cat][cat].append(type)

for type_cat in type_cats:
    print("\n# " + type_cat + "\n")
    for cat in sorted(type_cats[type_cat], key=lambda x: x):
        print("\n## " + cat + "\n")
        for type in sorted(type_cats[type_cat][cat], key=lambda x: x["name"]):
            type_name = type["name"]
            print("\n### " + type_name + "\n")
            include_example(type_name)
            if "comment" in type:
                for com in type["comment"]:
                    com = link_custom_type(com)
                    print(com)
            if type["base"]:
                print("Derived from", end="")
                bases = type["base"].split(",")
                for base in bases:
                    print(" [" + base + "](#" + base + ")", end="")
                print("\n")
            print(
                """
Type | Name | Description
---- | ---- | -----------"""
            )
            ctors = (
                [] if type_name not in ps.constructors else ps.constructors[type_name]
            )
            for var in ctors + type["vars"]:
                if "comment" in var and "NoDoc" in var["comment"]:
                    continue
                var_name = var["name"]
                search_link = (
                    "https://github.com/spelunky-fyi/overlunky/search?l=Lua&q="
                    + var_name
                )
                if "signature" in var:
                    signature = var["signature"].strip()
                    ret = ""
                    if var_name == type_name:
                        ret = type_name
                        signature = f"new({signature})"
                    else:
                        n = re.search(r"^\s*([^\( ]*)(\(([^\)]*))", signature)
                        m = re.search(
                            r"\s*([^\(]*)\s+([^\( ]*)(\(([^\)]*))?", signature
                        )

                        name = ""
                        param = ""

                        if n:
                            name = n.group(1)
                            if n.group(2):
                                param = replace_all(n.group(2).strip()) + ")"
                            signature = name + param
                        elif m:
                            ret = replace_all(m.group(1)) or "nil"
                            name = m.group(2)
                            if m.group(3):
                                param = replace_all(m.group(3).strip()) + ")"
                            signature = name + param
                    signature = signature.strip()
                    ret = ret.replace("<", "&lt;").replace(">", "&gt;")
                    ret = link_custom_type(ret)
                    print(f"{ret} | [{signature}]({search_link}) | ", end="")
                else:
                    ret = ""
                    name = var["name"]
                    print(f"{ret} | [{name}]({search_link}) | ", end="")
                if "docs" in var:
                    print(link_custom_type(var["docs"]), end=" ")
                if "comment" in var and var["comment"]:
                    print(link_custom_type("<br/>".join(var["comment"])))
                else:
                    print("")


gu.setup_stdout("src/includes/_casting.md")

print(
    """
# Automatic casting of entities

When using `get_entity()` the returned entity will automatically be of the correct type. It is not necessary to use the `as_<typename>` functions.

To figure out what type of entity you get back, consult the [entity hierarchy list](entities-hierarchy.md).

You can also use the types (uppercase `<typename>`) as `ENT_TYPE.<typename>` in `get_entities` functions and `pre/post spawn` callbacks

For reference, the available `as_<typename>` functions are listed below:\n"""
)

for known_cast in ps.known_casts:
    print("- " + known_cast)

gu.setup_stdout("src/includes/_enums.md")

print("\n# Enums\n")
print(
    """Enums are like numbers but in text that's easier to remember.

```lua
set_callback(function()
    if state.theme == THEME.COSMIC_OCEAN then
        x, y, l = get_position(players[1].uid)
        spawn(ENT_TYPE.ITEM_JETPACK, x, y, l, 0, 0)
    end
end, ON.LEVEL)
```"""
)
for type in sorted(ps.enums, key=lambda x: x["name"]):
    print("\n## " + type["name"] + "\n")
    search_link = (
        "https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=" + type["name"]
    )
    print(f"\n> Search script examples for [{type['name']}]({search_link})\n")
    if "comment" in type:
        print(link_custom_type("<br/>".join(type["comment"])))
    print(
        """
Name | Data | Description
---- | ---- | -----------"""
    )
    for var in type["vars"]:
        if var["name"]:
            print(
                "["
                + var["name"]
                + "](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q="
                + type["name"]
                + "."
                + var["name"]
                + ") | "
                + var["type"]
                + " | ",
                end="",
            )
        else:
            print(var["type"] + " |  | ", end="")
        if "docs" in var:
            print(link_custom_type(var["docs"]))
        else:
            print("")
