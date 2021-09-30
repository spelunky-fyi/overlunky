#pragma once

#include <cstdint>

// The numbers below represent the index in the memory block containing the virtual tables of all the objects
// This table can be found by following the pattern in the get_virtual_function_address function, which looks
// up the pointer to the first in the table.

// These numbers will change with every release of Spelunky. To look them up, you can use the x64dbg plugin:
// make sure you are in a level containing the entity you want to detect, click on Virtual Table -> Detect entities
// and search in the resulting list for the entity name

// Finding the relative offsets can be done by right-clicking somewhere in the function you are working in and choosing
// Spelunky2 > Lookup in virtual table. Then, choose the offset to the most sensible base entity type, e.g. MONS_MOLE +2
// if you're doing mole stuff.

enum class VTABLE_OFFSET
{
    NONE = 0,
    ACTIVEFLOOR_BONEBLOCK = 6574,
    FLOOR_ALTAR = 9124,
    ITEM_CURSEDPOT = 30960,
    ITEM_LIT_WALLTORCH = 30488,
    ITEM_MATTOCK = 24188,
    ITEM_OLMECCANNON_BOMBS = 3946,
    ITEM_OLMECCANNON_UFO = 3852,
    ITEM_SCEPTER = 23908,
    ITEM_TIAMAT_SHOT = 22882,
    LOGICAL_BOULDERSPAWNER = 1648,
    LOGICAL_MUMMYFLIES_SOUND_SOURCE = 1876,
    MONS_ANUBIS = 42410,
    MONS_ANUBIS2 = 41906,
    MONS_CRABMAN = 37090,
    MONS_CROCMAN = 43018,
    MONS_FEMALE_JIANGSHI = 41706,
    MONS_GHOST = 37392,
    MONS_GIANTSPIDER = 45332,
    MONS_GOLDMONKEY = 37292,
    MONS_HANGSPIDER = 45436,
    MONS_HUNDUNS_SERVANT = 37602,
    MONS_JIANGSHI = 41806,
    MONS_KINGU = 39982,
    MONS_LEPRECHAUN = 37190,
    MONS_MERCHANT = 38462,
    MONS_MOLE = 44628,
    MONS_MOSQUITO = 44022,
    MONS_MUMMY = 42514,
    MONS_NECROMANCER = 42614,
    MONS_OLD_HUNTER = 38036,
    MONS_QUEENBEE = 40182,
    MONS_ROBOT = 43720,
    MONS_SKELETON = 44928,
    MONS_SORCERESS = 42816,
    MONS_THIEF = 37928,
    MONS_TIAMAT = 39882,
    MONS_UFO = 41204,
    MONS_VAMPIRE = 43220,
    MONS_VLAD = 43120,
    MONS_YETI = 41002,
    MONS_YETIKING = 40896,
    MONS_YETIQUEEN = 40790,
    MOUNT_TURKEY = 1124,
    CHAR_ANA_SPELUNKY = 46816,
};

size_t get_virtual_function_address(VTABLE_OFFSET tableEntry, uint32_t relativeOffset);
