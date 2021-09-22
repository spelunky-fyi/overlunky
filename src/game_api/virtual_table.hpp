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
    ACTIVEFLOOR_BONEBLOCK = 10659,
    FLOOR_ALTAR = 39262,
    ITEM_CURSEDPOT = 42232,
    ITEM_LIT_WALLTORCH = 11570,
    ITEM_MATTOCK = 7058,
    ITEM_OLMECCANNON_BOMBS = 39169,
    ITEM_OLMECCANNON_UFO = 51617,
    ITEM_SCEPTER = 23690,
    ITEM_TIAMAT_SHOT = 47882,
    LOGICAL_BOULDERSPAWNER = 24909,
    LOGICAL_MUMMYFLIES_SOUND_SOURCE = 3696,
    MONS_ANUBIS = 2448,
    MONS_ANUBIS2 = 29263,
    MONS_CRABMAN = 41125,
    MONS_CROCMAN = 36778,
    MONS_FEMALE_JIANGSHI = 11050,
    MONS_GHOST = 23179,
    MONS_GIANTSPIDER = 6695,
    MONS_GOLDMONKEY = 36972,
    MONS_HANGSPIDER = 13272,
    MONS_HUNDUNS_SERVANT = 50400,
    MONS_JIANGSHI = 20179,
    MONS_KINGU = 27220,
    MONS_LEPRECHAUN = 20445,
    MONS_MERCHANT = 25654,
    MONS_MOLE = 51154,
    MONS_MOSQUITO = 29779,
    MONS_MUMMY = 31984,
    MONS_NECROMANCER = 9436,
    MONS_OLD_HUNTER = 38439,
    MONS_QUEENBEE = 43771,
    MONS_ROBOT = 21295,
    MONS_SKELETON = 6219,
    MONS_SORCERESS = 6070,
    MONS_THIEF = 12774,
    MONS_TIAMAT = 7243,
    MONS_UFO = 20964,
    MONS_VAMPIRE = 27499,
    MONS_VLAD = 40299,
    MONS_YETI = 32569,
    MONS_YETIKING = 49600,
    MONS_YETIQUEEN = 2141,
    MOUNT_TURKEY = 4966,
    CHAR_ANA_SPELUNKY = 46816,
};

size_t get_virtual_function_address(VTABLE_OFFSET tableEntry, uint32_t relativeOffset);
