    .export _HudSprites, _Tileset_00, _Tileset_01, _Tileset_02, _Tileset_03
    .export _HeroSprites, _HeroFrames, _DynaWave, _TitleMusic, _DiedMusic, _StairsMusic
    .export _MainMusic, _SecondMusic, _ThirdMusic, _FourthMusic
    .export _EnemyFrames_RAT, _EnemyFrames_BAT, _EnemyFrames_SPIDER, _EnemyFrames_ORC, _EnemyFrames_SNIPER, _EnemyFrames_ARROW
    .export _EnemySprites_RAT, _EnemySprites_BAT, _EnemySprites_SPIDER, _EnemySprites_ORC, _EnemySprites_SNIPER, _EnemySprites_ARROW
    .export _EnemyFrames_GHOST, _EnemyFrames_FIREBALL
    .export _EnemySprites_GHOST, _EnemySprites_FIREBALL

    .segment "COMMON"

_MainMusic:
    .incbin "fiend_loop.gtm2"

_SecondMusic:
    .incbin "darker_loop.gtm2"

_ThirdMusic:
    .incbin "march_loop.gtm2"

_FourthMusic:
    .incbin "spooky_loop.gtm2"

_TitleMusic:
    .incbin "title.gtm2"

_DiedMusic:
    .incbin "died.gtm2"

_StairsMusic:
    .incbin "stairs.gtm2"

_HeroFrames:
    .incbin "chara_hero.gsi"

_EnemyFrames_SPIDER:
    .incbin "chara_spider.gsi"

_EnemyFrames_BAT:
    .incbin "chara_bat.gsi"

_EnemyFrames_RAT:
    .incbin "chara_rat.gsi"

_EnemyFrames_ORC:
    .incbin "chara_orc.gsi"

_EnemyFrames_SNIPER:
    .incbin "chara_goblin_sniper.gsi"
    
_EnemyFrames_ARROW:
    .incbin "projectile_arrow.gsi"

_EnemyFrames_GHOST:
    .incbin "chara_ghost.gsi"

_EnemyFrames_FIREBALL:
    .incbin "projectile_fireball.gsi"
    

    .segment "LOADONCE"

_DynaWave:
    .incbin "audio_fw.bin.deflate"

_HudSprites:
    .incbin "hud_graphics.gtg.deflate"

_HeroSprites:
    .incbin "chara_hero.gtg.deflate"

    .segment "TILES"

_Tileset_00:
    .incbin "tileset_00.gtg.deflate"

_Tileset_01:
    .incbin "tileset_01.gtg.deflate"

_Tileset_02:
    .incbin "tileset_02.gtg.deflate"

_Tileset_03:
    .incbin "tileset_03.gtg.deflate"

    .segment "MONSTERS"
    
_EnemySprites_SPIDER:
    .incbin "chara_spider.gtg.deflate"

_EnemySprites_BAT:
    .incbin "chara_bat.gtg.deflate"

_EnemySprites_RAT:
    .incbin "chara_rat.gtg.deflate"

_EnemySprites_ORC:
    .incbin "chara_orc.gtg.deflate"

_EnemySprites_SNIPER:
    .incbin "chara_goblin_sniper.gtg.deflate"

_EnemySprites_ARROW:
    .incbin "projectile_arrow.gtg.deflate"

    .segment "MONSTERS2"

_EnemySprites_GHOST:
    .incbin "chara_ghost.gtg.deflate"
    
_EnemySprites_FIREBALL:
    .incbin "projectile_fireball.gtg.deflate"