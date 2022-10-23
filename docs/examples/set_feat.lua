-- Change the strings for "Parenthood"
change_feat_string(FEAT.PARENTHOOD, "Eggman", "I am the eggman")
-- Change the strings for the first feat
change_feat_string(FEAT[FEAT_NAME[1]], "Walrus", "I am the walrus")


-- Option 1: You keep track of all the feats and tell them to the game when asked with a callback
-- Don't use together with set_feat
feats = {
  [FEAT.THE_FULL_SPELUNKY] = true,
  [FEAT[FEAT_NAME[32]]] = true
}
hook_get_feat(function(feat)
    return feats[feat] or false
end)


-- Option 2: The API keeps track of the feats, you just set them when they are achieved
-- Don't use together with hook_get_feat
reset_feats()
set_feat(FEAT.THE_FULL_SPELUNKY, true)
set_feat(FEAT[FEAT_NAME[32]], true)
