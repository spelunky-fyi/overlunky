import re
import json
import urllib.request

# Parse all Entity::as_xxx functions so we know which ones are currently defined
entities_files = ["entity_lua.cpp",
                  "entities_items_lua.cpp",
                  "entities_chars_lua.cpp", 
                  "entities_monsters_lua.cpp", 
                  "entities_mounts_lua.cpp",
                  "entities_floors_lua.cpp",
                  "entities_activefloors_lua.cpp",
                  "entities_fx_lua.cpp",
                  "entities_liquids_lua.cpp",
                  "entities_backgrounds_lua.cpp",
                  "entities_decorations_lua.cpp",
                  "entities_logical_lua.cpp"]
as_re = re.compile(r'lua\["Entity"\]\["(as_.*)"\]')

known_casts = []

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
url = "https://raw.githubusercontent.com/spelunky-fyi/Spelunky2X64DbgPlugin/master/resources/Spelunky2Entities.json"
response = urllib.request.urlopen(url)
spelunky2json = response.read().decode('utf-8')
j = json.loads(re.sub("//.*", "", spelunky2json, flags=re.MULTILINE))
default_entity_types = j["default_entity_types"]
entity_class_hierarchy = j["entity_class_hierarchy"]

def is_movable(type):
    if type not in entity_class_hierarchy:
        return False

    if type == "Movable":
        return True

    parent_type = entity_class_hierarchy[type]
    if parent_type == "Movable":
        return True
    elif parent_type == "Entity":
        return False
    return is_movable(parent_type)

def first_base_class(type):
    if type in entity_class_hierarchy:
        return entity_class_hierarchy[type]

def linked_type_hierarchy(type, arr):
    arr.append("[" + type + "](https://spelunky-fyi.github.io/overlunky/#" + type + ")")
    if type in entity_class_hierarchy:
        parent_type = entity_class_hierarchy[type]
        linked_type_hierarchy(parent_type, arr)

# Process all entities
mapping = {}
derived_mapping = {}
hierarchy_doc_entries = []
with open("../../../../docs/game_data/entities.json") as fp:
    j = json.load(fp)
    for entityname, entitydetails in j.items():
        short_entityname = entityname.replace("ENT_TYPE_", "")
        doc_entry = "- " + f'{entitydetails["id"]:03}' + " " + short_entityname + " = "

        # match the entity name against all the regexes in default_entity_types
        regex_match_found = False
        for entityregex, entityclass in default_entity_types.items():
            if re.match("^" + entityregex + "$", short_entityname):
                regex_match_found = True
                as_function = "as_" + entityclass.lower()

                table_def = 'lua["TYPE_MAP"][' + str(entitydetails["id"]) + '] = lua["Entity"]["' + as_function + '"];  // ' + short_entityname
                movable_table_def = 'lua["TYPE_MAP"][' + str(entitydetails["id"]) + '] = lua["Entity"]["as_movable"];  // ' + short_entityname + " (NOT IMPLEMENTED YET, FORCED TO MOVABLE)"

                if as_function not in known_casts or as_function == "as_entity":
                    if as_function == "as_entity":
                        mapping[entitydetails["id"]] = "// " + table_def + " (plain entity)"
                        doc_entry = doc_entry + "[Entity](https://spelunky-fyi.github.io/overlunky/#Entity)"
                    elif is_movable(entityclass):
                        mapping[entitydetails["id"]] = movable_table_def
                        derived_mapping[entityclass] = "as_entity"
                        doc_entry = doc_entry + "[Entity](https://spelunky-fyi.github.io/overlunky/#Entity) > [Movable](https://spelunky-fyi.github.io/overlunky/#Movable) - NOT IMPLEMENTED YET, FORCED TO MOVABLE"
                    else:
                        mapping[entitydetails["id"]] = "// " + table_def + " (NOT IMPLEMENTED YET)"
                        doc_entry = doc_entry + "[Entity](https://spelunky-fyi.github.io/overlunky/#Entity) - NOT IMPLEMENTED YET"
                else:
                    mapping[entitydetails["id"]] = table_def

                    base_class = first_base_class(entityclass)
                    if base_class:
                        as_base_function = "as_" + base_class.lower()
                        derived_mapping[entityclass] = as_base_function

                    hierarchy = []
                    linked_type_hierarchy(entityclass, hierarchy)
                    hierarchy.reverse()
                    doc_entry = doc_entry + " > ".join(hierarchy)

                break
        if not regex_match_found:
            mapping[entitydetails["id"]] = '// lua["TYPE_MAP"][' + str(entitydetails["id"]) + '] = lua["Entity"]["?"]; // ' + short_entityname + ' (UNKNOWN IN PLUGIN)'
            doc_entry = doc_entry + "[Entity](https://spelunky-fyi.github.io/overlunky/#Entity) - UNKNOWN IN PLUGIN"
        
        hierarchy_doc_entries.append(doc_entry)

            

sorted_mapping = {k: mapping[k] for k in sorted(mapping)}
for k in sorted_mapping:
    print(sorted_mapping[k])

for k in derived_mapping:
    print(f'{{"{k}", "{derived_mapping[k]}"}},')

# Write the hierarchy documentation
hierarchy_doc_entries.sort()
with open("../../../../docs/entities-hierarchy.md", 'w') as doc_file:
    doc_file.write("# Entity hierarchy list\n")
    doc_file.write("The list below shows the type hierarchy for each entity in the game. This makes it easy to figure out which variables and functions are available for a specific entity.\n")
    for entry in hierarchy_doc_entries:
        doc_file.write(entry + "\n")
