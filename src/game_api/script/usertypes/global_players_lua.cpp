#include "global_players_lua.hpp"

#include "entities_chars.hpp"
#include "state.hpp"

#include <sol/sol.hpp>
#include <tuple>
#include <vector>

class Player;

struct Players
{
    // This is probably over complicating
    // but i couldn't find better solution for the global players to be always correct
    // (not return reference to non existing entity when in between screens etc. like in draw callback)

    using value_type = Player*;
    using iterator = std::vector<Player*>::iterator;

    Players()
    {
        update();
    }
    size_t size()
    {
        update();
        return p.size();
    }
    Player* at(const int index)
    {
        update();
        if (index < 0 || index >= p.size())
            return nullptr;

        return p[index];
    }
    auto begin()
    {
        return p.begin();
    }
    auto end()
    {
        return p.end();
    }

  private:
    std::vector<Player*> p;

    void update()
    {
        p = get_state_ptr()->get_players();
    }
    struct lua_iterator_state
    {
        typedef std::vector<Player*>::iterator it_t;
        it_t begin;
        it_t it;
        it_t last;

        lua_iterator_state(Players& mt)
            : begin(mt.begin()), it(mt.begin()), last(mt.end()){};
    };
    static std::tuple<sol::object, sol::object> my_next(sol::user<lua_iterator_state&> user_it_state, sol::this_state l)
    {
        // this gets called
        // to start the first iteration, and every
        // iteration there after

        lua_iterator_state& it_state = user_it_state;
        auto& it = it_state.it;
        if (it == it_state.last)
        {
            // return nil to signify that there's nothing more to work with.
            return std::make_tuple(sol::object(sol::lua_nil), sol::object(sol::lua_nil));
        }
        // 2 values are returned (pushed onto the stack):
        // the key and the value
        // the state is left alone
        auto r = std::make_tuple(
            sol::object(l, sol::in_place, it - it_state.begin + 1),
            sol::object(l, sol::in_place, *it));
        // the iterator must be moved forward one before we return
        std::advance(it, 1);
        return r;
    }

  public:
    static auto my_pairs(Players& mt)
    {
        mt.update();
        // pairs expects 3 returns:
        // the "next" function on how to advance,
        // the "table" itself or some state,
        // and an initial key value (can be nil)

        // prepare our state
        lua_iterator_state it_state(mt);
        // sol::user is a space/time optimization over regular
        // usertypes, it's incompatible with regular usertypes and
        // stores the type T directly in lua without any pretty
        // setup saves space allocation and a single dereference
        return std::make_tuple(&my_next, sol::user<lua_iterator_state>(std::move(it_state)), sol::lua_nil);
    }
};

namespace NGPlayers
{
void register_usertypes(sol::state& lua)
{
    /// NoDoc
    lua.new_usertype<Players>(
        "Players", sol::no_constructor, sol::meta_function::index, [](Players* p, const int index)
        { return p->at(index - 1); },
        sol::meta_function::pairs,
        Players::my_pairs);
    Players players;

    lua["players"] = players;
};
} // namespace NGPlayers
