#include "level_lua.hpp"

#include "level_api.hpp"
#include "rpc.hpp"
#include "savedata.hpp"
#include "script/lua_backend.hpp"
#include "state.hpp"

#include <sol/sol.hpp>

void PreLoadLevelFilesContext::override_level_files(std::vector<std::string> levels)
{
    override_next_levels(std::move(levels));
}
void PreLoadLevelFilesContext::add_level_files(std::vector<std::string> levels)
{
    add_next_levels(std::move(levels));
}

bool PostRoomGenerationContext::set_room_template(uint32_t x, uint32_t y, LAYER layer, ROOM_TEMPLATE room_template)
{
    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    return State::get().ptr_local()->level_gen->set_room_template(x, y, real_layer, room_template);
}
bool PostRoomGenerationContext::mark_as_machine_room_origin(uint32_t x, uint32_t y, LAYER layer)
{
    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    return State::get().ptr_local()->level_gen->mark_as_machine_room_origin(x, y, real_layer);
}
bool PostRoomGenerationContext::mark_as_set_room(uint32_t x, uint32_t y, LAYER layer)
{
    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    return State::get().ptr_local()->level_gen->mark_as_set_room(x, y, real_layer, true);
}
bool PostRoomGenerationContext::unmark_as_set_room(uint32_t x, uint32_t y, LAYER layer)
{
    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    return State::get().ptr_local()->level_gen->mark_as_set_room(x, y, real_layer, false);
}

bool PostRoomGenerationContext::set_shop_type(uint32_t x, uint32_t y, LAYER layer, int32_t shop_type)
{
    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    return State::get().ptr_local()->level_gen->set_shop_type(x, y, real_layer, static_cast<ShopType>(shop_type));
}

bool PostRoomGenerationContext::set_procedural_spawn_chance(PROCEDURAL_CHANCE chance_id, uint32_t inverse_chance)
{
    return State::get().ptr_local()->level_gen->set_procedural_spawn_chance(chance_id, inverse_chance);
}

void PostRoomGenerationContext::set_num_extra_spawns(std::uint32_t extra_spawn_id, std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer)
{
    State::get().ptr_local()->level_gen->data->set_num_extra_spawns(extra_spawn_id, num_spawns_front_layer, num_spawns_back_layer);
}

std::optional<SHORT_TILE_CODE> PostRoomGenerationContext::define_short_tile_code(ShortTileCodeDef short_tile_code_def)
{
    return State::get().ptr_local()->level_gen->data->define_short_tile_code(short_tile_code_def);
}
void PostRoomGenerationContext::change_short_tile_code(SHORT_TILE_CODE short_tile_code, ShortTileCodeDef short_tile_code_def)
{
    State::get().ptr_local()->level_gen->data->change_short_tile_code(short_tile_code, short_tile_code_def);
}

std::optional<SHORT_TILE_CODE> PreHandleRoomTilesContext::get_short_tile_code(uint32_t tx, uint32_t ty, LAYER layer) const
{
    if (tx >= 0 && tx < 10 && ty >= 0 && ty < 8)
    {
        const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
        if (real_layer == static_cast<uint8_t>(LAYER::FRONT))
        {
            return get_room_data().front_layer[ty][tx];
        }
        else if (has_back_layer())
        {
            return get_room_data().back_layer.value()[ty][tx];
        }
    }
    return std::nullopt;
}
bool PreHandleRoomTilesContext::set_short_tile_code(uint32_t tx, uint32_t ty, LAYER layer, SHORT_TILE_CODE short_tile_code)
{
    if (tx >= 0 && tx < 10 && ty >= 0 && ty < 8)
    {
        if (layer == LAYER::BOTH)
        {
            set_short_tile_code(tx, ty, LAYER::BACK, short_tile_code);
            layer = LAYER::FRONT;
        }

        const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
        if (real_layer == static_cast<uint8_t>(LAYER::FRONT))
        {
            get_mutable_room_data().front_layer[ty][tx] = short_tile_code;
            return true;
        }
        else if (has_back_layer())
        {
            get_mutable_room_data().back_layer.value()[ty][tx] = short_tile_code;
            return true;
        }
    }
    return false;
}
std::vector<std::tuple<uint32_t, uint32_t, LAYER>> PreHandleRoomTilesContext::find_all_short_tile_codes(LAYER layer, SHORT_TILE_CODE short_tile_code)
{
    if (layer == LAYER::BOTH)
    {
        std::vector<std::tuple<uint32_t, uint32_t, LAYER>> positions = find_all_short_tile_codes(LAYER::FRONT, short_tile_code);
        std::vector<std::tuple<uint32_t, uint32_t, LAYER>> positions_back = find_all_short_tile_codes(LAYER::BACK, short_tile_code);
        positions.insert(positions.end(), positions_back.begin(), positions_back.end());
        return positions_back;
    }

    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    const bool front_layer = real_layer == static_cast<uint8_t>(LAYER::FRONT);
    if (front_layer || has_back_layer())
    {
        const LevelGenRoomData& data = get_room_data();
        const SingleRoomData& mutable_room_data = front_layer ? data.front_layer : data.back_layer.value();

        std::vector<std::tuple<uint32_t, uint32_t, LAYER>> positions;
        for (uint32_t tx = 0; tx < 10; tx++)
        {
            for (uint32_t ty = 0; ty < 8; ty++)
            {
                if (mutable_room_data[ty][tx] == short_tile_code)
                {
                    positions.push_back({tx, ty, layer});
                }
            }
        }
        return positions;
    }
    return {};
}
bool PreHandleRoomTilesContext::replace_short_tile_code(LAYER layer, SHORT_TILE_CODE short_tile_code, SHORT_TILE_CODE replacement_short_tile_code)
{
    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    const bool front_layer = real_layer == static_cast<uint8_t>(LAYER::FRONT);
    if (front_layer || has_back_layer())
    {
        LevelGenRoomData& mutable_data = get_mutable_room_data();
        SingleRoomData& mutable_room_data = front_layer ? mutable_data.front_layer : mutable_data.back_layer.value();
        for (uint32_t tx = 0; tx < 10; tx++)
        {
            for (uint32_t ty = 0; ty < 8; ty++)
            {
                if (mutable_room_data[ty][tx] == short_tile_code)
                {
                    mutable_room_data[ty][tx] = replacement_short_tile_code;
                }
            }
        }
        return true;
    }
    return false;
}
bool PreHandleRoomTilesContext::has_back_layer() const
{
    return get_room_data().back_layer.has_value();
}
void PreHandleRoomTilesContext::add_empty_back_layer()
{
    if (!has_back_layer())
    {
        auto& data = get_mutable_room_data();
        data.back_layer.emplace();
        std::memset(data.back_layer.value().data(), '0', sizeof(SingleRoomData));
    }
}
void PreHandleRoomTilesContext::add_copied_back_layer()
{
    if (!has_back_layer())
    {
        auto& data = get_mutable_room_data();
        data.back_layer.emplace();
        std::memcpy(data.back_layer.value().data(), data.front_layer.data(), sizeof(SingleRoomData));
    }
}

const LevelGenRoomData& PreHandleRoomTilesContext::get_room_data() const
{
    return modded_room_data.has_value()
               ? modded_room_data.value()
               : room_data;
}
LevelGenRoomData& PreHandleRoomTilesContext::get_mutable_room_data()
{
    if (!modded_room_data.has_value())
    {
        modded_room_data = room_data;
    }
    return modded_room_data.value();
}

struct ThemeOverride
{
    bool enabled;
    uint8_t theme;
    sol::optional<sol::function> func;
    sol::optional<sol::function> pre;
    sol::optional<sol::function> post;
};

class CustomTheme : public ThemeInfo
{
  public:
    std::string level_file;
    uint8_t theme;
    uint8_t base_theme;
    LuaBackend* backend;

    std::map<THEME_OVERRIDE, std::unique_ptr<ThemeOverride>> overrides;
    std::map<DYNAMIC_TEXTURE, uint32_t> textures;

    void override(THEME_OVERRIDE index, bool enabled_)
    {
        if (overrides.find(index) == overrides.end())
            overrides[index] = std::unique_ptr<ThemeOverride>{new ThemeOverride{enabled_, UINT8_MAX}};
        else
            overrides[index]->enabled = enabled_;
    }

    void override(THEME_OVERRIDE index, uint8_t theme_)
    {
        if (overrides.find(index) == overrides.end())
            overrides[index] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, theme_}};
        else
            overrides[index]->theme = theme_;
    }

    void override(THEME_OVERRIDE index, sol::function func_)
    {
        if (overrides.find(index) == overrides.end())
            overrides[index] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX, std::move(func_)}};
        else
        {
            overrides[index]->func = std::move(func_);
        }
    }

    void pre(THEME_OVERRIDE index, sol::function func_)
    {
        if (overrides.find(index) == overrides.end())
            overrides[index] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX, std::nullopt, std::move(func_)}};
        else
        {
            overrides[index]->pre = std::move(func_);
        }
    }

    void post(THEME_OVERRIDE index, sol::function func_)
    {
        if (overrides.find(index) == overrides.end())
            overrides[index] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX, std::nullopt, std::nullopt, std::move(func_)}};
        else
        {
            overrides[index]->post = std::move(func_);
        }
    }

    uint8_t get_override_theme(THEME_OVERRIDE index)
    {
        if (overrides.find(index) != overrides.end() && overrides[index]->theme > 0 && overrides[index]->theme < 18)
            return overrides[index]->theme - 1;
        else if (overrides.find(THEME_OVERRIDE::BASE) != overrides.end() && overrides[THEME_OVERRIDE::BASE]->theme > 0 && overrides[THEME_OVERRIDE::BASE]->theme < 18)
            return overrides[THEME_OVERRIDE::BASE]->theme - 1;
        return get_base_id();
    }

    uint8_t get_override_theme_or_dwelling(THEME_OVERRIDE index)
    {
        if (overrides.find(index) != overrides.end() && overrides[index]->theme > 0 && overrides[index]->theme < 18)
            return overrides[index]->theme - 1;
        else if (overrides.find(THEME_OVERRIDE::BASE) != overrides.end() && overrides[THEME_OVERRIDE::BASE]->theme > 0 && overrides[THEME_OVERRIDE::BASE]->theme < 18)
            return overrides[THEME_OVERRIDE::BASE]->theme - 1;
        return 0;
    }

    bool get_override_enabled(THEME_OVERRIDE index)
    {
        if (overrides.find(index) != overrides.end())
        {
            return overrides[index]->enabled;
        }
        return false;
    }

    bool get_override_func_enabled(THEME_OVERRIDE index)
    {
        if (overrides.find(index) != overrides.end())
        {
            return overrides[index]->func.has_value();
        }
        return false;
    }

    bool get_pre_func_enabled(THEME_OVERRIDE index)
    {
        if (overrides.find(index) != overrides.end())
        {
            return overrides[index]->pre.has_value();
        }
        return false;
    }

    bool get_post_func_enabled(THEME_OVERRIDE index)
    {
        if (overrides.find(index) != overrides.end())
        {
            return overrides[index]->post.has_value();
        }
        return false;
    }

    template <class Ret, class... Args>
    std::optional<Ret> run_override_func(THEME_OVERRIDE index, Args&&... args)
    {
        if (overrides.find(index) != overrides.end() && get_override_func_enabled(index))
        {
            return backend->handle_function_with_return<Ret>(overrides[index]->func.value(), std::forward<Args>(args)...);
        }
        return std::nullopt;
    }

    template <class Ret, class... Args>
    std::optional<Ret> run_pre_func(THEME_OVERRIDE index, Args&&... args)
    {
        if (overrides.find(index) != overrides.end() && get_pre_func_enabled(index))
        {
            return backend->handle_function_with_return<Ret>(overrides[index]->pre.value(), std::forward<Args>(args)...);
        }
        return std::nullopt;
    }

    template <class Ret, class... Args>
    std::optional<Ret> run_post_func(THEME_OVERRIDE index, Args&&... args)
    {
        if (overrides.find(index) != overrides.end() && get_post_func_enabled(index))
        {
            return backend->handle_function_with_return<Ret>(overrides[index]->post.value(), std::forward<Args>(args)...);
        }
        return std::nullopt;
    }

    CustomTheme(uint8_t theme_id_, uint8_t base_theme_, bool defaults)
    {
        level_file = "";
        theme = theme_id_;
        base_theme = base_theme_ - 1;
        backend = LuaBackend::get_calling_backend();
        if (defaults)
        {
            overrides[THEME_OVERRIDE::INIT_FLAGS] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::SPAWN_LEVEL] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::SPAWN_PLAYERS] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, 0}};
            overrides[THEME_OVERRIDE::SPAWN_TRANSITION] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, 0}};
            overrides[THEME_OVERRIDE::POST_TRANSITION] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, 0}};
            overrides[THEME_OVERRIDE::SPAWN_PROCEDURAL] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::SPAWN_BACKGROUND] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::SPAWN_BORDER] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, 0}};
            overrides[THEME_OVERRIDE::ENT_BORDER] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::TEXTURE_DYNAMIC] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::PLAYER_DAMAGE] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::GRAVITY] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            /*
            overrides[THEME_OVERRIDE::INIT_LEVEL] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::INIT_FLAGS] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::SPAWN_EFFECTS] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::TRANSITION_MODIFIER] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::UNKNOWN_V32] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::UNKNOWN_V38] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::UNKNOWN_V47] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::SPAWN_DECORATION] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            overrides[THEME_OVERRIDE::SPAWN_DECORATION2] = std::unique_ptr<ThemeOverride>{new ThemeOverride{true, UINT8_MAX}};
            */
        }
    }
    CustomTheme(uint8_t theme_id_, uint8_t base_theme_)
        : CustomTheme(theme_id_, base_theme_, true)
    {
    }
    CustomTheme()
        : CustomTheme(100, 1, true)
    {
    }
    ~CustomTheme()
    {
        overrides.clear();
    }
    bool get_unknown1()
    {
        return unknown1;
    }
    void init_flags()
    {
        auto index = THEME_OVERRIDE::INIT_FLAGS;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->init_flags();
        run_post_func<std::monostate>(index);
    }
    void init_level()
    {
        auto index = THEME_OVERRIDE::INIT_LEVEL;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->init_level();
        run_post_func<std::monostate>(index);
    }
    void unknown_v4()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V4;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v4();
        run_post_func<std::monostate>(index);
    }
    void unknown_v5()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V5;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v5();
        run_post_func<std::monostate>(index);
    }
    void add_special_rooms()
    {
        auto index = THEME_OVERRIDE::SPECIAL_ROOMS;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->add_special_rooms();
        run_post_func<std::monostate>(index);
    }
    void unknown_v7()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V7;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v7();
        run_post_func<std::monostate>(index);
    }
    void unknown_v8()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V8;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v8();
        run_post_func<std::monostate>(index);
    }
    void add_vault()
    {
        auto index = THEME_OVERRIDE::VAULT;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->add_vault();
        run_post_func<std::monostate>(index);
    }
    void add_coffin()
    {
        auto index = THEME_OVERRIDE::COFFIN;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->add_coffin();
        run_post_func<std::monostate>(index);
    }
    void add_special_feeling()
    {
        auto index = THEME_OVERRIDE::FEELING;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->add_special_feeling();
        run_post_func<std::monostate>(index);
    }
    bool unknown_v12()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V12;
        bool ret = false;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<bool>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v12();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    void spawn_level()
    {
        auto index = THEME_OVERRIDE::SPAWN_LEVEL;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme_or_dwelling(index)]->spawn_level();
        run_post_func<std::monostate>(index);
    }
    void spawn_border()
    {
        auto index = THEME_OVERRIDE::SPAWN_BORDER;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme_or_dwelling(index)]->spawn_border();
        run_post_func<std::monostate>(index);
    }
    void post_process_level()
    {
        auto index = THEME_OVERRIDE::POST_PROCESS_LEVEL;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->post_process_level();
        run_post_func<std::monostate>(index);
    }
    void spawn_traps()
    {
        auto index = THEME_OVERRIDE::SPAWN_TRAPS;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_traps();
        run_post_func<std::monostate>(index);
    }
    void post_process_entities()
    {
        auto index = THEME_OVERRIDE::POST_PROCESS_ENTITIES;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->post_process_entities();
        run_post_func<std::monostate>(index);
    }
    void spawn_procedural()
    {
        auto index = THEME_OVERRIDE::SPAWN_PROCEDURAL;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_procedural();
        run_post_func<std::monostate>(index);
    }
    void spawn_background()
    {
        auto index = THEME_OVERRIDE::SPAWN_BACKGROUND;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_background();
        run_post_func<std::monostate>(index);
    }
    void spawn_lights()
    {
        auto index = THEME_OVERRIDE::SPAWN_LIGHTS;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_lights();
        run_post_func<std::monostate>(index);
    }
    void spawn_transition()
    {
        auto index = THEME_OVERRIDE::SPAWN_TRANSITION;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_transition();
        run_post_func<std::monostate>(index);
    }
    void post_transition()
    {
        auto index = THEME_OVERRIDE::POST_TRANSITION;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme_or_dwelling(index)]->post_transition();
        run_post_func<std::monostate>(index);
    }
    void spawn_players()
    {
        auto index = THEME_OVERRIDE::SPAWN_PLAYERS;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme_or_dwelling(index)]->spawn_players();
        run_post_func<std::monostate>(index);
    }
    void spawn_effects()
    {
        auto index = THEME_OVERRIDE::SPAWN_EFFECTS;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_effects();
        else
        {
            // set sane camera bounds anyway for your convenience
            // you can always change this in post
            auto state = State::get().ptr();
            state->camera->bounds_left = 0.5f;
            state->camera->bounds_top = 124.5f;
            state->camera->bounds_right = 10.0f * state->w + 4.5f;
            state->camera->bounds_bottom = 120.5f - 8.0f * state->h;
        }
        run_post_func<std::monostate>(index);
    }
    const char* get_level_file()
    {
        return level_file.c_str();
    }
    uint8_t get_theme_id()
    {
        auto index = THEME_OVERRIDE::THEME_ID;
        uint8_t ret = theme;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint8_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_theme_id();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint8_t get_base_id()
    {
        auto index = THEME_OVERRIDE::BASE_ID;
        uint8_t ret = base_theme;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint8_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_base_id();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t get_floor_spreading_type()
    {
        auto index = THEME_OVERRIDE::ENT_FLOOR_SPREADING;
        uint32_t ret = to_id("ENT_TYPE_FX_SHADOW");
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_floor_spreading_type();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t get_floor_spreading_type2()
    {
        auto index = THEME_OVERRIDE::ENT_FLOOR_SPREADING2;
        uint32_t ret = to_id("ENT_TYPE_FX_SHADOW");
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_floor_spreading_type2();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    bool unknown_v30()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V30;
        bool ret = false;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<bool>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v30();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t get_transition_block_modifier()
    {
        auto index = THEME_OVERRIDE::TRANSITION_MODIFIER;
        uint32_t ret = 85; // TODO: don't know what this is
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_floor_spreading_type2();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t unknown_v32()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V32;
        uint32_t ret = 12; // TODO: don't know what this is
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v32();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t get_backwall_type()
    {
        auto index = THEME_OVERRIDE::ENT_BACKWALL;
        uint32_t ret = to_id("ENT_TYPE_BG_LEVEL_BACKWALL");
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_backwall_type();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t get_border_type()
    {
        auto index = THEME_OVERRIDE::ENT_BORDER;
        uint32_t ret = to_id("ENT_TYPE_FLOOR_BORDERTILE");
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_border_type();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t get_critter_type()
    {
        auto index = THEME_OVERRIDE::ENT_CRITTER;
        uint32_t ret = to_id("ENT_TYPE_FX_SHADOW");
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_critter_type();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    float get_liquid_gravity()
    {
        auto index = THEME_OVERRIDE::GRAVITY;
        float ret = 0.0f;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<float>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_liquid_gravity();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    bool get_player_damage()
    {
        auto index = THEME_OVERRIDE::PLAYER_DAMAGE;
        bool ret = false;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<bool>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_player_damage();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    bool unknown_v38()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V38;
        bool ret = true;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<bool>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v38();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t get_backlayer_lut()
    {
        auto index = THEME_OVERRIDE::TEXTURE_BACKLAYER_LUT;
        uint32_t ret = 395; // default lut texture id
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_backlayer_lut();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    float get_backlayer_light_level()
    {
        auto index = THEME_OVERRIDE::BACKLAYER_LIGHT_LEVEL;
        float ret = 0.0f;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<float>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_backlayer_light_level();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    bool get_loop()
    {
        auto index = THEME_OVERRIDE::LOOP;
        bool ret = false;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<bool>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_loop();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint8_t get_vault_level()
    {
        auto index = THEME_OVERRIDE::VAULT_LEVEL;
        uint8_t ret = 255;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint8_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_vault_level();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    bool get_unknown_1_or_2(uint8_t index)
    {
        if (index == 0)
            return unknown1;
        return unknown2;
    }
    uint32_t get_dynamic_texture(int32_t texture_id)
    {
        auto index = THEME_OVERRIDE::TEXTURE_DYNAMIC;
        uint32_t ret = 0;
        run_pre_func<std::monostate>(index, texture_id);
        if (textures.find((DYNAMIC_TEXTURE)texture_id) != textures.end())
        {
            ret = textures[(DYNAMIC_TEXTURE)texture_id];
        }
        else if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index, texture_id).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_dynamic_texture(texture_id);
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    void pre_transition()
    {
        auto index = THEME_OVERRIDE::PRE_TRANSITION;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme_or_dwelling(index)]->pre_transition();
        run_post_func<std::monostate>(index);
    }
    uint32_t get_level_height()
    {
        auto index = THEME_OVERRIDE::LEVEL_HEIGHT;
        uint32_t ret = State::get().ptr_local()->h - 1;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->get_level_height();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    uint32_t unknown_v47()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V47;
        uint32_t ret = 0;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            ret = run_override_func<uint32_t>(index).value_or(ret);
        else if (get_override_enabled(index))
            ret = State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v47();
        run_post_func<std::monostate>(index, ret);
        return ret;
    }
    void spawn_decoration()
    {
        auto index = THEME_OVERRIDE::SPAWN_DECORATION;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_decoration();
        run_post_func<std::monostate>(index);
    }
    void spawn_decoration2()
    {
        auto index = THEME_OVERRIDE::SPAWN_DECORATION2;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_decoration2();
        run_post_func<std::monostate>(index);
    }
    void spawn_extra()
    {
        auto index = THEME_OVERRIDE::SPAWN_EXTRA;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->spawn_extra();
        run_post_func<std::monostate>(index);
    }
    void unknown_v51()
    {
        auto index = THEME_OVERRIDE::UNKNOWN_V51;
        run_pre_func<std::monostate>(index);
        if (get_override_func_enabled(index))
            run_override_func<std::monostate>(index);
        else if (get_override_enabled(index))
            State::get().ptr_local()->level_gen->themes[get_override_theme(index)]->unknown_v51();
        run_post_func<std::monostate>(index);
    }
};

namespace NLevel
{
void register_usertypes(sol::state& lua)
{
    /// Default function in spawn definitions to check whether a spawn is valid or not
    lua["default_spawn_is_valid"] = default_spawn_is_valid;

    /// Add a callback for a specific tile code that is called before the game handles the tile code.
    /// The callback signature is `bool pre_tile_code(x, y, layer, room_template)`
    /// Return true in order to stop the game or scripts loaded after this script from handling this tile code.
    /// For example, when returning true in this callback set for `"floor"` then no floor will spawn in the game (unless you spawn it yourself)
    lua["set_pre_tile_code_callback"] = [](sol::function cb, std::string tile_code) -> CallbackId
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        backend->pre_tile_code_callbacks.push_back(LevelGenCallback{backend->cbcount, std::move(tile_code), std::move(cb)});
        return backend->cbcount++;
    };
    /// Add a callback for a specific tile code that is called after the game handles the tile code.
    /// The callback signature is `nil post_tile_code(x, y, layer, room_template)`
    /// Use this to affect what the game or other scripts spawned in this position.
    /// This is received even if a previous pre-tile-code-callback has returned true
    lua["set_post_tile_code_callback"] = [](sol::function cb, std::string tile_code) -> CallbackId
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        backend->post_tile_code_callbacks.push_back(LevelGenCallback{backend->cbcount, std::move(tile_code), std::move(cb)});
        return backend->cbcount++;
    };
    /// Define a new tile code, to make this tile code do anything you have to use either `set_pre_tile_code_callback` or `set_post_tile_code_callback`.
    /// If a user disables your script but still uses your level mod nothing will be spawned in place of your tile code.
    lua["define_tile_code"] = [](std::string tile_code) -> TILE_CODE
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return backend->g_state->level_gen->data->define_tile_code(std::move(tile_code));
    };

    /// Gets a short tile code based on definition, returns `nil` if it can't be found
    lua["get_short_tile_code"] = [](ShortTileCodeDef short_tile_code_def) -> std::optional<uint8_t>
    {
        return State::get().ptr_local()->level_gen->data->get_short_tile_code(short_tile_code_def);
    };
    /// Gets the definition of a short tile code (if available), will vary depending on which file is loaded
    lua["get_short_tile_code_definition"] = [](SHORT_TILE_CODE short_tile_code) -> std::optional<ShortTileCodeDef>
    {
        return State::get().ptr_local()->level_gen->data->get_short_tile_code_def(short_tile_code);
    };

    /// Define a new procedural spawn, the function `nil do_spawn(x, y, layer)` contains your code to spawn the thing, whatever it is.
    /// The function `bool is_valid(x, y, layer)` determines whether the spawn is legal in the given position and layer.
    /// Use for example when you can spawn only on the ceiling, under water or inside a shop.
    /// Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
    /// If a user disables your script but still uses your level mod nothing will be spawned in place of your procedural spawn.
    lua["define_procedural_spawn"] = [](std::string procedural_spawn, sol::function do_spawn, sol::function is_valid) -> PROCEDURAL_CHANCE
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        LevelGenData* data = backend->g_state->level_gen->data;
        uint32_t chance = data->define_chance(std::move(procedural_spawn));
        std::function<bool(float, float, int)> is_valid_call{nullptr};
        if (is_valid)
        {
            is_valid_call = [backend, is_valid_lua = std::move(is_valid)](float x, float y, int layer)
            {
                std::lock_guard lock{backend->gil};
                return backend->handle_function_with_return<bool>(is_valid_lua, x, y, layer).value_or(false);
            };
        }
        std::function<void(float, float, int)> do_spawn_call = [backend, do_spawn_lua = std::move(do_spawn)](float x, float y, int layer)
        {
            std::lock_guard lock{backend->gil};
            return backend->handle_function_with_return<bool>(do_spawn_lua, x, y, layer).value_or(false);
        };
        std::uint32_t id = data->register_chance_logic_provider(chance, SpawnLogicProvider{std::move(is_valid_call), std::move(do_spawn_call)});
        backend->chance_callbacks.push_back(id);
        return chance;
    };

    /// Define a new extra spawn, these are semi-guaranteed level gen spawns with a fixed upper bound.
    /// The function `nil do_spawn(x, y, layer)` contains your code to spawn the thing, whatever it is.
    /// The function `bool is_valid(x, y, layer)` determines whether the spawn is legal in the given position and layer.
    /// Use for example when you can spawn only on the ceiling, under water or inside a shop.
    /// Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
    /// To change the number of spawns use `PostRoomGenerationContext::set_num_extra_spawns` during `ON.POST_ROOM_GENERATION`
    /// No name is attached to the extra spawn since it is not modified from level files, instead every call to this function will return a new uniqe id.
    lua["define_extra_spawn"] = [](sol::function do_spawn, sol::function is_valid, std::uint32_t num_spawns_frontlayer, std::uint32_t num_spawns_backlayer) -> std::uint32_t
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        std::function<bool(float, float, int)> is_valid_call{nullptr};
        if (is_valid)
        {
            is_valid_call = [backend, is_valid_lua = std::move(is_valid)](float x, float y, int layer)
            {
                std::lock_guard lock{backend->gil};
                return backend->handle_function_with_return<bool>(is_valid_lua, x, y, layer).value_or(false);
            };
        }
        std::function<void(float, float, int)> do_spawn_call = [backend, do_spawn_lua = std::move(do_spawn)](float x, float y, int layer)
        {
            std::lock_guard lock{backend->gil};
            return backend->handle_function_with_return<bool>(do_spawn_lua, x, y, layer).value_or(false);
        };
        LevelGenData* data = backend->g_state->level_gen->data;
        std::uint32_t extra_spawn_id = data->define_extra_spawn(num_spawns_frontlayer, num_spawns_backlayer, SpawnLogicProvider{std::move(is_valid_call), std::move(do_spawn_call)});
        backend->extra_spawn_callbacks.push_back(extra_spawn_id);
        return extra_spawn_id;
    };
    /// Use to query whether any of the requested spawns could not be made, usually because there were not enough valid spaces in the level.
    /// Returns missing spawns in the front layer and missing spawns in the back layer in that order.
    /// The value only makes sense after level generation is complete, aka after `ON.POST_LEVEL_GENERATION` has run.
    lua["get_missing_extra_spawns"] = [](std::uint32_t extra_spawn_chance_id) -> std::pair<std::uint32_t, std::uint32_t>
    {
        return State::get().ptr()->level_gen->data->get_missing_extra_spawns(extra_spawn_chance_id);
    };

    /// Transform a position to a room index to be used in `get_room_template` and `PostRoomGenerationContext.set_room_template`
    lua["get_room_index"] = [](float x, float y) -> std::pair<int, int>
    {
        return State::get().ptr_local()->level_gen->get_room_index(x, y);
    };
    /// Transform a room index into the top left corner position in the room
    lua["get_room_pos"] = [](int x, int y) -> std::pair<float, float>
    {
        return State::get().ptr_local()->level_gen->get_room_pos(x, y);
    };
    /// Get the room template given a certain index, returns `nil` if coordinates are out of bounds
    lua["get_room_template"] = [](int x, int y, LAYER layer) -> std::optional<uint16_t>
    {
        const uint8_t real_layer = enum_to_layer(layer);
        return State::get().ptr_local()->level_gen->get_room_template(x, y, real_layer);
    };
    /// Get whether a room is flipped at the given index, returns `false` if coordinates are out of bounds
    lua["is_room_flipped"] = [](int x, int y) -> bool
    {
        return State::get().ptr_local()->level_gen->is_room_flipped(x, y);
    };
    /// For debugging only, get the name of a room template, returns `'invalid'` if room template is not defined
    lua["get_room_template_name"] = [](int16_t room_template) -> std::string_view
    {
        return State::get().ptr_local()->level_gen->get_room_template_name(room_template);
    };

    /// Define a new room remplate to use with `set_room_template`
    lua["define_room_template"] = [](std::string room_template, ROOM_TEMPLATE_TYPE type) -> uint16_t
    {
        return State::get().ptr_local()->level_gen->data->define_room_template(std::move(room_template), static_cast<RoomTemplateType>(type));
    };
    /// Set the size of room template in tiles, the template must be of type `ROOM_TEMPLATE_TYPE.MACHINE_ROOM`.
    lua["set_room_template_size"] = [](uint16_t room_template, uint16_t width, uint16_t height) -> bool
    {
        return State::get().ptr_local()->level_gen->data->set_room_template_size(room_template, width, height);
    };

    /// Get the inverse chance of a procedural spawn for the current level.
    /// A return value of 0 does not mean the chance is infinite, it means the chance is zero.
    lua["get_procedural_spawn_chance"] = [](PROCEDURAL_CHANCE chance_id) -> uint32_t
    {
        return State::get().ptr_local()->level_gen->get_procedural_spawn_chance(chance_id);
    };

    /// Gets the sub theme of the current cosmic ocean level, returns `COSUBTHEME.NONE` if the current level is not a CO level.
    lua["get_co_subtheme"] = get_co_subtheme;
    /// Forces the theme of the next cosmic ocean level(s) (use e.g. `force_co_subtheme(COSUBTHEME.JUNGLE)`. Use `COSUBTHEME.RESET` to reset to default random behaviour)
    lua["force_co_subtheme"] = force_co_subtheme;

    /// Gets the value for the specified config
    lua["get_level_config"] = [](LEVEL_CONFIG config) -> uint32_t
    {
        return State::get().ptr_local()->level_gen->data->level_config[config];
    };

    lua.new_usertype<ThemeInfo>(
        "ThemeInfo",
        "sub_theme",
        &ThemeInfo::sub_theme,
        "get_unknown1",
        &ThemeInfo::get_unknown1,
        "init_flags",
        &ThemeInfo::init_flags,
        "init_level",
        &ThemeInfo::init_level,
        "unknown_v4",
        &ThemeInfo::unknown_v4,
        "unknown_v5",
        &ThemeInfo::unknown_v5,
        "add_special_rooms",
        &ThemeInfo::add_special_rooms,
        "unknown_v7",
        &ThemeInfo::unknown_v7,
        "unknown_v8",
        &ThemeInfo::unknown_v8,
        "add_vault",
        &ThemeInfo::add_vault,
        "add_coffin",
        &ThemeInfo::add_coffin,
        "add_special_feeling",
        &ThemeInfo::add_special_feeling,
        "unknown_v12",
        &ThemeInfo::unknown_v12,
        "spawn_level",
        &ThemeInfo::spawn_level,
        "spawn_border",
        &ThemeInfo::spawn_border,
        "post_process_level",
        &ThemeInfo::post_process_level,
        "spawn_traps",
        &ThemeInfo::spawn_traps,
        "post_process_entities",
        &ThemeInfo::post_process_entities,
        "spawn_procedural",
        &ThemeInfo::spawn_procedural,
        "spawn_background",
        &ThemeInfo::spawn_background,
        "spawn_lights",
        &ThemeInfo::spawn_lights,
        "spawn_transition",
        &ThemeInfo::spawn_transition,
        "post_transition",
        &ThemeInfo::post_transition,
        "spawn_players",
        &ThemeInfo::spawn_players,
        "spawn_effects",
        &ThemeInfo::spawn_effects,
        "get_level_file",
        &ThemeInfo::get_level_file,
        "get_theme_id",
        &ThemeInfo::get_theme_id,
        "get_base_id",
        &ThemeInfo::get_base_id,
        "get_floor_spreading_type",
        &ThemeInfo::get_floor_spreading_type,
        "get_floor_spreading_type2",
        &ThemeInfo::get_floor_spreading_type2,
        "unknown_v30",
        &ThemeInfo::unknown_v30,
        "get_transition_block_modifier",
        &ThemeInfo::get_transition_block_modifier,
        "unknown_v32",
        &ThemeInfo::unknown_v32,
        "get_backwall_type",
        &ThemeInfo::get_backwall_type,
        "get_border_type",
        &ThemeInfo::get_border_type,
        "get_critter_type",
        &ThemeInfo::get_critter_type,
        "get_liquid_gravity",
        &ThemeInfo::get_liquid_gravity,
        "get_player_damage",
        &ThemeInfo::get_player_damage,
        "unknown_v38",
        &ThemeInfo::unknown_v38,
        "get_backlayer_lut",
        &ThemeInfo::get_backlayer_lut,
        "get_backlayer_light_level",
        &ThemeInfo::get_backlayer_light_level,
        "get_loop",
        &ThemeInfo::get_loop,
        "get_vault_level",
        &ThemeInfo::get_vault_level,
        "get_unknown_1_or_2",
        &ThemeInfo::get_unknown_1_or_2,
        "get_dynamic_texture",
        &ThemeInfo::get_dynamic_texture,
        "pre_transition",
        &ThemeInfo::pre_transition,
        "get_level_height",
        &ThemeInfo::get_level_height,
        "unknown_v47",
        &ThemeInfo::unknown_v47,
        "spawn_decoration",
        &ThemeInfo::spawn_decoration,
        "spawn_decoration2",
        &ThemeInfo::spawn_decoration2,
        "spawn_extra",
        &ThemeInfo::spawn_extra,
        "unknown_v51",
        &ThemeInfo::unknown_v51);

    auto theme_override = sol::overload(
        static_cast<void (CustomTheme::*)(THEME_OVERRIDE, bool)>(&CustomTheme::override),
        static_cast<void (CustomTheme::*)(THEME_OVERRIDE, uint8_t)>(&CustomTheme::override),
        static_cast<void (CustomTheme::*)(THEME_OVERRIDE, sol::function)>(&CustomTheme::override));

    /// Customizable ThemeInfo with ability to override certain theming functions from different themes or write custom functions. Warning: We WILL change these function names, especially the unknown ones, when you figure out what they do.
    lua.new_usertype<CustomTheme>(
        "CustomTheme",
        sol::constructors<CustomTheme(), CustomTheme(uint8_t, uint8_t), CustomTheme(uint8_t, uint8_t, bool)>(),
        "level_file",
        &CustomTheme::level_file,
        "theme",
        &CustomTheme::theme,
        "base_theme",
        sol::property([](CustomTheme& ct) -> uint8_t
                      {
                          if (ct.base_theme < UINT8_MAX)
                              return ct.base_theme + 1;
                          return UINT8_MAX; },
                      [](CustomTheme& ct, uint8_t bt)
                      {
                          if (bt > 0 && bt < UINT8_MAX)
                              ct.base_theme = bt - 1;
                          else
                              ct.base_theme = 0;
                      }),
        "sub_theme",
        &CustomTheme::sub_theme,
        "textures",
        &CustomTheme::textures,
        "override",
        theme_override,
        "pre",
        &CustomTheme::pre,
        "post",
        &CustomTheme::post,
        "unknown1",
        &CustomTheme::unknown1,
        "unknown2",
        &CustomTheme::unknown2,
        "unknown3",
        &CustomTheme::unknown3,
        "unknown4",
        &CustomTheme::unknown4,
        "get_unknown1",
        &CustomTheme::get_unknown1,
        "init_flags",
        &CustomTheme::init_flags,
        "init_level",
        &CustomTheme::init_level,
        "unknown_v4",
        &CustomTheme::unknown_v4,
        "unknown_v5",
        &CustomTheme::unknown_v5,
        "add_special_rooms",
        &CustomTheme::add_special_rooms,
        "unknown_v7",
        &CustomTheme::unknown_v7,
        "unknown_v8",
        &CustomTheme::unknown_v8,
        "add_vault",
        &CustomTheme::add_vault,
        "add_coffin",
        &CustomTheme::add_coffin,
        "add_special_feeling",
        &CustomTheme::add_special_feeling,
        "unknown_v12",
        &CustomTheme::unknown_v12,
        "spawn_level",
        &CustomTheme::spawn_level,
        "spawn_border",
        &CustomTheme::spawn_border,
        "post_process_level",
        &CustomTheme::post_process_level,
        "spawn_traps",
        &CustomTheme::spawn_traps,
        "post_process_entities",
        &CustomTheme::post_process_entities,
        "spawn_procedural",
        &CustomTheme::spawn_procedural,
        "spawn_background",
        &CustomTheme::spawn_background,
        "spawn_lights",
        &CustomTheme::spawn_lights,
        "spawn_transition",
        &CustomTheme::spawn_transition,
        "post_transition",
        &CustomTheme::post_transition,
        "spawn_players",
        &CustomTheme::spawn_players,
        "spawn_effects",
        &CustomTheme::spawn_effects,
        "get_level_file",
        &CustomTheme::get_level_file,
        "get_theme_id",
        &CustomTheme::get_theme_id,
        "get_base_id",
        &CustomTheme::get_base_id,
        "get_floor_spreading_type",
        &CustomTheme::get_floor_spreading_type,
        "get_floor_spreading_type2",
        &CustomTheme::get_floor_spreading_type2,
        "unknown_v30",
        &CustomTheme::unknown_v30,
        "get_transition_block_modifier",
        &CustomTheme::get_transition_block_modifier,
        "unknown_v32",
        &CustomTheme::unknown_v32,
        "get_backwall_type",
        &CustomTheme::get_backwall_type,
        "get_border_type",
        &CustomTheme::get_border_type,
        "get_critter_type",
        &CustomTheme::get_critter_type,
        "get_liquid_gravity",
        &CustomTheme::get_liquid_gravity,
        "get_player_damage",
        &CustomTheme::get_player_damage,
        "unknown_v38",
        &CustomTheme::unknown_v38,
        "get_backlayer_lut",
        &CustomTheme::get_backlayer_lut,
        "get_backlayer_light_level",
        &CustomTheme::get_backlayer_light_level,
        "get_loop",
        &CustomTheme::get_loop,
        "get_vault_level",
        &CustomTheme::get_vault_level,
        "get_unknown_1_or_2",
        &CustomTheme::get_unknown_1_or_2,
        "get_dynamic_texture",
        &CustomTheme::get_dynamic_texture,
        "pre_transition",
        &CustomTheme::pre_transition,
        "get_level_height",
        &CustomTheme::get_level_height,
        "unknown_v47",
        &CustomTheme::unknown_v47,
        "spawn_decoration",
        &CustomTheme::spawn_decoration,
        "spawn_decoration2",
        &CustomTheme::spawn_decoration2,
        "spawn_extra",
        &CustomTheme::spawn_extra,
        "unknown_v51",
        &CustomTheme::unknown_v51);

    lua.create_named_table("DYNAMIC_TEXTURE", "INVISIBLE", DYNAMIC_TEXTURE::INVISIBLE, "BACKGROUND", DYNAMIC_TEXTURE::BACKGROUND, "FLOOR", DYNAMIC_TEXTURE::FLOOR, "DOOR", DYNAMIC_TEXTURE::DOOR, "DOOR_LAYER", DYNAMIC_TEXTURE::DOOR_LAYER, "BACKGROUND_DECORATION", DYNAMIC_TEXTURE::BACKGROUND_DECORATION, "KALI_STATUE", DYNAMIC_TEXTURE::KALI_STATUE, "COFFIN", DYNAMIC_TEXTURE::COFFIN);

    /// Overrides for different CustomTheme functions. Warning: We WILL change these, especially the unknown ones, and even the known ones if they turn out wrong in testing.
    lua.create_named_table("THEME_OVERRIDE", "BASE", THEME_OVERRIDE::BASE, "UNKNOWN_V1", THEME_OVERRIDE::UNKNOWN_V1, "INIT_FLAGS", THEME_OVERRIDE::INIT_FLAGS, "INIT_LEVEL", THEME_OVERRIDE::INIT_LEVEL, "UNKNOWN_V4", THEME_OVERRIDE::UNKNOWN_V4, "UNKNOWN_V5", THEME_OVERRIDE::UNKNOWN_V5, "SPECIAL_ROOMS", THEME_OVERRIDE::SPECIAL_ROOMS, "UNKNOWN_V7", THEME_OVERRIDE::UNKNOWN_V7, "UNKNOWN_V8", THEME_OVERRIDE::UNKNOWN_V8, "VAULT", THEME_OVERRIDE::VAULT, "COFFIN", THEME_OVERRIDE::COFFIN, "FEELING", THEME_OVERRIDE::FEELING, "UNKNOWN_V12", THEME_OVERRIDE::UNKNOWN_V12, "SPAWN_LEVEL", THEME_OVERRIDE::SPAWN_LEVEL, "SPAWN_BORDER", THEME_OVERRIDE::SPAWN_BORDER, "POST_PROCESS_LEVEL", THEME_OVERRIDE::POST_PROCESS_LEVEL, "SPAWN_TRAPS", THEME_OVERRIDE::SPAWN_TRAPS, "POST_PROCESS_ENTITIES", THEME_OVERRIDE::POST_PROCESS_ENTITIES, "SPAWN_PROCEDURAL", THEME_OVERRIDE::SPAWN_PROCEDURAL, "SPAWN_BACKGROUND", THEME_OVERRIDE::SPAWN_BACKGROUND, "SPAWN_LIGHTS", THEME_OVERRIDE::SPAWN_LIGHTS, "SPAWN_TRANSITION", THEME_OVERRIDE::SPAWN_TRANSITION, "POST_TRANSITION", THEME_OVERRIDE::POST_TRANSITION, "SPAWN_PLAYERS", THEME_OVERRIDE::SPAWN_PLAYERS, "SPAWN_EFFECTS", THEME_OVERRIDE::SPAWN_EFFECTS, "LVL_FILE", THEME_OVERRIDE::LVL_FILE, "THEME_ID", THEME_OVERRIDE::THEME_ID, "BASE_ID", THEME_OVERRIDE::BASE_ID, "ENT_FLOOR_SPREADING", THEME_OVERRIDE::ENT_FLOOR_SPREADING, "ENT_FLOOR_SPREADING2", THEME_OVERRIDE::ENT_FLOOR_SPREADING2, "UNKNOWN_V30", THEME_OVERRIDE::UNKNOWN_V30, "TRANSITION_MODIFIER", THEME_OVERRIDE::TRANSITION_MODIFIER, "UNKNOWN_V32", THEME_OVERRIDE::UNKNOWN_V32, "ENT_BACKWALL", THEME_OVERRIDE::ENT_BACKWALL, "ENT_BORDER", THEME_OVERRIDE::ENT_BORDER, "ENT_CRITTER", THEME_OVERRIDE::ENT_CRITTER, "GRAVITY", THEME_OVERRIDE::GRAVITY, "PLAYER_DAMAGE", THEME_OVERRIDE::PLAYER_DAMAGE, "UNKNOWN_V38", THEME_OVERRIDE::UNKNOWN_V38, "TEXTURE_BACKLAYER_LUT", THEME_OVERRIDE::TEXTURE_BACKLAYER_LUT, "BACKLAYER_LIGHT_LEVEL", THEME_OVERRIDE::BACKLAYER_LIGHT_LEVEL, "LOOP", THEME_OVERRIDE::LOOP, "VAULT_LEVEL", THEME_OVERRIDE::VAULT_LEVEL, "GET_UNKNOWN1_OR_2", THEME_OVERRIDE::GET_UNKNOWN1_OR_2, "TEXTURE_DYNAMIC", THEME_OVERRIDE::TEXTURE_DYNAMIC, "PRE_TRANSITION", THEME_OVERRIDE::PRE_TRANSITION, "LEVEL_HEIGHT", THEME_OVERRIDE::LEVEL_HEIGHT, "UNKNOWN_V47", THEME_OVERRIDE::UNKNOWN_V47, "SPAWN_DECORATION", THEME_OVERRIDE::SPAWN_DECORATION, "SPAWN_DECORATION2", THEME_OVERRIDE::SPAWN_DECORATION2, "SPAWN_EXTRA", THEME_OVERRIDE::SPAWN_EXTRA, "UNKNOWN_V51", THEME_OVERRIDE::UNKNOWN_V51);

    /// Force a theme in PRE_LOAD_LEVEL_FILES, POST_ROOM_GENERATION or PRE_LEVEL_GENERATION to change different aspects of the levelgen. You can pass a CustomTheme, ThemeInfo or THEME.
    lua["force_custom_theme"] = sol::overload(
        [](CustomTheme* customtheme)
        {
            State::get().ptr()->current_theme = customtheme;
        },
        [](ThemeInfo* customtheme)
        {
            State::get().ptr()->current_theme = customtheme;
        },
        [](uint32_t customtheme)
        {
            if (customtheme < 18)
                State::get().ptr()->current_theme = State::get().ptr()->level_gen->themes[customtheme - 1];
        });

    /// Force current subtheme used in the CO theme. You can pass a CustomTheme, ThemeInfo or THEME. Not to be confused with force_co_subtheme.
    lua["force_custom_subtheme"] = sol::overload(
        [](CustomTheme* customtheme)
        {
            State::get().ptr()->level_gen->theme_cosmicocean->sub_theme = customtheme;
        },
        [](ThemeInfo* customtheme)
        {
            State::get().ptr()->level_gen->theme_cosmicocean->sub_theme = customtheme;
        },
        [](uint32_t customtheme)
        {
            if (customtheme < 18)
                State::get().ptr()->level_gen->theme_cosmicocean->sub_theme = State::get().ptr()->level_gen->themes[customtheme - 1];
        });

    // Context received in ON.PRE_LOAD_LEVEL_FILES, used for forcing specific `.lvl` files to load.
    lua.new_usertype<PreLoadLevelFilesContext>(
        "PreLoadLevelFilesContext",
        sol::no_constructor,
        "override_level_files",
        &PreLoadLevelFilesContext::override_level_files,
        "add_level_files",
        &PreLoadLevelFilesContext::add_level_files);

    lua.new_usertype<DoorCoords>("DoorCoords", sol::no_constructor, "door1_x", &DoorCoords::door1_x, "door1_y", &DoorCoords::door1_y, "door2_x", &DoorCoords::door2_x, "door2_y", &DoorCoords::door2_y);

    lua.new_usertype<LevelGenSystem>(
        "LevelGenSystem",
        sol::no_constructor,
        "shop_type",
        &LevelGenSystem::shop_type,
        "backlayer_shop_type",
        &LevelGenSystem::backlayer_shop_type,
        "spawn_x",
        &LevelGenSystem::spawn_x,
        "spawn_y",
        &LevelGenSystem::spawn_y,
        "spawn_room_x",
        &LevelGenSystem::spawn_room_x,
        "spawn_room_y",
        &LevelGenSystem::spawn_room_y,
        "exits",
        &LevelGenSystem::exit_doors_locations,
        "exit_doors",
        &LevelGenSystem::exit_doors,
        "themes",
        sol::property([](LevelGenSystem& lgs)
                      { return std::ref(lgs.themes); }),
        "flags",
        &LevelGenSystem::flags);

    // Context received in ON.POST_ROOM_GENERATION.
    // Used to change the room templates in the level and other shenanigans that affect level gen.
    lua.new_usertype<PostRoomGenerationContext>(
        "PostRoomGenerationContext",
        sol::no_constructor,
        "set_room_template",
        &PostRoomGenerationContext::set_room_template,
        "mark_as_machine_room_origin",
        &PostRoomGenerationContext::mark_as_machine_room_origin,
        "mark_as_set_room",
        &PostRoomGenerationContext::mark_as_set_room,
        "unmark_as_set_room",
        &PostRoomGenerationContext::unmark_as_set_room,
        "set_shop_type",
        &PostRoomGenerationContext::set_shop_type,
        "set_procedural_spawn_chance",
        &PostRoomGenerationContext::set_procedural_spawn_chance,
        "set_num_extra_spawns",
        &PostRoomGenerationContext::set_num_extra_spawns,
        "define_short_tile_code",
        &PostRoomGenerationContext::define_short_tile_code,
        "change_short_tile_code",
        &PostRoomGenerationContext::change_short_tile_code);

    auto find_all_short_tile_codes = [&lua](PreHandleRoomTilesContext& ctx, LAYER layer, SHORT_TILE_CODE short_tile_code)
    {
        auto positions = ctx.find_all_short_tile_codes(layer, short_tile_code);
        sol::table positions_converted = lua.create_table();
        for (size_t i = 0; i < positions.size(); i++)
        {
            auto& tuple = positions[i];
            sol::table tuple_table = lua.create_table();
            tuple_table[1] = std::get<0>(tuple);
            tuple_table[2] = std::get<1>(tuple);
            tuple_table[3] = std::get<2>(tuple);
            positions_converted[i + 1] = tuple_table;
        }
        return positions_converted;
    };

    // Context received in ON.PRE_HANDLE_ROOM_TILES.
    // Used to change the room data as well as add a backlayer room if none is set yet.
    lua.new_usertype<PreHandleRoomTilesContext>(
        "PreHandleRoomTilesContext",
        sol::no_constructor,
        "get_short_tile_code",
        &PreHandleRoomTilesContext::get_short_tile_code,
        "set_short_tile_code",
        &PreHandleRoomTilesContext::set_short_tile_code,
        "find_all_short_tile_codes",
        find_all_short_tile_codes,
        "replace_short_tile_code",
        &PreHandleRoomTilesContext::replace_short_tile_code,
        "has_back_layer",
        &PreHandleRoomTilesContext::has_back_layer,
        "add_empty_back_layer",
        &PreHandleRoomTilesContext::add_empty_back_layer,
        "add_copied_back_layer",
        &PreHandleRoomTilesContext::add_copied_back_layer);

    lua.new_usertype<ShortTileCodeDef>(
        "ShortTileCodeDef",
        "tile_code",
        &ShortTileCodeDef::tile_code,
        "chance",
        &ShortTileCodeDef::chance,
        "alt_tile_code",
        &ShortTileCodeDef::alt_tile_code);

    lua.new_usertype<QuestsInfo>(
        "QuestsInfo",
        "yang_state",
        &QuestsInfo::yang_state,
        "jungle_sisters_flags",
        &QuestsInfo::jungle_sisters_flags,
        "van_horsing_state",
        &QuestsInfo::van_horsing_state,
        "sparrow_state",
        &QuestsInfo::sparrow_state,
        "madame_tusk_state",
        &QuestsInfo::madame_tusk_state,
        "beg_state",
        &QuestsInfo::beg_state);

    lua.new_usertype<SaveData>(
        "SaveData",
        "places",
        sol::readonly(&SaveData::places),
        "bestiary",
        sol::readonly(&SaveData::bestiary),
        "people",
        sol::readonly(&SaveData::people),
        "items",
        sol::readonly(&SaveData::items),
        "traps",
        sol::readonly(&SaveData::traps),
        "last_daily",
        sol::readonly(&SaveData::last_daily),
        "characters",
        sol::readonly(&SaveData::characters),
        "shortcuts",
        sol::readonly(&SaveData::shortcuts),
        "bestiary_killed",
        sol::readonly(&SaveData::bestiary_killed),
        "bestiary_killed_by",
        sol::readonly(&SaveData::bestiary_killed_by),
        "people_killed",
        sol::readonly(&SaveData::people_killed),
        "people_killed_by",
        sol::readonly(&SaveData::people_killed_by),
        "plays",
        sol::readonly(&SaveData::plays),
        "deaths",
        sol::readonly(&SaveData::deaths),
        "wins_normal",
        sol::readonly(&SaveData::wins_normal),
        "wins_hard",
        sol::readonly(&SaveData::wins_hard),
        "wins_special",
        sol::readonly(&SaveData::wins_special),
        "score_total",
        sol::readonly(&SaveData::score_total),
        "score_top",
        sol::readonly(&SaveData::score_top),
        "deepest_area",
        sol::readonly(&SaveData::deepest_area),
        "deepest_level",
        sol::readonly(&SaveData::deepest_level),
        "time_best",
        sol::readonly(&SaveData::time_best),
        "time_total",
        sol::readonly(&SaveData::time_total),
        "time_tutorial",
        sol::readonly(&SaveData::time_tutorial),
        "character_deaths",
        sol::readonly(&SaveData::character_deaths),
        "pets_rescued",
        sol::readonly(&SaveData::pets_rescued),
        "completed_normal",
        sol::readonly(&SaveData::completed_normal),
        "completed_ironman",
        sol::readonly(&SaveData::completed_ironman),
        "completed_hard",
        sol::readonly(&SaveData::completed_hard),
        "profile_seen",
        sol::readonly(&SaveData::profile_seen),
        "seeded_unlocked",
        sol::readonly(&SaveData::seeded_unlocked),
        "world_last",
        sol::readonly(&SaveData::world_last),
        "level_last",
        sol::readonly(&SaveData::level_last),
        "score_last",
        sol::readonly(&SaveData::score_last),
        "time_last",
        sol::readonly(&SaveData::time_last),
        "stickers",
        sol::readonly(&SaveData::stickers),
        "players",
        sol::readonly(&SaveData::players),
        "constellation",
        &SaveData::constellation);

    lua.new_usertype<Constellation>(
        "Constellation",
        "star_count",
        &Constellation::star_count,
        "stars",
        &Constellation::stars,
        "scale",
        &Constellation::scale,
        "line_count",
        &Constellation::line_count,
        "lines",
        &Constellation::lines,
        "line_red_intensity",
        &Constellation::line_red_intensity);

    lua.new_usertype<ConstellationStar>(
        "ConstellationStar",
        "type",
        &ConstellationStar::type,
        "x",
        &ConstellationStar::x,
        "y",
        &ConstellationStar::y,
        "size",
        &ConstellationStar::size,
        "red",
        &ConstellationStar::red,
        "green",
        &ConstellationStar::green,
        "blue",
        &ConstellationStar::blue,
        "alpha",
        &ConstellationStar::alpha,
        "halo_red",
        &ConstellationStar::halo_red,
        "halo_green",
        &ConstellationStar::halo_green,
        "halo_blue",
        &ConstellationStar::halo_blue,
        "halo_alpha",
        &ConstellationStar::halo_alpha,
        "canis_ring",
        &ConstellationStar::canis_ring,
        "fidelis_ring",
        &ConstellationStar::fidelis_ring);

    lua.new_usertype<ConstellationLine>(
        "ConstellationLine",
        "from",
        &ConstellationLine::from,
        "to",
        &ConstellationLine::to);

    lua.create_named_table("LAYER", "FRONT", 0, "BACK", 1, "PLAYER", -1, "PLAYER1", -1, "PLAYER2", -2, "PLAYER3", -3, "PLAYER4", -4, "BOTH", -128);
    lua.create_named_table(
        "THEME",
        "DWELLING",
        1,
        "JUNGLE",
        2,
        "VOLCANA",
        3,
        "OLMEC",
        4,
        "TIDE_POOL",
        5,
        "TEMPLE",
        6,
        "ICE_CAVES",
        7,
        "NEO_BABYLON",
        8,
        "SUNKEN_CITY",
        9,
        "COSMIC_OCEAN",
        10,
        "CITY_OF_GOLD",
        11,
        "DUAT",
        12,
        "ABZU",
        13,
        "TIAMAT",
        14,
        "EGGPLANT_WORLD",
        15,
        "HUNDUN",
        16,
        "BASE_CAMP",
        17,
        "ARENA",
        18);

    /// Parameter to force_co_subtheme
    lua.create_named_table("COSUBTHEME", "NONE", -1, "RESET", -1, "DWELLING", 0, "JUNGLE", 1, "VOLCANA", 2, "TIDE_POOL", 3, "TEMPLE", 4, "ICE_CAVES", 5, "NEO_BABYLON", 6, "SUNKEN_CITY", 7);

    /// Yang quest states
    lua.create_named_table("YANG", "ANGRY", -1, "QUEST_NOT_STARTED", 0, "TURKEY_PEN_SPAWNED", 2, "BOTH_TURKEYS_DELIVERED", 3, "TURKEY_SHOP_SPAWNED", 4, "ONE_TURKEY_BOUGHT", 5, "TWO_TURKEYS_BOUGHT", 6, "THREE_TURKEYS_BOUGHT", 7);

    /// Jungle sister quest flags (angry = -1)
    lua.create_named_table("JUNGLESISTERS", "PARSLEY_RESCUED", 1, "PARSNIP_RESCUED", 2, "PARMESAN_RESCUED", 3, "WARNING_ONE_WAY_DOOR", 4, "GREAT_PARTY_HUH", 5, "I_WISH_BROUGHT_A_JACKET", 6);

    /// Van Horsing quest states
    lua.create_named_table("VANHORSING", "QUEST_NOT_STARTED", 0, "JAILCELL_SPAWNED", 1, "FIRST_ENCOUNTER_DIAMOND_THROWN", 2, "SPAWNED_IN_VLADS_CASTLE", 3, "SHOT_VLAD", 4, "TEMPLE_HIDEOUT_SPAWNED", 5, "SECOND_ENCOUNTER_COMPASS_THROWN", 6, "TUSK_CELLAR", 7);

    /// Sparrow quest states
    lua.create_named_table("SPARROW", "QUEST_NOT_STARTED", 0, "THIEF_STATUS", 1, "FINISHED_LEVEL_WITH_THIEF_STATUS", 2, "FIRST_HIDEOUT_SPAWNED_ROPE_THROW", 3, "FIRST_ENCOUNTER_ROPES_THROWN", 4, "TUSK_IDOL_STOLEN", 5, "SECOND_HIDEOUT_SPAWNED_NEOBAB", 6, "SECOND_ENCOUNTER_INTERACTED", 7, "MEETING_AT_TUSK_BASEMENT", 8);

    /// Madame Tusk quest states
    lua.create_named_table("TUSK", "ANGRY", -2, "DEAD", -1, "QUEST_NOT_STARTED", 0, "DICE_HOUSE_SPAWNED", 1, "HIGH_ROLLER_STATUS", 2, "PALACE_WELCOME_MESSAGE", 3);

    /// Beg quest states
    lua.create_named_table("BEG", "QUEST_NOT_STARTED", 0, "ALTAR_DESTROYED", 1, "SPAWNED_WITH_BOMBBAG", 2, "BOMBBAG_THROWN", 3, "SPAWNED_WITH_TRUECROWN", 4, "TRUECROWN_THROWN", 5);

    /// Use in `define_room_template` to declare whether a room template has any special behavior
    lua.create_named_table("ROOM_TEMPLATE_TYPE", "NONE", 0, "ENTRANCE", 1, "EXIT", 2, "SHOP", 3, "MACHINE_ROOM", 4);

    /// Determines which kind of shop spawns in the level, if any

    lua.create_named_table(
        "SHOP_TYPE",
        "GENERAL_STORE",
        0,
        "CLOTHING_SHOP",
        1,
        "WEAPON_SHOP",
        2,
        "SPECIALTY_SHOP",
        3,
        "HIRED_HAND_SHOP",
        4,
        "PET_SHOP",
        5,
        "DICE_SHOP",
        6,
        "HEDJET_SHOP",
        8,
        "CURIO_SHOP",
        9,
        "CAVEMAN_SHOP",
        10,
        "TURKEY_SHOP",
        11,
        "GHIST_SHOP",
        12,
        "TUSK_DICE_SHOP",
        13);

    /// Use with `get_level_config`
    lua.create_named_table(
        "LEVEL_CONFIG",
        "BACK_ROOM_CHANCE",
        0,
        "BACK_ROOM_INTERCONNECTION_CHANCE",
        1,
        "BACK_ROOM_HIDDEN_DOOR_CHANCE",
        2,
        "BACK_ROOM_HIDDEN_DOOR_CACHE_CHANCE",
        3,
        "MOUNT_CHANCE",
        4,
        "ALTAR_ROOM_CHANCE",
        5,
        "IDOL_ROOM_CHANCE",
        6,
        "FLOOR_SIDE_SPREAD_CHANCE",
        7,
        "FLOOR_BOTTOM_SPREAD_CHANCE",
        8,
        "BACKGROUND_CHANCE",
        9,
        "GROUND_BACKGROUND_CHANCE",
        10,
        "MACHINE_BIGROOM_CHANCE",
        11,
        "MACHINE_WIDEROOM_CHANCE",
        12,
        "MACHINE_TALLROOM_CHANCE",
        13,
        "MACHINE_REWARDROOM_CHANCE",
        14,
        "MAX_LIQUID_PARTICLES",
        15,
        "FLAGGED_LIQUID_ROOMS",
        16);

    lua.create_named_table("TILE_CODE"
                           //, "EMPTY", 0
                           //, "", ...check__[tile_codes.txt]\[game_data/tile_codes.txt\]...
    );
    for (const auto& [tile_code_name, tile_code] : State::get().ptr()->level_gen->data->tile_codes)
    {
        std::string clean_tile_code_name = tile_code_name.c_str();
        std::transform(
            clean_tile_code_name.begin(), clean_tile_code_name.end(), clean_tile_code_name.begin(), [](unsigned char c)
            { return (unsigned char)std::toupper(c); });
        std::replace(clean_tile_code_name.begin(), clean_tile_code_name.end(), '-', '_');
        lua["TILE_CODE"][std::move(clean_tile_code_name)] = tile_code.id;
    };

    lua.create_named_table("ROOM_TEMPLATE"
                           //, "SIDE", 0
                           //, "", ...check__[room_templates.txt]\[game_data/room_templates.txt\]...
    );

    auto room_templates = State::get().ptr()->level_gen->data->room_templates;
    room_templates["empty_backlayer"] = {9};
    room_templates["boss_arena"] = {22};
    room_templates["shop_jail_backlayer"] = {44};
    room_templates["waddler"] = {86};
    room_templates["ghistshop_backlayer"] = {87};
    room_templates["challange_entrance_backlayer"] = {90};
    room_templates["blackmarket"] = {118};
    room_templates["mothership_room"] = {125};
    for (const auto& [room_name, room_template] : room_templates)
    {
        std::string clean_room_name = room_name.c_str();
        std::transform(
            clean_room_name.begin(), clean_room_name.end(), clean_room_name.begin(), [](unsigned char c)
            { return (unsigned char)std::toupper(c); });
        std::replace(clean_room_name.begin(), clean_room_name.end(), '-', '_');
        lua["ROOM_TEMPLATE"][std::move(clean_room_name)] = room_template.id;
    };

    lua.create_named_table("PROCEDURAL_CHANCE"
                           //, "ARROWTRAP_CHANCE", 0
                           //, "", ...check__[spawn_chances.txt]\[game_data/spawn_chances.txt\]...
    );
    auto* state = State::get().ptr();
    for (auto* chances : {&state->level_gen->data->monster_chances, &state->level_gen->data->trap_chances})
    {
        for (const auto& [chance_name, chance] : *chances)
        {
            std::string clean_chance_name = chance_name.c_str();
            std::transform(
                clean_chance_name.begin(), clean_chance_name.end(), clean_chance_name.begin(), [](unsigned char c)
                { return (unsigned char)std::toupper(c); });
            std::replace(clean_chance_name.begin(), clean_chance_name.end(), '-', '_');
            lua["PROCEDURAL_CHANCE"][std::move(clean_chance_name)] = chance.id;
        }
    }
}
}; // namespace NLevel
