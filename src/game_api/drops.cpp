#include "drops.hpp"

#include <new>         // for operator new
#include <optional>    // for nullopt
#include <string_view> //
#include <utility>     // for min, max

#include "entity.hpp" // for to_id
#include "memory.hpp" // for Memory, recover_mem, write_mem_recoverable
#include "search.hpp" // for find_inst

std::vector<DropEntry> drop_entries{
    {"ALTAR_DICE_CLIMBINGGLOVES", "\xBA\x0D\x02\x00\x00\xEB\x05"sv, VTABLE_OFFSET::NONE, 0, 1}, // VTABLE_OFFSET::FLOOR_ALTAR, 26
    {"ALTAR_DICE_COOKEDTURKEY", "\xBA\x06\x02\x00\x00\xEB\x0C"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_DIAMOND", "\xBA\xF1\x01\x00\x00\xE9\x87\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_MACHETE", "\xBA\x48\x02\x00\x00\xEB\x1A"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_ROPEPILE", "\xBA\x00\x02\x00\x00\xEB\x21"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_SPECTACLES", "\xBA\x0C\x02\x00\x00\xEB\x28"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_SNAKE", "\xBA\xDC\x00\x00\x00\xEB\x36"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_TELEPACK", "\xBA\x3A\x02\x00\x00\xEB\x2F"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"ALTAR_DICE_VAMPIRE", "\xEB\x05\xBA\xF5\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_DICE_WEBGUN", "\xBA\x40\x02\x00\x00\xEB\x13"sv, VTABLE_OFFSET::NONE, 0, 1},
    /// Has to be ENT_TYPE_CHAR_*
    {"ALTAR_DICE_HIREDHAND", "\x48\x0F\x45\xCE\xC7\x44\x24\x20\xD7\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 8},
    {"ALTAR_IDOL_GOLDEN_MONKEY", "\x20\x00\xBA\x35\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_PRESENT_EGGPLANT", "\x20\x00\xBA\xE7\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_ROCK_WOODENARROW", "\x20\x00\xBA\x73\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_USHABTI_CAVEMAN", "\x20\x00\xBA\xE1\x00\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_USHABTI_TURKEY", "\x20\x00\xBA\x83\x03\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 3},
    {"ALTAR_USHABTI_VAMPIRE", "\x20\x00\xBA\xF5\x00\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 3},
    /// Has to be ENT_TYPE_CHAR_*
    {"ALTAR_USHABTI_HIREDHAND", "\xC7\x44\x24\x20\xD7\x00\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 4}, // this relies on the fact that this it is the first pattern
    /// Shotgun for HH above, if you have enough favor, has to be pickable item
    {"ALTAR_HIREDHAND_SHOTGUN", "\xBA\x41\x02\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 1}, // this relies on the fact that this it is the first pattern
    /// Gift from Kali when reaching 8 favor, this is what you get if you have all the powerups
    {"ALTAR_GIFT_BOMBBAG", "\xBA\x01\x02\x00\x00"sv, VTABLE_OFFSET::FLOOR_ALTAR, 26, 1},
    /// Will spawn only if you don't have any backpack
    {"ALTAR_GIFT_CAPE", "\xBA\x34\x02\x00\x00"sv, VTABLE_OFFSET::FLOOR_ALTAR, 26, 1},
    {"ALTAR_KAPALA", "\xBA\x16\x02\x00\x00"sv, VTABLE_OFFSET::FLOOR_ALTAR, 26, 1},
    {"ALTAR_ROYAL_JELLY", "\xBA\x05\x02\x00\x00\xE8"sv, VTABLE_OFFSET::FLOOR_ALTAR, 26, 1},
    /* Kali gifts are potentially possible, but the game checks if player has the powerup
    which is more complicated */
    {"ANUBIS2_JETPACK", "\x37\x02\x00\x00"sv, VTABLE_OFFSET::MONS_ANUBIS2, 3},
    {"ANUBIS_SCEPTER", "\x4C\x02\x00\x00"sv, VTABLE_OFFSET::MONS_ANUBIS, 3},
    {"BEG_BOMBBAG", "\xBA\x01\x02\x00\x00\xE8****\x31"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"BEG_TRUECROWN", "\x1A\x02\x00\x00"sv, VTABLE_OFFSET::MONS_HUNDUNS_SERVANT, 104},
    {"BEG_TELEPACK", "\xBA\x3A\x02\x00\x00"sv, VTABLE_OFFSET::MONS_HUNDUNS_SERVANT, 48, 1},
    {"BONEPILE_SKELETONKEY", "\x1D\x02\x00\x00"sv, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 3},
    {"BONEPILE_SKULL", "\xE2\x01\x00\x00"sv, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 3},
    {"CROCMAN_TELEPACK", "\x3A\x02\x00\x00"sv, VTABLE_OFFSET::MONS_CROCMAN, 3},
    {"CROCMAN_TELEPORTER", "\x45\x02\x00\x00"sv, VTABLE_OFFSET::MONS_CROCMAN, 3},
    {"GHOSTJAR_DIAMOND", "\xF1\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CURSEDPOT, 3},
    {"GHOST_DIAMOND", "\xF1\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GHOST, 26},
    {"GIANTSPIDER_PASTE", "\x11\x02\x00\x00"sv, VTABLE_OFFSET::MONS_GIANTSPIDER, 3},
    /// Has a chance to be +3 (NUGGET_SMALL)
    {"GOLDENMONKEY_SMALLEMERALD", "\x8D\x94\x6D\xF7\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GOLDMONKEY, 78, 3},
    {"GOLDENMONKEY_SMALLSAPPHIRE", "\xBA\xF8\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GOLDMONKEY, 78, 1},
    {"GOLDENMONKEY_SMALLRUBY", "\xBA\xF9\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GOLDMONKEY, 78, 1},
    {"GOLDENMONKEY_NUGGET", "\xBA\xF5\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GOLDMONKEY, 78, 1},
    {"HANGINGSPIDER_WEBGUN", "\x40\x02\x00\x00"sv, VTABLE_OFFSET::MONS_HANGSPIDER, 3},
    {"ICECAVE_BOULDER", "\x57\x02\x00\x00"sv, VTABLE_OFFSET::LOGICAL_BOULDERSPAWNER, 2},
    {"JIANGSHIASSASSIN_SPIKESHOES", "\x10\x02\x00\x00"sv, VTABLE_OFFSET::MONS_FEMALE_JIANGSHI, 3},
    {"JIANGSHI_SPRINGSHOES", "\x0F\x02\x00\x00"sv, VTABLE_OFFSET::MONS_JIANGSHI, 3},
    {"KINGU_TABLETOFDESTINY", "\xBA\x1C\x02\x00\x00"sv, VTABLE_OFFSET::MONS_KINGU, 3, 1},
    {"KINGU_OCTOPUS", "\xC6\x44\x24\x20\x00\xBA\x07\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"KINGU_JIANGSHI", "\xC6\x44\x24\x20\x00\xBA\x05\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"KINGU_FEMALE_JIANGSHI", "\xC6\x44\x24\x20\x00\xBA\x04\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"LEPRECHAUN_CLOVER", "\x09\x02\x00\x00"sv, VTABLE_OFFSET::MONS_LEPRECHAUN, 3},
    {"MATTOCK_BROKENMATTOCK", "\xAC\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_MATTOCK, 77},
    {"MOLE_MATTOCK", "\x46\x02\x00\x00"sv, VTABLE_OFFSET::MONS_MOLE, 3},
    {"MOSQUITO_HOVERPACK", "\x3C\x02\x00\x00"sv, VTABLE_OFFSET::MONS_MOSQUITO, 3},
    {"MUMMY_DIAMOND", "\xF1\x01\x00\x00"sv, VTABLE_OFFSET::MONS_MUMMY, 3},
    {"NECROMANCER_RUBY", "\xF4\x01\x00\x00"sv, VTABLE_OFFSET::MONS_NECROMANCER, 3},
    {"OLMEC_CAVEMAN_1", "\x48\x89\xF1\xBA\xE1\x00\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 4},
    {"OLMEC_CAVEMAN_2", "\xFF\x90\x80\x00\x00\x00\xC6\x44\x24\x20\x00\xF3\x0F\x10\x15"sv, VTABLE_OFFSET::NONE, 0, 0x1F},
    {"OLMEC_CAVEMAN_3", "\x48\x89\xF1\xBA\xE1\x00\x00\x00\xE8****\x48\x89\xC6"sv, VTABLE_OFFSET::NONE, 0, 4},
    {"OSIRIS_EMERALDS", "\x20\x00\xBA\xF2\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 3},
    {"OSIRIS_TABLETOFDESTINY", "\xBA\x1C\x02\x00\x00\xE8****\x8B\x0D"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"OSIRIS_PORTAL", "\xBA\x64\x03\x00\x00\xE8****\x66\xC7"sv, VTABLE_OFFSET::NONE, 0, 1},
    /// Has a chance to spawn metal shield, in which it just +1 to this value
    {"PANGXIE_WOODENSHIELD", "\xBA\x4F\x02\x00\x00"sv, VTABLE_OFFSET::MONS_CRABMAN, 3, 1},
    {"QUEENBEE_ROYALJELLY", "\x05\x02\x00\x00"sv, VTABLE_OFFSET::MONS_QUEENBEE, 3},
    {"ROBOT_METALSHIELD", "\x50\x02\x00\x00"sv, VTABLE_OFFSET::MONS_ROBOT, 3},
    /// It's actually for all RoomOwner's, so: shopkeeper, tun, yang and madame tusk
    {"SHOPKEEPER_GOLDCOIN", "\xBA\xF6\x01\x00\x00"sv, VTABLE_OFFSET::MONS_YANG, 109, 1},
    /// Rare goldbar from shopkeeper
    {"SHOPKEEPER_GOLDBAR", "\xBA\xEF\x01\x00\x00"sv, VTABLE_OFFSET::MONS_SHOPKEEPER, 109, 1},
    /// GHIST and GHIST_SHOPKEEPER
    {"GHIST_GOLDCOIN", "\xBA\xF6\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GHIST_SHOPKEEPER, 105, 1}, // there is no 105 function for ghist
    {"SKELETON_SKELETONKEY", "\x1D\x02\x00\x00"sv, VTABLE_OFFSET::MONS_SKELETON, 3},
    {"SKELETON_SKULL", "\xE2\x01\x00\x00"sv, VTABLE_OFFSET::MONS_SKELETON, 3},
    {"SORCERESS_RUBY", "\xF4\x01\x00\x00"sv, VTABLE_OFFSET::MONS_SORCERESS, 3},
    {"SPARROW_ROPEPILE", "\x00\x02\x00\x00"sv, VTABLE_OFFSET::MONS_THIEF, 104},
    {"SPARROW_SKELETONKEY", "\x1D\x02\x00\x00"sv, VTABLE_OFFSET::MONS_THIEF, 3},

    /// TIAMAT_SHOT spawns:
    {"TIAMAT_BAT", "\xE0\x00\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_BEE", "\x16\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_CAVEMAN", "\xE1\x00\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_COBRA", "\xF8\x00\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_HERMITCRAB", "\xBA\x08\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78, 1},
    {"TIAMAT_MONKEY", "\xEE\x00\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_MOSQUITO", "\xED\x00\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_OCTOPUS", "\x07\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_OLMITE", "\x15\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_SCORPION", "\xE5\x00\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_SHOT", "\x80\x01\x00\x00"sv, VTABLE_OFFSET::MONS_TIAMAT, 78},
    {"TIAMAT_SNAKE", "\xDC\x00\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_UFO", "\x0A\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78},
    {"TIAMAT_YETI", "\xBA\x0C\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_TIAMAT_SHOT, 78, 1},
    /// ---

    {"TORCH_SMALLNUGGET", "\xFA\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_LITWALLTORCH, 93},
    {"TURKEY_COOKEDTURKEY", "\x06\x02\x00\x00"sv, VTABLE_OFFSET::MOUNT_TURKEY, 48},
    {"UFO_PARACHUTE", "\x14\x02\x00\x00"sv, VTABLE_OFFSET::MONS_UFO, 78},
    {"VAMPIRE_CAPE", "\x34\x02\x00\x00"sv, VTABLE_OFFSET::MONS_VAMPIRE, 3},
    {"VAN_HORSING_COMPASS", "\x13\x02\x00\x00"sv, VTABLE_OFFSET::MONS_OLD_HUNTER, 104},
    {"VAN_HORSING_DIAMOND", "\xF1\x01\x00\x00"sv, VTABLE_OFFSET::MONS_OLD_HUNTER, 104},
    {"VLAD_VLADSCAPE", "\x35\x02\x00\x00"sv, VTABLE_OFFSET::MONS_VLAD, 3},
    {"YETIKING_FREEZERAY", "\x42\x02\x00\x00"sv, VTABLE_OFFSET::MONS_YETIKING, 105},
    {"YETIQUEEN_POWERPACK", "\x3E\x02\x00\x00"sv, VTABLE_OFFSET::MONS_YETIQUEEN, 105},
    {"YETI_PITCHERSMITT", "\x0E\x02\x00\x00"sv, VTABLE_OFFSET::MONS_YETI, 3},
    /* set write bp on Generator.spawned_uid, when it's being set to -1, execute return, find item_scrap id close to this location */
    {"FACTORY_GENERATOR_SCRAP", "\x48\x89\xF9\xBA\xE6\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 4},
    /// Keep in mind that shopkeeper generator spawns two types of entities, the second one will be this + 1
    {"SHOPKEEPER_GENERATOR_1", "\x81\xCA\x10\x01\x00\x00\x4C\x89\xE1"sv, VTABLE_OFFSET::NONE, 0, 2},
    /* set conditional bp on load_item for spider, break some scrap until you hit bp, execute return, you will see spider id right above the spawn function
     * scroll up to find the same pattern but for snake
     * just above the snake you can find cobra and scorpion id, with jump to the spawn function after snake
     * little higher up, same thing for alien */
    {"SCRAP_SPIDER", "\xC6\x44\x24\x20\x00\xBA\xDD\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"SCRAP_SNAKE", "\xC6\x44\x24\x20\x00\xBA\xDC\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"SCRAP_COBRA", "\xC6\x44\x24\x20\x00\xBA\xF8\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"SCRAP_SCORPION", "\xC6\x44\x24\x20\x00\xBA\xE5\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"SCRAP_ALIEN", "\xBA\x0B\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 1},
    /// Will spawn with parachute, so has to be PowerupCapable entity
    {"UFO_ALIEN", "\xBA\x0B\x01\x00\x00"sv, VTABLE_OFFSET::MONS_UFO, 3, 1},
    /* set write bp on Altar->unknown (second byte after timer)
    {"SACRIFICE_DIE", "\x3D\xC0\x01\x00\x00\x0F"sv, VTABLE_OFFSET::NONE, 0, 1}, // depends on animation frame
    {"SACRIFICE_USHABTI", "\x3D\xBA\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 1}, // depends on animation frame */

    /// Game does: (this value & 0xFFFFFFFE) to get idol and tusk idol
    /// Essentially clearing last bit, so it's ether this entity + 1 or -1 or just one entity, depending of the chosen type
    {"SACRIFICE_IDOL", "\x81\xF9\x64\x01\x00\x00\x0F"sv, VTABLE_OFFSET::NONE, 0, 2},
    {"SACRIFICE_PRESENT", "\x8B\x40\x14\x3D\xA6\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 4},
    {"SACRIFICE_ROCK", "\x3D\x6D\x01\x00\x00\x0F\x85"sv, VTABLE_OFFSET::FLOOR_ALTAR, 2, 1},
    {"SACRIFICE_EGGPLANT", "\x48\x8B\x42\x08\x81\x78\x14\xE7\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 7},
    {"YAMA_EGGPLANTCROWN", "\xC6\x44\x24\x20\x00\xBA\x19\x02\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"YAMA_GIANTFOOD", "\xC6\x44\x24\x20\x00\xBA\x07\x02\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"YANG_KEY", "\xba\x8f\x01\x00\x00\xe8\xd6\xe7\xe2\xff\x48\x89\xc7\x84\xdb"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"MADAME_TUSK_KEY", "\xBA\x8F\x01\x00\x00"sv, VTABLE_OFFSET::MONS_MADAMETUSK, 0, 1},
    {"ANUBIS_COFFIN_VAMPIRE", "\x83\xF9\x02\xBA\xF5\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 4},
    {"ANUBIS_COFFIN_SORCERESS", "\x83\xF9\x01\xB8\xFA\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 4},
    {"ANUBIS_COFFIN_WITCHDOCTOR", "\xBF\xEB\x00\x00\x00\x0F\x44\xF8"sv, VTABLE_OFFSET::NONE, 0, 1},
    /// It spawns two types of entities (frog and firefrog), the second one will be this + 1
    /// Crashes the game with most entities
    {"GIANTFROG_FROG", "\x81\xC2\x1B\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GIANTFROG, 78, 2},
    {"GIANTFROG_TADPOLE", "\xBA\x1F\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GIANTFROG, 78, 1},
    {"EGGSAC_GRUB_1", "\xBA\x1E\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"EGGSAC_GRUB_2", "\xBA\x1E\x01\x00\x00\x41\x0F\x28\xD0"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"EGGSAC_GRUB_3", "\xBA\x1E\x01\x00\x00\x0F\x28\xD6"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"QUILLBACK_BOMBBAG", "\xBA\x01\x02\x00\x00"sv, VTABLE_OFFSET::MONS_CAVEMAN_BOSS, 3, 1},
    {"QUILLBACK_COOKEDTURKEY", "\xBA\x06\x02\x00\x00"sv, VTABLE_OFFSET::MONS_CAVEMAN_BOSS, 3, 1},
    {"LAVAPOT_MAGMAMAN", "\xBA\xEF\x00\x00\x00"sv, VTABLE_OFFSET::ITEM_LAVAPOT, 3, 1},
    /// Always spawns 5 of them
    {"LAVAMANDER_RUBY", "\xBA\xF4\x01\x00\x00"sv, VTABLE_OFFSET::MONS_LAVAMANDER, 3, 1}, // 41 80 FE 05 - 05 amount
    {"CATMUMMY_DIAMOND", "\xBA\xF1\x01\x00\x00"sv, VTABLE_OFFSET::MONS_CATMUMMY, 3, 1},
    /// Has to be ENT_TYPE_CHAR_*
    {"HUMPHEAD_HIREDHAND", "\xC7\x44\x24\x20\xD7\x00\x00\x00"sv, VTABLE_OFFSET::MONS_GIANTFISH, 3, 4},
    {"LAMASSU_SAPPHIRE", "\xBE\xF3\x01\x00\x00"sv, VTABLE_OFFSET::MONS_LAMASSU, 3, 1},
    {"LAMASSU_RUBY", "\xBD\xF4\x01\x00\x00"sv, VTABLE_OFFSET::MONS_LAMASSU, 3, 1},
    {"LAMASSU_EMERALD", "\xBA\xF2\x01\x00\x00"sv, VTABLE_OFFSET::MONS_LAMASSU, 3, 1}, // there is a second line with 1F2 value, not sure if that the type as well
    {"LAMASSU_DIAMOND", "\xB8\xF1\x01\x00\x00"sv, VTABLE_OFFSET::MONS_LAMASSU, 3, 1},
    {"DUATALTAR_BOMBBAG", "\xBE\x01\x02\x00\x00"sv, VTABLE_OFFSET::FLOOR_DUAT_ALTAR, 36, 1},
    {"DUATALTAR_BOMBBOX", "\xBE\x02\x02\x00\x00"sv, VTABLE_OFFSET::FLOOR_DUAT_ALTAR, 36, 1},
    {"DUATALTAR_COOKEDTURKEY", "\xBE\x06\x02\x00\x00"sv, VTABLE_OFFSET::FLOOR_DUAT_ALTAR, 36, 1},
    /// Also LION_TRAP
    {"FLOORSTYLEDCOG_NUGGET", "\xBA\xF5\x01\x00\x00"sv, VTABLE_OFFSET::FLOORSTYLED_COG, 3, 1},
    /// COG
    {"CRUSHTRAP_NUGGET", "\xBA\xF5\x01\x00\x00"sv, VTABLE_OFFSET::ACTIVEFLOOR_CRUSH_TRAP_LARGE, 3, 1},
    /// COG
    {"SLIDINGWALL_NUGGET", "\xBA\xF5\x01\x00\x00"sv, VTABLE_OFFSET::ACTIVEFLOOR_SLIDINGWALL, 3, 1},
    /// Can spawn this + 5 (for NUGGET_SMALL)
    {"EMBED_NUGGET", "\x81\xC2\xF5\x01\x00\x00"sv, VTABLE_OFFSET::EMBED_GOLD, 3, 2},
    /// Can be +1 for SAPPHIRE_SMALL
    {"CHEST_SMALLEMERALD", "\xBA\xF7\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    {"CHEST_EMERALD", "\xBA\xF2\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    {"CHEST_SAPPHIRE", "\xBA\xF3\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    {"CHEST_RUBY", "\xBA\xF4\x01\x00\x00\xEB"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    {"CHEST_SMALLRUBY", "\xBA\xF9\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    {"CHEST_LEPRECHAUN", "\xBA\x36\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    {"CHEST_BOMB", "\xBA\x5B\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    /// Game does this (this value | 0x1) to get SAPPHIRE (so depending on the chosen ENT_TYPE it can be + 1 or + 0)
    {"VAULTCHEST_EMERALD", "\x81\xCA\xF2\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 2},
    {"VAULTCHEST_DIAMOND", "\xBA\xF1\x01\x00\x00\xEB"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    {"VAULTCHEST_RUBY", "\xBA\xF4\x01\x00\x00\x48"sv, VTABLE_OFFSET::ITEM_CHEST, 3, 1},
    {"LOCKEDCHEST_UDJATEYE", "\xBA\x15\x02\x00\x00"sv, VTABLE_OFFSET::ITEM_LOCKEDCHEST, 26, 1},
    {"USHABTI_QILIN", "\xBA\x87\x03\x00\x00"sv, VTABLE_OFFSET::ITEM_USHABTI, 77, 1},
    {"POTOFGOLD_GOLDCOIN", "\xBA\xF6\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_POTOFGOLD, 88, 1}, // 88 is just the closest function
    /// Cutscene after beating hundun
    {"CUTSCENE_GOLDCOIN", "\xBA\xF6\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 1}, // this relies on the fact that this it is the first pattern
    /// Can spawn 0..3
    {"REDLANTERN_SMALLNUGGET", "\xBA\xFA\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_REDLANTERN, 3, 1},
    /// Has a chance to be +5 (NUGGET_SMALL)
    {"CANDLE_NUGGET", "\x81\xC2\xF5\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_PALACE_CANDLE, 3, 2},
    {"COOKFIRE_TORCH", "\xBA\xA0\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_COOKFIRE, 3, 1},
    {"SKULLDROPTRAP_SKULL", "\xBA\xEA\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"COFFIN_SKULL", "\xBA\xE2\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_COFFIN, 88, 1},
    /// Game does this (this value | 0x1) to get BOMBBAG (so depending on the chosen ENT_TYPE it can be + 1 or + 0)
    {"OLMEC_SISTERS_ROPEPILE", "\x0D\x00\x02\x00\x00\x83\xFB\x03"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"OLMEC_SISTERS_BOMBBOX", "\xBA\x02\x02\x00\x00\x0F\x45\xD0"sv, VTABLE_OFFSET::NONE, 0, 1},
    /// Challenge rewards:
    {"CHALLENGESTAR_CLONEGUN", "\xB8\x4D\x02\x00\x00"sv, VTABLE_OFFSET::LOGIC_TUN_STAR_CHALLENGE, 1, 1},
    {"CHALLENGESTAR_ELIXIR", "\xBD\x08\x02\x00\x00"sv, VTABLE_OFFSET::LOGIC_TUN_STAR_CHALLENGE, 1, 1},
    /// Game will change texture based on the active player unless the item search_flags/MASK is: FLOOR, DECORATION, BG, SHADOW, LOGICAL, WATER, LAVA, or the 16th bit that is normally unused
    {"CHALLENGESUN_PLAYERBAG", "\xBA\x1F\x02\x00\x00"sv, VTABLE_OFFSET::LOGIC_TUN_SUN_CHALLENGE, 1, 1},
    /// Mattock and Arrow of Light are build into the level, use level editor or `set_pre_entity_spawn` to replace them

    ///
    /// Attacks:
    ///

    /// Needs to be an arrow, otherwise it may crash the game on spawn or interaction
    {"ARROWTRAP_WOODENARROW", "\xBA\x73\x01\x00\x00"sv, VTABLE_OFFSET::FLOOR_ARROW_TRAP, 40, 1},
    /// Needs to be an arrow, otherwise it may crash the game on spawn or interaction, arrow can still be poisoned after spawn
    {"POISONEDARROWTRAP_WOODENARROW", "\xBA\x73\x01\x00\x00"sv, VTABLE_OFFSET::FLOOR_POISONED_ARROW_TRAP, 40, 1},
    {"LASERTRAP_SHOT", "\xBA\x7E\x01\x00\x00"sv, VTABLE_OFFSET::FLOOR_LASER_TRAP, 23, 1},
    {"ANUBIS2_ANUBIS_COFFIN", "\xBA\xC5\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"MUMMY_FLY", "\xB4\x01\x00\x00"sv, VTABLE_OFFSET::LOGICAL_MUMMYFLIES_SOUND_SOURCE, 2},
    {"GIANTSPIDER_WEBSHOT", "\xBA\x70\x01\x00\x00"sv, VTABLE_OFFSET::MONS_GIANTSPIDER, 78, 1},
    {"OLMEC_BOMB", "\x5B\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_OLMECCANNON_BOMBS, 93},
    {"OLMEC_UFO", "\x0A\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_OLMECCANNON_UFO, 93},
    {"YETIKING_ICESPIRE", "\xE8\x01\x00\x00"sv, VTABLE_OFFSET::MONS_YETIKING, 104},
    /// Anubis regular shot = new_drop_uid - 1 !
    {"SCEPTER_ANUBISSPECIALSHOT", "\x79\x01\x00\x00"sv, VTABLE_OFFSET::MONS_ANUBIS, 101}, // should probably be 102
    {"SCEPTER_PLAYERSHOT", "\x7A\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_SCEPTER, 24},
    {"PLASMACANNON_SHOT", "\xBA\x77\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_PLASMACANNON, 24, 1},
    {"FREEZERAY_SHOT", "\xBA\xA9\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_FREEZERAY, 24, 1},
    {"CLONEGUN_SHOT", "\xBA\xAA\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_CLONEGUN, 24, 1},
    /// It spawns 6 bullets
    {"SHOTGUN_BULLET", "\xBA\xA8\x01\x00\x00"sv, VTABLE_OFFSET::ITEM_SHOTGUN, 24, 1},
    {"UFO_LASERSHOT", "\xBA\x7B\x01\x00\x00"sv, VTABLE_OFFSET::MONS_UFO, 75, 1}, // 75 is just the closest function
    {"LAMASSU_LASERSHOT", "\xBA\x7C\x01\x00\x00"sv, VTABLE_OFFSET::MONS_LAMASSU, 78, 1},
    {"SORCERESS_DAGGERSHOT", "\xBA\x7D\x01\x00\x00"sv, VTABLE_OFFSET::MONS_SORCERESS, 78, 1},
    {"TIAMAT_TIAMATSHOT", "\xBA\x80\x01\x00\x00"sv, VTABLE_OFFSET::MONS_TIAMAT, 78, 1},
    {"ROCKDOG_FIREBALL", "\xBA\x81\x01\x00\x00"sv, VTABLE_OFFSET::MOUNT_ROCKDOG, 75, 1}, // 75 is just the closest function
    {"QILIN_FIREBALL", "\xBA\x81\x01\x00\x00"sv, VTABLE_OFFSET::MOUNT_QILIN, 75, 1},     // 75 is just the closest function
    {"AXOLOTL_BUBBLE", "\xBA\xC8\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"HUNDUN_FIREBALL", "\xBA\x82\x01\x00\x00"sv, VTABLE_OFFSET::MONS_HUNDUN_BIRDHEAD, 78, 1},
    {"FIREBUG_FIREBALL", "\xBA\x83\x01\x00\x00"sv, VTABLE_OFFSET::MONS_FIREBUG, 75, 1}, // 75 is just the closest function
    {"COBRA_ACIDSPIT", "\xBA\x85\x01\x00\x00"sv, VTABLE_OFFSET::MONS_COBRA, 78, 1},
    {"OCTOPUS_INKSPIT", "\xBA\x86\x01\x00\x00"sv, VTABLE_OFFSET::MONS_OCTOPUS, 78, 1},
    {"HERMITCRAB_ACIDBUBBLE", "\xBA\x87\x01\x00\x00\x0F"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"PANGXIE_ACIDBUBBLE", "\xBA\x88\x01\x00\x00"sv, VTABLE_OFFSET::MONS_CRABMAN, 78, 1},
    {"ALIENQUEEN_ALIENBLAST", "\xBA\xB6\x02\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 1},
    /// ALIENBLAST_RETICULE_INTERNAL
    {"ALIENQUEEN_ALIENBLAST_RI", "\xBA\xB4\x02\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 1},
    /// ALIENBLAST_RETICULE_EXTERNAL
    {"ALIENQUEEN_ALIENBLAST_RE", "\xBA\xB5\x02\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 1},
    {"CATMUMMY_CURSINGCLOUD", "\xBA\xB8\x01\x00\x00"sv, VTABLE_OFFSET::MONS_CATMUMMY, 3, 1},
    /// ANUBIS_SPECIAL_SHOT_RETICULE
    {"ANUBIS_SPECIALSHOT_R", "\xBA\xDF\x02\x00\x00\xE8****\x48"sv, VTABLE_OFFSET::NONE, 0, 1},
    /// ANUBIS_SPECIAL_SHOT_RETICULE
    {"ANUBIS2_SPECIALSHOT_R", "\xBA\xDF\x02\x00\x00\xE8****\x8B"sv, VTABLE_OFFSET::NONE, 0, 1},

    ///
    /// Spawn:
    ///

    /// It set's move_state for them for sleep and wake_up_timer, so i has to be movable and entity offset 0x150 can't be something important
    {"COOKFIRE_CAVEMAN_1", "\xBA\xE1\x00\x00\x00\x0F\x28\xD7\xE8****\x48\x89\xC3"sv, VTABLE_OFFSET::ITEM_COOKFIRE, 75, 1},
    {"COOKFIRE_CAVEMAN_2", "\xBA\xE1\x00\x00\x00\x0F\x28\xD7\xE8****\x48\x89\xC6"sv, VTABLE_OFFSET::ITEM_COOKFIRE, 75, 1},
    {"BONEBLOCK_SKELETON", "\xBA\xE3\x00\x00\x00"sv, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 75, 1},
    {"BONEBLOCK_SKULL", "\xBA\xE2\x01\x00\x00"sv, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 75, 1},
    {"BONEBLOCK_BONES", "\xBA\xE3\x01\x00\x00"sv, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 75, 1},
    {"GIANTSPIDER_WEB_LEFT", "\xBA\x6E\x01\x00\x00\xE8****\x8B"sv, VTABLE_OFFSET::MONS_GIANTSPIDER, 75, 1},
    {"GIANTSPIDER_WEB_RIGHT", "\xBA\x6E\x01\x00\x00\xE8****\x48"sv, VTABLE_OFFSET::MONS_GIANTSPIDER, 75, 1},
    {"IMP_LAVAPOT", "\xBA\xE5\x01\x00\x00"sv, VTABLE_OFFSET::MONS_IMP, 75, 1},
    // {"MECH_JETPACK_MECH", "\xBA\x38\x02\x00\x00"sv, VTABLE_OFFSET::MOUNT_MECH, 75, 1}, // hoverpack actually works (lol), but still everything crashes on mech death
    {"ICE_ALIVE_EMBEDDED_ON_ICE", "\xBA\xCF\x01\x00\x00\xE8"sv, VTABLE_OFFSET::NONE, 0, 1, 2}, // two of them for some reason
    /// Most items that don't normally spawn in floor will be invisible and can have broken physics when the floor is destroyed
    {"FLOOR_EMERALD", "\xC6\x44\x24\x30\x00\xBA\xF2\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    /// Most items that don't normally spawn in floor will be invisible and can have broken physics when the floor is destroyed
    {"FLOOR_SAPPHIRE", "\xC6\x44\x24\x30\x00\xBA\xF3\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    /// Probably for the FLOOR_EMPRESS_GRAVE
    /// Most items that don't normally spawn in floor will be invisible and can have broken physics when the floor is destroyed
    {"FLOOR_DIAMOND", "\xC6\x44\x24\x30\x00\xBA\xF1\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    /// Most items that don't normally spawn in floor will be invisible and can have broken physics when the floor is destroyed
    {"FLOOR_RUBY", "\xC6\x44\x24\x30\x00\xBA\xF4\x01\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"FLOOR_EMBED_GOLD", "\xC6\x44\x24\x30\x00\xBA\xBE\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},
    {"FLOOR_EMBED_GOLD_BIG", "\xC6\x44\x24\x30\x00\xBA\xBF\x00\x00\x00"sv, VTABLE_OFFSET::NONE, 0, 6},

    ///
    /// Special:
    ///

    /// Change item used to open the door (still requires hedjet, crown or eggplant crown)
    {"DOOR_COG_SCEPTER", "\x81\x78\x14\x4C\x02\x00\x00"sv, VTABLE_OFFSET::FLOOR_DOOR_COG, 25, 3, 2}, // is not the exact virtual, but function that the virtual uses
    /// It's not ENT_TYPE but amount of health, with elixir it will be this value * 2
    {"MOTHERSTATUE_HEALTH", "\xBD\x04\x00\x00\x00\xD3"sv, VTABLE_OFFSET::FLOOR_MOTHER_STATUE, 2, 1},
    /// It's not ENT_TYPE but amount of health, with elixir it will be this value * 2
    {"COOKEDTURKEY_HEALTH", "\xBA\x01\x00\x00\x00\x48\x0F\xBA\xE1\x2E\xBD"sv, VTABLE_OFFSET::ITEM_PICKUP_GIANTFOOD, 94, 1},
    /// It's not ENT_TYPE but amount of health, with elixir it will be this value * 2
    {"GIANTFOOD_HEALTH", "\xBD\x0A\x00\x00\x00\xD3"sv, VTABLE_OFFSET::ITEM_PICKUP_GIANTFOOD, 94, 1},
    /// It's not ENT_TYPE but amount of health, with elixir it will be this value * 2
    {"ROYALJELLY_HEALTH", "\xBD\x06\x00\x00\x00\xD3"sv, VTABLE_OFFSET::ITEM_PICKUP_GIANTFOOD, 94, 1},
    /// It's not ENT_TYPE but amount of health, with elixir it will be this value * 2
    {"KAPALA_HEALTH", "\xBA\x01\x00\x00\x00\xB8\x01"sv, VTABLE_OFFSET::NONE, 0, 1},
    /* can't do elixir as there are some calculations for cursed, poisoned etc.
    can't do pet, it has some complex calculation for some reason
    can't do ankh, made a separate function for that (see modify_ankh_health_gain)
    can't do initial health (camp, level, duat, coffin) as it's a word/byte
    can't do drops for: humphead, yetiking, yetiqueen, alien queen, pangxie (gems) those are stored in array, need special function for that
    */

    /// maybe TODO: if someone wants all the explosions (from damage/death/crush), could also be added
};

std::vector<DropChanceEntry> dropchance_entries{
    {"BONEBLOCK_SKELETONKEY", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::ACTIVEFLOOR_BONEBLOCK, 3},
    {"CROCMAN_TELEPACK", "\x64"sv, VTABLE_OFFSET::MONS_CROCMAN, 3, 1},
    {"HANGINGSPIDER_WEBGUN", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::MONS_HANGSPIDER, 3},
    {"JIANGSHIASSASSIN_SPIKESHOES", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::MONS_FEMALE_JIANGSHI, 3},
    {"JIANGSHI_SPRINGSHOES", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::MONS_JIANGSHI, 3},
    {"MOLE_MATTOCK", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::MONS_MOLE, 3},
    {"MOSQUITO_HOVERPACK", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::MONS_MOSQUITO, 3},
    {"ROBOT_METALSHIELD", "\x64"sv, VTABLE_OFFSET::MONS_ROBOT, 3, 1},
    {"SKELETON_SKELETONKEY", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::MONS_SKELETON, 3},
    {"UFO_PARACHUTE", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::MONS_UFO, 78},
    {"YETI_PITCHERSMITT", "\xE8\x03\x00\x00"sv, VTABLE_OFFSET::MONS_YETI, 3},
};

void set_drop_chance(int32_t dropchance_id, uint32_t new_drop_chance)
{
    if (dropchance_id < (int32_t)dropchance_entries.size())
    {
        if (dropchance_id < 0)
        {
            if (dropchance_id == -1)
                recover_mem("drop_chance");
            return;
        }
        auto& entry = dropchance_entries.at(dropchance_id);
        if (entry.offset == 0)
        {
            auto& memory = Memory::get();
            size_t offset = memory.at_exe(find_inst(memory.exe(), entry.pattern, get_virtual_function_address(entry.vtable_offset, entry.vtable_rel_offset)));
            if (offset > memory.exe_address())
            {
                entry.offset = offset;
            }
        }

        if (entry.offset != 0)
        {
            if (entry.chance_sizeof == 4)
            {
                write_mem_recoverable("drop_chance", entry.offset, new_drop_chance, true);
            }
            else if (entry.chance_sizeof == 1)
            {
                uint8_t value = static_cast<uint8_t>(new_drop_chance);
                write_mem_recoverable("drop_chance", entry.offset, value, true);
            }
        }
    }
}

void replace_drop(int32_t drop_id, ENT_TYPE new_drop_entity_type)
{
    const static auto nof_ent_types = to_id("ENT_TYPE_LIQUID_COARSE_LAVA") + 1;
    if (drop_id < (int32_t)drop_entries.size() && new_drop_entity_type < nof_ent_types)
    {
        if (drop_id < 0)
        {
            if (drop_id == -1)
                recover_mem("replace_drop");

            return;
        }
        auto& entry = drop_entries.at(drop_id);
        if (new_drop_entity_type == 0)
        {
            for (int x = 0; x < 3; ++x)
                if (entry.offsets[x])
                    recover_mem("replace_drop", entry.offsets[x]);

            return;
        }
        if (entry.offsets[0] == 0)
        {
            auto& memory = Memory::get();
            size_t offset = 0;
            const auto drop_name{"DROP." + entry.caption};

            if (entry.vtable_offset == VTABLE_OFFSET::NONE)
                offset = memory.after_bundle_address();
            else
                offset = get_virtual_function_address(entry.vtable_offset, entry.vtable_rel_offset);

            if (offset == 0)
                return;

            int x = 0;
            do
            {
                offset = find_inst(memory.exe(), entry.pattern, offset, std::nullopt, drop_name);
                if (offset == 0)
                    return;

                entry.offsets[x] = memory.at_exe(offset + entry.value_offset);

                offset += entry.pattern.size();
                ++x;
            } while (x < entry.vtable_occurrence && x < 3);
        }

        if (entry.offsets[0] != 0)
        {
            for (auto x = 0; x < entry.vtable_occurrence; ++x)
            {
                write_mem_recoverable("replace_drop", entry.offsets[x], new_drop_entity_type, true);
            }
        }
    }
}

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
