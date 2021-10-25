meta.name = "Strings test"
meta.version = "WIP"
meta.description = "Changes all death messages and shopkeeper names to new one. Disables ghost toast. Changes responses in the camp and Yang's. One item i every shop is Garbage"
meta.author = "Mr Auto"

death_hashes = {
0xd646180e, -- I broke every bone in my body.
0xce96fc57, -- I haven't reached the bottom yet...
0xb098b089, -- I teleported right into a wall!
0x6d246cd8, -- Ooh, my insides hurt...
0x970a96f8, -- An alien bumped me a little too hard.
0xcda27397, -- The queen of the aliens melted my brain.
0x45e8ca32, -- An ammit caused me to die a second time.
0x2a8b3bd5, -- Anubis ripped my soul from its prison of flesh.
0x02a8c5a5, -- Anubis took great pleasure in erasing me from the underworld.
0x4a2c091a, -- The snake god made a snack out of my soul.
0x9bc56ef4, -- I was nibbled to death by a bat.
0x1aa69dc3, -- I must be allergic to bees...
0xb7675db8, -- A mummy cat decided she didn't like me.
0x43784d32, -- A caveman pummeled me to death.
0x38db11f3, -- Quillback showed me the full extent of his primitive strength!
0x80f31a2c, -- I succumbed to a cobra's venom.
0x87e434ac, -- I couldn't crack a crab man's shell.
0xfed65a87, -- A crocman tore a big chunk out of me.
0xa699e629, -- A jiang-shi assassin snapped my neck.
0xb7320380, -- A firebug roasted me alive.
0xde0d02be, -- A fire frog turned me into a lily pad.
0xc12de33f, -- The fisher became the fished...
0xb709a2e8, -- A frog turned me into a lily pad.
0x370467a4, -- Where am I? I can't feel my body...
0x9896afc7, -- I was crushed in the jaws of a giant clam.
0xd536257b, -- The Great Humphead's appetite was temporarily sated.
0xa33b417e, -- A giant fly grossed me out... to death!
0x996fb53a, -- A giant frog found me annoying.
0x036d846d, -- A giant spider preyed on me.
0xb3a0186c, -- I can still feel it wriggling all over me! Yuck!
0xe8cd3dd6, -- A spider bite ended my life.
0xb8d823d2, -- That hermit crab was a bit crabby.
0xb0a2c39c, -- A horned lizard finished me off.
0x75bb5c12, -- I succumbed to Hundun's dream.
0xc879d1a3, -- An imp finished me off.
0xc90c360e, -- A jiang-shi hopped on me.
0xa954e7a6, -- A lamassu tap danced on me.
0x5ae314da, -- A lavamander had me for dinner.
0x5d458f25, -- I was melted by a magma man.
0x446f1b7b, -- I was devoured by a mantrap.
0xe8af4546, -- An astral jellyfish transformed me into pure energy.
0xd78fc49e, -- I shouldn't have angered her!
0xcb9ac90e, -- A mole chewed a hole right through me.
0x7637fdc1, -- A mosquito sucked out the last few drops of my blood.
0x934f3ae6, -- A tomb lord took a thousand-year grudge out on me.
0x95563f56, -- I became another undead pawn for a necromancer.
0x5e998192, -- An octopus gave me eight reasons to feel bad about myself.
0xcebda5cd, -- Olmec proved why he's still the boss.
0xb0a43aac, -- An olmite made a big brain play and I lost.
0xb634bb4d, -- Osiris found me unworthy of joining him in the afterlife.
0xd419e4f4, -- That prototype shopkeeper was still pretty effective at killing me!
0x4ff907cb, -- A queen bee sentenced me to death.
0x4139d18f, -- A robot ended my life without a hint of emotion.
0x25914054, -- A rock dog went straight for my jugular.
0x4d42890f, -- A scorpion stung me to death.
0x0c69601f, -- Maybe I shouldn't have angered that shopkeeper...
0x6d7c0e96, -- A skeleton ripped me apart.
0xb3e93943, -- A snake bit me.
0xe8f77695, -- A sorceress ended me with a powerful spell.
0x42e0565a, -- A spider bite ended my life.
0x4ad038b6, -- Despite its cute looks, that tadpole took my life!
0xd0a0af42, -- I failed to meet the expectations of the Mother Goddess.
0x7331200d, -- A tikiman pummeled me to death.
0xf7372a42, -- A UFO took my life.
0xa11c2855, -- I can feel my humanity draining away from the vampire's bite...
0xf9c4063b, -- I can feel my humanity draining away from Vlad's bite...
0xcd3761ad, -- A witch doctor has ended my life.
0xd175f616, -- I succumbed to a witch doctor's curse.
0x7909049b, -- A yeti made me its plaything.
0x1d795652, -- The Yeti King is using me as a cushion now.
0xb89c79fa, -- The Yeti Queen is wearing me as a scarf now.
0x08ef8d49, -- An arrow pierced a vital organ.
0xe92f2282, -- A giant spear punched a hole right through me.
0x48827b2d, -- Turns out I couldn't outrun a boulder.
0x4cc25d0c, -- A crush trap turned me into a pancake.
0x60d9fab4, -- I was crushed by the giant drill.
0x5bf0b681, -- I should have stood somewhere a little bit safer!
0x52bc07b7, -- An explosion sent me flying.
0x7313a73b, -- A laser trap caught me unaware.
0x8fb42b53, -- I was burnt to a crisp by some lava.
0x2d260171, -- A lion trap perforated me.
0xb021ca88, -- I was crushed by a falling platform.
0x8c02b73c, -- A powder keg blew me to smithereens.
0xa81ceb9e, -- I was crushed by a falling push block.
0x22901107, -- I was sacrificed to Kali.
0xd2232344, -- Strangely enough, my skull was cracked by another skull.
0xaa22d15d, -- I should have stood somewhere a little bit safer!
0x081c016f, -- I was caught in the jaws of death.
0x9f78d1cc, -- I had a hair-raising experience with a shock trap.
0xfe3dd8fe, -- A spear trap skewered me.
0x90bd2ea4, -- I was crushed by a spikeball trap.
0x9ff69164, -- I fell on some spikes.
0xfc83e106, -- I bled out against some sharp thorns.
0x29e02820, -- A totem trap punched my lights out.
0xa589a3c0, -- I was a victim of my own greed...
0x6bcb5e95, -- I met my untimely demise.
0x60d4ec4f, -- This might be harder than I thought!
0xf0741254, -- I'm learning a little bit more each time, though!
0xc2245030, -- You gotta admire my persistence! I know I'll get it eventually!
0x083047df, -- This is the furthest I've ever gotten!
0xeed28125, -- Wow, I got a new high score! That feels great!
0x296c45cb, -- I was so close to reaching the next area... maybe next time!
0x32993207, -- It's the most exquisite tomb in the Universe...
0x3b418250, -- As I sank into the eggplants, a feeling of calm washed over me...
0x39eb2a3a, -- It's okay! We just need more practice!
0x98c14b86, -- We were so close to reaching the next area... maybe next time!
0x5dce8fae, -- We're starting to get pretty good at working together.
0xae0fed01, -- We didn't make it, but we should be proud of how far we got together.
0xcab491f4, -- Tiamat defeated us! But we'll be victorious next time!
0x179984d5, -- We work together as if commanded by a single mind!
0x332e1b4b -- We succumbed to Chaos. But the struggle was worth it!
--0x96276d7c -- We're one with the Cosmos!
}

shopkeeper_names = {
{0x55618267, "Gustavo"}, -- Ali
{0x2b21d662, "Florida Man"}, -- Bob
{0x4bf2ecfd, "Juan"}, -- Cosmo
{0xeaf14d39, "Justin"}, -- Darwin
--{0x487c2057, "Ear"}, -- Ear , leave him be <3
{0x6df76a2b, "Doge"}, -- Earl
{0x62016f82, "Gabe"}, -- Frank
{0x4d827fea, "Chungus"}, -- Garth
{0xa2531c54, "Tem"}, -- Hakim
{0x98ac1767, "Chuck"}, -- Iggy
{0x91cdab81, "Bruce"}, -- Ivan
{0x467d71f2, "Impostor"}, -- Jimbo
{0xe8fa2f3d, "Ankha"}, -- Kinni
{0xa1926a93, "Toad"}, -- Lou
{0x95293801, "Pixel"}, -- Micky
{0x7bb85aef, "Fernando"}, -- Nacho
{0x05bf4315, "Surprise"}, -- Omar
{0x5b11fe53, "Kiki"}, -- Pancho
{0xaafea7d9, "Dante"}, -- Quincy
{0x800cd4e9, "Noah"}, -- Ron
{0xb09dcf02, "Evan"}, -- Sparky
{0xa79c425b, "Michael"}, -- Tarn
{0x4fe302fe, "James"}, -- Ulf
{0x9b358f60, "Ashton"}, -- Vern
{0x7357e408, "Xardas"}, -- Wang
{0xd425073f, "Keyaru"}, -- Willy
{0x72c3e92d, "ThrillFly"}, -- Xander
{0x11f92172, "MrBeast"}, -- Yoshi
{0x232d1fb3, "Subaru"} -- Ziggy
}

set_callback(function()

    str = get_string(hash_to_stringid(0x154b498f))
    -- always use hash_to_stringid to get text from the game itself
    -- the hex value can be found in the extracted strings00_hashed.str or
    -- https://github.com/spelunky-fyi/overlunky/blob/main/docs/game_data/strings00_hashed.str

    message("The string below will be text from journal in the language choosen in settings:")
    message(str)
    
    
    for _, hash in ipairs(death_hashes) do -- change every death message to "Just git gud lol"
    
        change_string(hash_to_stringid(hash), "Just git gud lol")
    end
    
    
    for _, result in ipairs(shopkeeper_names) do -- change shopkeeper names
    
        change_string(hash_to_stringid(result[1]), result[2])
    end

end, ON.LOAD)

set_callback(function()

    ents = get_entities_by(0, MASK.ITEM, LAYER.FRONT) -- get all items
    shop_items = {}
    for _, item in ipairs(ents) do
    
        if test_flag(get_entity_flags(item), ENT_FLAG.SHOP_ITEM) then -- if it's shop item
            shop_items[#shop_items + 1] = item; -- add to table
        end
    end
    
    random_shopitem = shop_items[math.random(#shop_items)] -- chose one at random
    
    add_custom_name(random_shopitem, "Garbage") -- change random item in shop to Garbage, this will only affect this one entity

end, ON.LEVEL)


set_callback(function(speaking_entity, text)

    if state.theme == THEME.BASE_CAMP then
    
        return "What are you looking at?" -- everyone in the camp will response with that message
    end

    if speaking_entity.type.id == ENT_TYPE.MONS_YANG and state.world == 1 then
        speaking_entity.is_patrolling = true
        return "Stupid birds, keep escaping, I had enough!"
    end

end, ON.SPEECH_BUBBLE)


set_callback(function(text)

    if text == "A terrible chill runs up your spine!" then -- this will only work when chosen language is English, unless you add all variants for all languages
        
        return "" -- ghost message won't be shown
    end

end, ON.TOAST)
