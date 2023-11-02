#include "bucket_lua.hpp"

#include "bucket.hpp"

namespace NBucket
{
void register_usertypes(sol::state& lua)
{
    /// Used to get and set Overlunky settings in Bucket
    auto ol_type = lua.new_usertype<Overlunky>("Overlunky", sol::no_constructor);
    ol_type["options"] = &Overlunky::options;
    ol_type["set_options"] = &Overlunky::set_options;
    ol_type["keys"] = &Overlunky::keys;
    ol_type["ignore_keys"] = &Overlunky::ignore_keys;
    ol_type["ignore_keycodes"] = &Overlunky::ignore_keycodes;

    /// Shared memory structure used for Playlunky-Overlunky interoperability
    auto bucket_type = lua.new_usertype<Bucket>("Bucket", sol::no_constructor);
    bucket_type["data"] = &Bucket::data;
    bucket_type["overlunky"] = sol::readonly(&Bucket::overlunky);

    lua["get_bucket"] = Bucket::get;
}
}; // namespace NBucket
