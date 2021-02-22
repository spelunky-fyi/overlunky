meta.name = "Pot Randomizer"
meta.version = "WIP"
meta.description = "Random container contents and player stats."
meta.author = "Dregu"

register_option_int("max_health", "Max starting health", 20, 4, 99)
register_option_int("max_bombs", "Max starting bombs", 20, 4, 99)
register_option_int("max_ropes", "Max starting ropes", 20, 4, 99)
items = {220,221,222,223,224,225,227,228,229,230,231,233,234,237,238,239,240,242,243,245,246,247,248,250,251,252,260,261,262,263,264,266,267,268,272,273,275,276,277,278,280,283,284,286,287,289,295,296,297,298,300,301,302,303,304,305,306,307,308,309,310,312,317,318,319,320,321,322,323,331,332,333,334,336,337,338,339,340,341,347,348,356,357,358,365,366,371,372,373,374,377,395,396,399,400,401,402,409,416,422,428,429,435,436,439,440,442,444,448,453,456,457,462,469,475,476,477,478,479,480,481,482,490,491,492,493,494,495,496,497,498,499,500,501,506,507,508,509,510,511,512,513,514,515,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,536,557,558,560,563,565,567,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,585,592,593,596,604,610,630,631,884,885,886,887,888}
tools = {374,422,509,510,511,512,513,514,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,534,536,557,558,560,563,565,567,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,585}

function init_run()
  for i,player in ipairs(players) do
    player.health = math.random(4, options.max_health)
    player.inventory.bombs = math.random(4, options.max_bombs)
    player.inventory.ropes = math.random(4, options.max_ropes)
  end
end

set_callback(function()
  if state.level_count == 0 then
    init_run()
  end
  
  set_interval(function()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    -- randomize pots
    pots = get_entities_at(ENT_TYPE.ITEM_POT, 0, x, y, l, 10)
    for i,v in ipairs(pots) do
      item = items[math.random(#items)]
      set_contents(v, item)
    end
    
    -- randomize crates
    crates = get_entities_at(ENT_TYPE.ITEM_CRATE, 0, x, y, l, 10)
    for i,v in ipairs(crates) do
      item = tools[math.random(#tools)]
      set_contents(v, item)
    end
    
    -- randomize coffins
    coffins = get_entities_at(ENT_TYPE.ITEM_COFFIN, 0, x, y, l, 10)
    for i,v in ipairs(coffins) do
      item = math.random(194, 216)
      if item == 214 then
        item = 215
      end
      set_contents(v, item)
    end
  end, 15)
end, ON.LEVEL)

message("Initialized")
