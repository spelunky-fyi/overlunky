meta.name = 'Entity Metatable Override'
meta.description = 'Demonstrate information transfer from one entity to another thru level transition using metadata. Using overlunky UI change the width and height of rock entity and bring it to the next level'
meta.author = 'Mr-Auto'

local metadata_table = {}
local metadata_next_uid = 1

set_callback(function()
    metadata_table = {}
    metadata_next_uid = 1
end, ON.START)

function get_new_medata_uid()
    local uid = metadata_next_uid
    metadata_next_uid = metadata_next_uid + 1
    -- matedata is stored as 16bit integer, we can't store bigger number
    -- this could be improved by lookup for an cleared id instead of just override whatever is there at index 1 and up
    -- but 65535 is a lot and should be enough for most use cases
    if metadata_next_uid > 65535 then
        metadata_next_uid = 1
    end
    return uid
end

local inner_call = false -- used to prevent infinite recursion

function get_metadata_override(ent)
    if not inner_call then
        inner_call = true
        local org_data = ent:get_metadata() -- call original to get the actual metadata
        inner_call = false

        -- for stuff like rock that doesn't use metadata we probably could just simplify all of this
        -- but this example is meant to work for any entity, also this supports multiple scripts with this code running at the same time
        local uid = get_new_medata_uid()
        metadata_table[uid] = {
            metadata = org_data,
            data = Vec2:new(ent.width, ent.height)
            -- can get most types of data from entity, also stuff from user_data
            -- exception for color, since game changes it to black when going thru layer door
        }
        return uid
    end
end

function apply_metadata_override(ent, uid)
    if uid ~= 0 then
        if metadata_table[uid] then
            if not inner_call then
                inner_call = true
                ent:apply_metadata(metadata_table[uid].metadata) -- call original to apply the actual metadata
                inner_call = false

                -- need to apply back all the data
                ent.width = metadata_table[uid].data.x
                ent.height = metadata_table[uid].data.y
                metadata_table[uid] = nil
                return true -- just to skip the execution of the game funcitonf
            end
        end
    end
end

set_post_entity_spawn(function(ent)
    -- called when level unloads, probably one of the last chances to get entity information
    ent:set_pre_get_metadata(get_metadata_override)
    -- called after spawning of the entity that was transferred thru level/wadler storage
    ent:set_pre_apply_metadata(apply_metadata_override)
end, SPAWN_TYPE.ANY, MASK.ANY, ENT_TYPE.ITEM_ROCK)
