#include "drops.hpp"
#include "memory.hpp"
#include "search.hpp"

#include <iostream>

using namespace std::string_literals;

std::vector<DropEntry> drop_entries{
    {"ALTAR_DICE_CLIMBINGGLOVES", "\xBA\x0D\x02\x00\x00\xEB\x05"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_COOKEDTURKEY", "\xBA\x06\x02\x00\x00\xEB\x0C"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_DIAMOND", "\xBA\xF1\x01\x00\x00\xE9\x87\x01\x00\x00"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_MACHETE", "\xBA\x48\x02\x00\x00\xEB\x1A"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_ROPEPILE", "\xBA\x00\x02\x00\x00\xEB\x21"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_SPECTACLES", "\xBA\x0C\x02\x00\x00\xEB\x28"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_SNAKE", "\xBA\xDC\x00\x00\x00\xEB\x36"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_TELEPACK", "\xBA\x3A\x02\x00\x00\xEB\x2F"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_VAMPIRE", "\xEB\x05\xBA\xF5\x00\x00\x00"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_DICE_WEBGUN", "\xBA\x40\x02\x00\x00\xEB\x13"s, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_IDOL_GOLDEN_MONKEY", "\x20\x00\xBA\x35\x01\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_KAPALA", "\x20\x00\xBA\x16\x02\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_PRESENT_EGGPLANT", "\x20\x00\xBA\xE7\x01\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_ROCK_WOODENARROW", "\x20\x00\xBA\x73\x01\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_ROYAL_JELLY", "\x20\x00\xBA\x05\x02\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_USHABTI_CAVEMAN", "\x20\x00\xBA\xE1\x00\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_USHABTI_TURKEY", "\x20\x00\xBA\x83\x03\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_USHABTI_VAMPIRE", "\x20\x00\xBA\xF5\x00\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"ANUBIS2_JETPACK", "\x37\x02\x00\x00"s, VTABLE_OFFSET::MONS_ANUBIS2, 3},
    {"ANUBIS_SCEPTER", "\x4C\x02\x00\x00"s, VTABLE_OFFSET::MONS_ANUBIS, 3},
    {"BEG_BOMBBAG", "\xBA\x01\x02\x00\x00\xE8****\x31"s, VTABLE_OFFSET::NONE, 0, 1},
    {"BEG_TRUECROWN", "\x1A\x02\x00\x00"s, VTABLE_OFFSET::MONS_HUNDUNS_SERVANT, 104},
    {"BONEPILE_SKELETONKEY", "\x1D\x02\x00\x00"s, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 3},
    {"BONEPILE_SKULL", "\xE2\x01\x00\x00"s, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 3},
    {"CROCMAN_TELEPACK", "\x3A\x02\x00\x00"s, VTABLE_OFFSET::MONS_CROCMAN, 3},
    {"CROCMAN_TELEPORTER", "\x45\x02\x00\x00"s, VTABLE_OFFSET::MONS_CROCMAN, 3},
    {"GHOSTJAR_DIAMOND", "\xF1\x01\x00\x00"s, VTABLE_OFFSET::ITEM_CURSEDPOT, 3},
    {"GHOST_DIAMOND", "\xF1\x01\x00\x00"s, VTABLE_OFFSET::MONS_GHOST, 26},
    {"GIANTSPIDER_PASTE", "\x11\x02\x00\x00"s, VTABLE_OFFSET::MONS_GIANTSPIDER, 3},
    {"GOLDENMONKEY_SMALLRUBY", "\xF9\x01\x00\x00"s, VTABLE_OFFSET::MONS_GOLDMONKEY, 78},
    {"GOLDENMONKEY_SMALLSAPPHIRE", "\xF8\x01\x00\x00"s, VTABLE_OFFSET::MONS_GOLDMONKEY, 78},
    {"HANGINGSPIDER_WEBGUN", "\x40\x02\x00\x00"s, VTABLE_OFFSET::MONS_HANGSPIDER, 3},
    {"ICECAVE_BOULDER", "\x57\x02\x00\x00"s, VTABLE_OFFSET::LOGICAL_BOULDERSPAWNER, 2},
    {"JIANGSHIASSASSIN_SPIKESHOES", "\x10\x02\x00\x00"s, VTABLE_OFFSET::MONS_FEMALE_JIANGSHI, 3},
    {"JIANGSHI_SPRINGSHOES", "\x0F\x02\x00\x00"s, VTABLE_OFFSET::MONS_JIANGSHI, 3},
    {"KINGU_TABLETOFDESTINY", "\x1C\x02\x00\x00"s, VTABLE_OFFSET::MONS_KINGU, 3},
    {"LEPRECHAUN_CLOVER", "\x09\x02\x00\x00"s, VTABLE_OFFSET::MONS_LEPRECHAUN, 3},
    {"MATTOCK_BROKENMATTOCK", "\xAC\x01\x00\x00"s, VTABLE_OFFSET::ITEM_MATTOCK, 77},
    {"MOLE_MATTOCK", "\x46\x02\x00\x00"s, VTABLE_OFFSET::MONS_MOLE, 3},
    {"MOSQUITO_HOVERPACK", "\x3C\x02\x00\x00"s, VTABLE_OFFSET::MONS_MOSQUITO, 3},
    {"MUMMY_DIAMOND", "\xF1\x01\x00\x00"s, VTABLE_OFFSET::MONS_MUMMY, 3},
    {"MUMMY_FLY", "\xB4\x01\x00\x00"s, VTABLE_OFFSET::LOGICAL_MUMMYFLIES_SOUND_SOURCE, 2},
    {"NECROMANCER_RUBY", "\xF4\x01\x00\x00"s, VTABLE_OFFSET::MONS_NECROMANCER, 3},
    {"OLMEC_BOMB", "\x5B\x01\x00\x00"s, VTABLE_OFFSET::ITEM_OLMECCANNON_BOMBS, 93},
    {"OLMEC_CAVEMAN_1", "\xBA\xE1\x00\x00\x00\xE8\x58\xB3\xD7\xFF"s, VTABLE_OFFSET::NONE, 0, 1},
    {"OLMEC_CAVEMAN_2", "\xBA\xE1\x00\x00\x00\xE8\xA4\xB2\xD7\xFF"s, VTABLE_OFFSET::NONE, 0, 1},
    {"OLMEC_CAVEMAN_3", "\xBA\xE1\x00\x00\x00\xE8\xD4\xB1\xD7\xFF"s, VTABLE_OFFSET::NONE, 0, 1},
    {"OLMEC_UFO", "\x0A\x01\x00\x00"s, VTABLE_OFFSET::ITEM_OLMECCANNON_UFO, 93},
    {"OSIRIS_EMERALDS", "\x20\x00\xBA\xF2\x01\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 3},
    {"OSIRIS_TABLETOFDESTINY", "\xBA\x1C\x02\x00\x00\xE8****\x8B\x0D"s, VTABLE_OFFSET::NONE, 0, 1},
    {"OSIRIS_PORTAL", "\xBA\x64\x03\x00\x00\xE8****\x66\xC7"s, VTABLE_OFFSET::NONE, 0, 1},
    {"PANGXIE_ACIDBUBBLE", "\xBA\x88\x01\x00\x00\xE8"s, VTABLE_OFFSET::NONE, 0, 1},
    {"QUEENBEE_ROYALJELLY", "\x05\x02\x00\x00"s, VTABLE_OFFSET::MONS_QUEENBEE, 3},
    {"ROBOT_METALSHIELD", "\x50\x02\x00\x00"s, VTABLE_OFFSET::MONS_ROBOT, 3},
    {"SCEPTER_ANUBISSPECIALSHOT", "\x79\x01\x00\x00"s, VTABLE_OFFSET::MONS_ANUBIS, 101}, // anubis regular shot = new_drop_uid - 1 !
    {"SCEPTER_PLAYERSHOT", "\x7A\x01\x00\x00"s, VTABLE_OFFSET::ITEM_SCEPTER, 24},
    {"SHOPKEEPER_GOLDCOIN", "\xBA\xF6\x01\x00\x00\xE8****\x48\x89\xC7\xC7\x44\x24"s, VTABLE_OFFSET::NONE, 0, 1},
    {"SKELETON_SKELETONKEY", "\x1D\x02\x00\x00"s, VTABLE_OFFSET::MONS_SKELETON, 3},
    {"SKELETON_SKULL", "\xE2\x01\x00\x00"s, VTABLE_OFFSET::MONS_SKELETON, 3},
    {"SORCERESS_RUBY", "\xF4\x01\x00\x00"s, VTABLE_OFFSET::MONS_SORCERESS, 3},
    {"SPARROW_ROPEPILE", "\x00\x02\x00\x00"s, VTABLE_OFFSET::MONS_THIEF, 104},
    {"SPARROW_SKELETONKEY", "\x1D\x02\x00\x00"s, VTABLE_OFFSET::MONS_THIEF, 3},
    {"TIAMAT_BAT", "\xE0\x00\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_BEE", "\x16\x01\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_CAVEMAN", "\xE1\x00\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_COBRA", "\xF8\x00\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_HERMITCRAB", "\xBA\x08\x01\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78, 1},
    {"TIAMAT_MONKEY", "\xEE\x00\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_MOSQUITO", "\xED\x00\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_OCTOPUS", "\x07\x01\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_OLMITE", "\x15\x01\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_SCORPION", "\xE5\x00\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_SHOT", "\x80\x01\x00\x00"s, VTABLE_OFFSET::MONS_TIAMAT, 78},
    {"TIAMAT_SNAKE", "\xDC\x00\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_UFO", "\x0A\x01\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_YETI", "\xBA\x0C\x01\x00\x00"s, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78, 1},
    {"TORCH_SMALLNUGGET", "\xFA\x01\x00\x00"s, VTABLE_OFFSET::ITEM_LIT_WALLTORCH, 93},
    {"TURKEY_COOKEDTURKEY", "\x06\x02\x00\x00"s, VTABLE_OFFSET::MOUNT_TURKEY, 48},
    {"UFO_PARACHUTE", "\x14\x02\x00\x00"s, VTABLE_OFFSET::MONS_UFO, 78},
    {"VAMPIRE_CAPE", "\x34\x02\x00\x00"s, VTABLE_OFFSET::MONS_VAMPIRE, 3},
    {"VAN_HORSING_COMPASS", "\x13\x02\x00\x00"s, VTABLE_OFFSET::MONS_OLD_HUNTER, 104},
    {"VAN_HORSING_DIAMOND", "\xF1\x01\x00\x00"s, VTABLE_OFFSET::MONS_OLD_HUNTER, 104},
    {"VLAD_VLADSCAPE", "\x35\x02\x00\x00"s, VTABLE_OFFSET::MONS_VAMPIRE, 3},
    {"YETIKING_FREEZERAY", "\x42\x02\x00\x00"s, VTABLE_OFFSET::MONS_YETIKING, 105},
    {"YETIKING_ICESPIRE", "\xE8\x01\x00\x00"s, VTABLE_OFFSET::MONS_YETIKING, 104},
    {"YETIQUEEN_POWERPACK", "\x3E\x02\x00\x00"s, VTABLE_OFFSET::MONS_YETIQUEEN, 105},
    {"YETI_PITCHERSMITT", "\x0E\x02\x00\x00"s, VTABLE_OFFSET::MONS_YETI, 3},
};

std::vector<DropChanceEntry> dropchance_entries{
    {"BONEBLOCK_SKELETONKEY", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 3},
    {"CROCMAN_TELEPACK", "\x64"s, VTABLE_OFFSET::MONS_CROCMAN, 3, 1},
    {"HANGINGSPIDER_WEBGUN", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::MONS_HANGSPIDER, 3},
    {"JIANGSHIASSASSIN_SPIKESHOES", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::MONS_FEMALE_JIANGSHI, 3},
    {"JIANGSHI_SPRINGSHOES", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::MONS_JIANGSHI, 3},
    {"MOLE_MATTOCK", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::MONS_MOLE, 3},
    {"MOSQUITO_HOVERPACK", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::MONS_MOSQUITO, 3},
    {"ROBOT_METALSHIELD", "\x64"s, VTABLE_OFFSET::MONS_ROBOT, 3, 1},
    {"SKELETON_SKELETONKEY", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::MONS_SKELETON, 3},
    {"UFO_PARACHUTE", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::MONS_UFO, 78},
    {"YETI_PITCHERSMITT", "\xE8\x03\x00\x00"s, VTABLE_OFFSET::MONS_YETI, 3},
};

#ifdef PERFORM_DROPS_TEST
void test_drops()
{
    auto memory = Memory::get();
    auto exe = memory.exe();

    for (const auto& drop : drop_entries)
    {
        switch (drop.vtable_offset)
        {
        case VTABLE_OFFSET::NONE:
        {
            size_t times_found = 0;
            size_t address = memory.after_bundle;
            while (true)
            {
                try
                {
                    address = find_inst(exe, drop.pattern, address + 1, "", false);
                    times_found++;
                }
                catch (...)
                {
                    break;
                }
            }
            if (times_found == 0 || times_found > 1)
            {
                std::cerr << "DROP " << drop.caption << ": FOUND " << times_found << " TIMES\n";
            }
            break;
        }
        default:
        {
            auto vfunc_address = get_virtual_function_address(drop.vtable_offset, drop.vtable_rel_offset);
            try
            {
                auto address = find_inst(exe, drop.pattern, vfunc_address, "", false);
                std::cerr << "DROP " << drop.caption << " found with distance " << (address - vfunc_address) << "\n";
            }
            catch (...)
            {
                std::cerr << "DROP " << drop.caption << " NOT FOUND IN VIRTUAL FUNCTION\n";
            }
        }
        }
    }

    for (const auto& dropchance : dropchance_entries)
    {
        switch (dropchance.vtable_offset)
        {
        case VTABLE_OFFSET::NONE:
        {
            size_t times_found = 0;
            size_t address = memory.after_bundle;
            while (true)
            {
                try
                {
                    address = find_inst(exe, dropchance.pattern, address + 1, "", false);
                    times_found++;
                }
                catch (...)
                {
                    break;
                }
            }
            if (times_found == 0 || times_found > 1)
            {
                std::cerr << "DROPCHANCE " << dropchance.caption << ": FOUND " << times_found << " TIMES\n";
            }
            break;
        }
        default:
        {
            auto vfunc_address = get_virtual_function_address(dropchance.vtable_offset, dropchance.vtable_rel_offset);
            try
            {
                auto address = find_inst(exe, dropchance.pattern, vfunc_address, "", false);
                std::cerr << "DROPCHANCE " << dropchance.caption << " found with distance " << (address - vfunc_address) << "\n";
            }
            catch (...)
            {
                std::cerr << "DROPCHANCE " << dropchance.caption << " NOT FOUND IN VIRTUAL FUNCTION\n";
            }
        }
        }
    }
}
#endif
