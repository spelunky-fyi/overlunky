#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

using BucketItem = std::variant<bool, int64_t, float, std::string>;

class Overlunky
{
  public:
    std::map<std::string, BucketItem> options;
    std::map<std::string, BucketItem> set_options;

    std::map<std::string, int64_t> keys;
    std::unordered_set<std::string> ignore_keys;
    std::unordered_set<int64_t> ignore_keycodes;
};

class Bucket
{
  public:
    static Bucket* get();

    std::unordered_map<std::string, BucketItem> data;
    Overlunky* overlunky{nullptr};
};
