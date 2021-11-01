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
    ACTIVEFLOOR_BONEBLOCK = 6592,
    FLOOR_ALTAR = 9146,
    ITEM_CURSEDPOT = 31368,
    ITEM_LIT_WALLTORCH = 30892,
    ITEM_MATTOCK = 24482,
    ITEM_OLMECCANNON_BOMBS = 3960,
    ITEM_OLMECCANNON_UFO = 3866,
    ITEM_SCEPTER = 24198,
    ITEM_TIAMAT_SHOT = 23154,
    LOGICAL_BOULDERSPAWNER = 1662,
    LOGICAL_MUMMYFLIES_SOUND_SOURCE = 1890,
    MONS_ANUBIS = 43016,
    MONS_ANUBIS2 = 42504,
    MONS_CRABMAN = 37608,
    MONS_CROCMAN = 43634,
    MONS_FEMALE_JIANGSHI = 42300,
    MONS_GHOST = 37916,
    MONS_GIANTSPIDER = 45994,
    MONS_GOLDMONKEY = 37814,
    MONS_HANGSPIDER = 46098,
    MONS_HUNDUNS_SERVANT = 38128,
    MONS_JIANGSHI = 42402,
    MONS_KINGU = 40546,
    MONS_LEPRECHAUN = 37710,
    MONS_MERCHANT = 39000,
    MONS_MOLE = 45276,
    MONS_MOSQUITO = 44658,
    MONS_MUMMY = 43120,
    MONS_NECROMANCER = 43222,
    MONS_OLD_HUNTER = 38568,
    MONS_QUEENBEE = 40750,
    MONS_ROBOT = 44350,
    MONS_SKELETON = 45582,
    MONS_SORCERESS = 43428,
    MONS_THIEF = 38458,
    MONS_TIAMAT = 40444,
    MONS_UFO = 41788,
    MONS_VAMPIRE = 43840,
    MONS_VLAD = 43738,
    MONS_YETI = 41582,
    MONS_YETIKING = 41476,
    MONS_YETIQUEEN = 41370,
    MOUNT_TURKEY = 1136,
    CHAR_ANA_SPELUNKY = 47490,
};

size_t get_virtual_function_address(VTABLE_OFFSET tableEntry, uint32_t relativeOffset);
