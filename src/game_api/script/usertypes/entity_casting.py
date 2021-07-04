import re
import json
import urllib.request

# Parse all Entity::as_xxx functions so we know which ones are currently defined
entities_files = ["entities_items_lua.cpp",
                  "entities_monsters_lua.cpp", 
                  "entities_mounts_lua.cpp"]
as_re = re.compile(r'lua\["Entity"\]\["(as_.*)"\]')

# a couple as_xxx functions are predefined, they are either 'global' or haven't been moved into their proper separate file yet
known_casts = [ "as_movable", "as_door", "as_player", "as_crushtrap", "as_arrowtrap", "as_olmec", "as_olmec_floater" ]

for f in entities_files:
    with open(f) as fp:
        line = fp.readline()
        while line:
            m = as_re.search(line)
            if m != None:
                known_casts.append(m.group(1))
            line = fp.readline()

# Download Spelunky2.json from the x64dbg github repo
#url = "https://gitcdn.link/repo/spelunky-fyi/Spelunky2X64DbgPlugin/master/resources/Spelunky2.json"
url = "https://raw.githubusercontent.com/spelunky-fyi/Spelunky2X64DbgPlugin/master/resources/Spelunky2.json"
response = urllib.request.urlopen(url)
spelunky2json = response.read().decode('utf-8')
j = json.loads(re.sub("//.*", "", spelunky2json, flags=re.MULTILINE))
default_entity_types = j["default_entity_types"]

# Process all entities
mapping = {}
with open("../../../../docs/game_data/entities.json") as fp:
    j = json.load(fp)
    for entityname, entitydetails in j.items():
        short_entityname = entityname.replace("ENT_TYPE_", "")
        # match the entity name against all the regexes in default_entity_types
        for entityregex, entityclass in default_entity_types.items():
            if re.match("^" + entityregex + "$", short_entityname):
                as_function = "as_" + entityclass.lower()
                table_def = 'lua["TYPE_MAP"][' + str(entitydetails["id"]) + '] = "' + as_function + '";  // '  + short_entityname

                if as_function not in known_casts:
                    table_def = "// " + table_def + " (NOT IMPLEMENTED YET)"

                mapping[entitydetails["id"]] = table_def
                break

sorted_mapping = {k: mapping[k] for k in sorted(mapping)}
for k in sorted_mapping:
    print(sorted_mapping[k])
