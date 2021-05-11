import re

# redirect stdout to script-api.md
import sys
sys.stdout = open('script-api.md', 'w')

header_files = ['../src/game_api/rpc.hpp', '../src/game_api/script.hpp', '../src/game_api/entity.hpp']
api_files = ['../src/game_api/script.cpp']
rpc = []
events = []
funcs = []
types = []
lualibs = []
enums = []
replace = {
    'uint8_t': 'int',
    'uint16_t': 'int',
    'uint32_t': 'int',
    'int32_t': 'int',
    'ImU32': 'int',
    'vector': 'array',
    'wstring': 'string',
    'std::': '',
    'sol::': '',
    'void': '',
    'variadic_args va': 'int, int...'
}
comment = []
skip = False

def getfunc(name):
    for func in funcs:
        if func['name'] == name:
            return func
    return False

def rpcfunc(name):
    ret = []
    for func in rpc:
        if func['name'] == name:
            ret.append(func)
    return ret

def replace_all(text, dic):
    for i, j in dic.items():
        text = text.replace(i, j)
    return text

def print_af(lf, af):
    ret = replace_all(af['return'], replace)
    param = replace_all(af['param'], replace)
    print('### [`'+lf['name']+'`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q='+lf['name']+')')
    if param: print('#### Params: `'+param+'`')
    if ret: print('#### Returns: `'+ret+'`')
    for com in lf['comment']:
        print(('#### ' if com.startswith('Returns:') else '')+com)

for file in header_files:
    data = open(file, 'r').read().split('\n')
    for line in data:
        line = line.replace('*', '')
        s = re.search(r'{', line)
        if s:
            skip = True
        s = re.search(r'}', line)
        if s:
            skip = False
        c = re.search(r'/// ?(.*)$', line)
        if c:
            comment.append(c.group(1))
        m = re.search(r'\s*(.*)\s+([^\(]*)\(([^\)]*)', line)
        if m:
            if not skip or file.endswith('script.hpp'): rpc.append({'return': m.group(1), 'name': m.group(2), 'param': m.group(3), 'comment': comment})
        else:
            comment = []

for file in api_files:
    data = open(file, 'r').read().split('\n')
    for line in data:
        line = line.replace('*', '')
        m = re.search(r'lua\[[\'"]([^\'"]*)[\'"]\];', line)
        if m:
            events.append({'name': m.group(1), 'comment': comment})
        else:
            comment = []
        c = re.search(r'/// ?(.*)$', line)
        if c:
            comment.append(c.group(1))
        else:
            comment = []

for file in api_files:
    data = open(file, 'r').read().split('\n')
    for line in data:
        line = line.replace('*', '')
        a = re.search(r'lua\[[\'"]([^\'"]*)[\'"]\]\s+=\s+(.*);', line)
        b = re.search(r'lua\[[\'"]([^\'"]*)[\'"]\]\s+=\s+(.*)$', line)
        if a:
            if not getfunc(a.group(1)): funcs.append({'name': a.group(1), 'cpp': a.group(2), 'comment': comment})
            comment = []
        elif b:
            if not getfunc(b.group(1)): funcs.append({'name': b.group(1), 'cpp': b.group(2), 'comment': comment})
            comment = []
        c = re.search(r'/// ?(.*)$', line)
        if c:
            comment.append(c.group(1))

for file in api_files:
    data = open(file, 'r').read()
    data = data.replace('\n', '')
    data = re.sub(r' ', '', data)
    m = re.findall(r'new_usertype\<.*?\>\s*\(\s*"([^"]*)",(.*?)\);', data)
    for type in m:
        name = type[0]
        attr = type[1]
        base = ""
        bm = re.search(r'sol::bases<([^\]]*)>', attr)
        if bm:
            base = bm.group(1)
        attr = attr.replace('",', ',')
        attr = attr.split('"')
        vars = []
        for var in attr:
            if not var: continue
            var = var.split(',')
            if 'table_of' in var[1]:
                var[1] = var[1].replace('table_of(', '')+'[]'
            vars.append({ 'name': var[0], 'type': var[1] })
        types.append({'name': name, 'vars': vars, 'base': base})
    data = open(file, 'r').read()
    data = data.replace('\n', ' ')
    m = re.findall(r'/\*(.*?)\*/', data)
    for extended_type_info in m:
        extended_type_info = extended_type_info.strip()
        type = extended_type_info[:extended_type_info.find(' ')]
        type_to_mod = next((item for item in types if item['name'] == type), dict())
        if type_to_mod:
            sub_matches = re.findall(r'([^\s]*?)\s*([^\s]*)(\(.*?\))', extended_type_info.strip())
            for sub_match in sub_matches:
                var_name = sub_match[1]
                var_to_mod = next((item for item in type_to_mod['vars'] if item['name'] == var_name), dict())
                if var_to_mod:
                    var_to_mod['signature'] = sub_match[0] + ' ' + var_name + sub_match[2]

for file in api_files:
    data = open(file, 'r').read()
    data = data.replace('\n', '')
    data = re.sub(r' ', '', data)
    m = re.findall(r'create_named_table\s*\(\s*"([^"]*)"\/\/,([^\)]*)', data)
    for type in m:
        name = type[0]
        attr = type[1]
        attr = attr.replace('//', '')
        attr = attr.replace('",', ',')
        attr = attr.split('"')
        vars = []
        for var in attr:
            if not var: continue
            var = var.split(',')
            var[1] = var[1].replace('__', ' ')
            vars.append({ 'name': var[0], 'type': var[1] })
        enums.append({'name': name, 'vars': vars})

for file in api_files:
    data = open(file, 'r').read()
    data = data.replace('\n', '')
    data = re.sub(r' ', '', data)
    m = re.findall(r'new_enum\s*\(\s*"([^"]*)",([^\)]*)', data)
    for type in m:
        name = type[0]
        attr = type[1]
        attr = attr.replace('",', ',')
        attr = attr.split('"')
        vars = []
        for var in attr:
            if not var: continue
            var = var.split(',')
            vars.append({ 'name': var[0], 'type': var[1] })
        enums.append({'name': name, 'vars': vars})
    data = open(file, 'r').read()
    data = data.replace('\n', ' ')
    m = re.findall(r'/\*(.*?)\*/', data)
    for extended_enum_info in m:
        extended_enum_info = extended_enum_info.strip()
        enum = extended_enum_info[:extended_enum_info.find(' ')]
        enum_to_mod = next((item for item in enums if item['name'] == enum), dict())
        current_var_to_mod = dict()
        if enum_to_mod:
            sub_matches = re.findall(r'\/\/\s*([^\/\/]+)', extended_enum_info.strip())
            collected_docs = ''
            for sub_match in sub_matches:
                var_name = sub_match.strip()
                var_to_mod = next((item for item in enum_to_mod['vars'] if item['name'] == var_name), dict())
                if var_to_mod:
                    if current_var_to_mod:
                        current_var_to_mod['docs'] = collected_docs
                    current_var_to_mod = var_to_mod
                    collected_docs = ''
                else:
                    collected_docs += '\\\n' + var_name
        if current_var_to_mod:
            current_var_to_mod['docs'] = collected_docs

for file in api_files:
    data = open(file, 'r').read()
    data = data.replace('\n', '')
    data = re.sub(r' ', '', data)
    m = re.search(r'open_libraries\s*\(([^\)]*)\)', data)
    if m:
        libs = m.group(1).split(',')
        for lib in libs:
            lualibs.append(lib.replace('sol::lib::', ''))

print('# Overlunky Lua API')
print('- Everything here is still changing, don\'t be sad if your scripts break next week!')
print('- This doc doesn\'t have a lot of examples, that\'s why we have [examples/](https://github.com/spelunky-fyi/overlunky/tree/main/examples).')
print('- This doc and the examples are written for a person who already knows [how to program in Lua](http://lua-users.org/wiki/TutorialDirectory).')
print('- This doc is up to date for the [WHIP build](https://github.com/spelunky-fyi/overlunky/releases/tag/whip). If you\'re using an official release from the past, you might find some things here don\'t work.')
print('- You can find changes to and earlier versions of this doc [here](https://github.com/spelunky-fyi/overlunky/commits/main/docs/script-api.md).')
print('- Click on the names of things to search for examples on how to use that function or variable.')

print('## Lua libraries')
print('The following Lua libraries and their functions are available. You can read more about them in the [Lua documentation](https://www.lua.org/manual/5.4/manual.html#6).')
for lib in lualibs:
    print('### `' + lib + '`')

print('## Unsafe mode')
print('Setting `meta.unsafe = true` enables the rest of the standard Lua libraries like `io` and `os`, loading dlls with require and `package.loadlib`. Using unsafe scripts requires users to enable the option in the overlunky.ini file which is found in the Spelunky 2 installation directory.')

print('## Modules')
print("""You can load modules with `require "mymod"` or `require "mydir.mymod"`, just put `mymod.lua` in the same directory the script is, or in `mydir/` to keep things organized.

Check the [Lua tutorial](http://lua-users.org/wiki/ModulesTutorial) or examples how to actually make modules.""")

print('## Global variables')
print("""These variables are always there to use.""")
for lf in funcs:
    if lf['name'] in ['players', 'state', 'savegame', 'options', 'meta']:
        print('### [`'+lf['name']+'`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q='+lf['name']+')')
        for com in lf['comment']:
            print(com)

print('## Event functions')
print("""Define these in your script to be called on an event. For example:
```lua
function on_level()
    toast("Welcome to the level")
end
```""")
for lf in events:
    if lf['name'].startswith('on_'):
        print('### [`'+lf['name']+'`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q='+lf['name']+')')
        for com in lf['comment']:
            print(com)

print('## Functions')
print('Note: The game functions like `spawn` use [level coordinates](#get_position). Draw functions use normalized [screen coordinates](#screen_position) from `-1.0 .. 1.0` where `0.0, 0.0` is the center of the screen.')
for lf in funcs:
    if len(rpcfunc(lf['cpp'])):
        for af in rpcfunc(lf['cpp']):
            print_af(lf, af)
    elif not (lf['name'].startswith('on_') or lf['name'] in ['players', 'state', 'savegame', 'options', 'meta']):
        m = re.search(r'\(([^\{]*)\)', lf['cpp'])
        param = ''
        if m:
            param = m.group(1)
            param = replace_all(param, replace)
        print('### [`'+lf['name']+'`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q='+lf['name']+')')
        if param: print('#### Params: `'+param+'`')
        for com in lf['comment']:
            print(('#### ' if com.startswith('Returns:') else '')+com)

print('## Types')
print('Using the api through these directly is kinda dangerous, but such is life. I got pretty bored writing this doc generator at this point, so you can find the variable types in the [source files](https://github.com/spelunky-fyi/overlunky/tree/main/src/game_api). They\'re mostly just ints and floats. Example:')
print("""```lua
-- This doesn't make any sense, as you could just access the variables directly from players[]
-- It's just a weird example OK!
ids = get_entities_by_mask(1) -- I think this just covers CHARs
for i,id in ipairs(ids) do
    e = get_entity(id):as_player() -- cast Entity to Player to access inventory
    e.health = 99
    e.inventory.bombs = 99
    e.inventory.ropes = 99
    e.type.jump = 0.36
end
```""")
for type in types:
    print('### `' + type['name'] + '`')
    if type['base']:
        print('Derived from', end='')
        bases = type['base'].split(',')
        for base in bases:
            print(' [`' + base + '`](#' + base.lower() + ')', end='')
        print()
    for var in type['vars']:
        search_link = 'https://github.com/spelunky-fyi/overlunky/search?l=Lua&q='+var['name']
        if 'signature' in var:
            print('- [`'+var['signature']+'`]('+search_link+')')
        else:
            print('- [`'+var['name']+'`]('+search_link+') '+var['type'].replace('<', '&lt;').replace('>', '&gt;'))

print('## Enums')
print('Enums are like numbers but in text that\'s easier to remember. Example:')
print("""```lua
set_callback(function()
    if state.theme == THEME.COSMIC_OCEAN then
        x, y, l = get_position(players[1].uid)
        spawn(ENT_TYPE.ITEM_JETPACK, x, y, l, 0, 0)
    end
end, ON.LEVEL)
```""")
for type in enums:
    print('### '+type['name'])
    for var in type['vars']:
        if var['name']:
            print('- [`'+var['name']+'`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q='+type['name']+'.'+var['name']+') ' + var['type'])
        else:
            print('- '+var['type'])
        if 'docs' in var:
            print(var['docs'])

