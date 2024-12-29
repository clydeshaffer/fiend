    .export _SineTable, _BossMap, _PauseScreen
    .export _HudSprites, _Tileset_00, _Tileset_01, _Tileset_02, _Tileset_03
    .export _HeroSprites, _HeroFrames, _DynaWave, _TitleMusic, _BossMusic, _BossMusic2, _EndMusic
    .export _DiedMusic, _StairsMusic, _PickupMusic, _FanfareMusic, _MapItemMusic
    .export _ConsoleJingle
    .export _MainMusic, _SecondMusic, _ThirdMusic, _FourthMusic
    .export _EnemyFrames_RAT, _EnemyFrames_BAT, _EnemyFrames_SPIDER, _EnemyFrames_ORC, _EnemyFrames_SNIPER, _EnemyFrames_ARROW
    .export _EnemySprites_RAT, _EnemySprites_BAT, _EnemySprites_SPIDER, _EnemySprites_ORC, _EnemySprites_SNIPER, _EnemySprites_ARROW
    .export _EnemyFrames_GHOST, _EnemyFrames_FIREBALL
    .export _EnemySprites_GHOST, _EnemySprites_FIREBALL
    .export _EnemySprites_SKELETON_0, _EnemySprites_SKELETON_1, _EnemySprites_SKELETON_2, _EnemySprites_SKELETON_3
    .export _EnemyFrames_SKELETON
    .export _EnemySprites_CULTIST_0, _EnemySprites_CULTIST_1, _EnemySprites_CULTIST_2, _EnemySprites_CULTIST_3, _EnemyFrames_CULTIST
    .export _EnemyFrames_BAT2, _EnemySprites_BAT2
    .export _EpilogueText_0, _EpilogueText_1

    .segment "COMMON"

_SineTable:
	.byte 128,131,134,137,140,143,146,149,152,155,158,162,165,167,170,173
	.byte 176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215
	.byte 218,220,222,224,226,228,230,232,234,235,237,238,240,241,243,244
	.byte 245,246,248,249,250,250,251,252,253,253,254,254,254,255,255,255
	.byte 255,255,255,255,254,254,254,253,253,252,251,250,250,249,248,246
	.byte 245,244,243,241,240,238,237,235,234,232,230,228,226,224,222,220
	.byte 218,215,213,211,208,206,203,201,198,196,193,190,188,185,182,179
	.byte 176,173,170,167,165,162,158,155,152,149,146,143,140,137,134,131
	.byte 128,124,121,118,115,112,109,106,103,100,97,93,90,88,85,82
	.byte 79,76,73,70,67,65,62,59,57,54,52,49,47,44,42,40
	.byte 37,35,33,31,29,27,25,23,21,20,18,17,15,14,12,11
	.byte 10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0 
	.byte 0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9
	.byte 10,11,12,14,15,17,18,20,21,23,25,27,29,31,33,35
	.byte 37,40,42,44,47,49,52,54,57,59,62,65,67,70,73,76
	.byte 79,82,85,88,90,93,97,100,103,106,109,112,115,118,121,124

_ConsoleJingle:
    .incbin "jingle.gtm2"

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

_BossMusic:
    .incbin "boss_loop.gtm2"

_BossMusic2:
    .incbin "boss_loop_p2.gtm2"

_EndMusic:
    .incbin "end.gtm2"

_PickupMusic:
    .incbin "pickup.gtm2"

_FanfareMusic:
    .incbin "fanfare.gtm2"

_MapItemMusic:
    .incbin "map.gtm2"

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
    
_EnemyFrames_SKELETON:
    .incbin "SkeletonMage_Red.gsi"

_EnemyFrames_CULTIST:
    .incbin "Cultist_Red.gsi"

_EnemyFrames_BAT2:
    .incbin "chara_bat2.gsi"

    .segment "LOADONCE"

_DynaWave:
    .incbin "audio_fw.bin.deflate"

_HudSprites:
    .incbin "hud_graphics.gtg.deflate"

_HeroSprites:
    .incbin "chara_hero.gtg.deflate"

_PauseScreen:
    .incbin "pause.gtg.deflate"

    .segment "TILES"

_Tileset_00:
    .incbin "tileset_00.gtg.deflate"

_Tileset_01:
    .incbin "tileset_01.gtg.deflate"

_Tileset_02:
    .incbin "tileset_02.gtg.deflate"

_Tileset_03:
    .incbin "tileset_03.gtg.deflate"

_BossMap:
    .incbin "bossmap.gtg.deflate"

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

_EnemySprites_SKELETON_0:
    .incbin "SkeletonMage_Red.gtg.deflate"

_EnemySprites_SKELETON_1:
    .incbin "SkeletonMage_Red.gtg.1.deflate"

_EnemySprites_SKELETON_2:
    .incbin "SkeletonMage_Red.gtg.2.deflate"

_EnemySprites_SKELETON_3:
    .incbin "SkeletonMage_Red.gtg.3.deflate"

    .segment "MONSTERS3"

_EnemySprites_CULTIST_0:
    .incbin "Cultist_Red.gtg.deflate"

_EnemySprites_CULTIST_1:
    .incbin "Cultist_Red.gtg.1.deflate"

_EnemySprites_CULTIST_2:
    .incbin "Cultist_Red.gtg.2.deflate"

_EnemySprites_CULTIST_3:
    .incbin "Cultist_Red.gtg.3.deflate"

_EnemySprites_BAT2:
    .incbin "chara_bat2.gtg.deflate"

    .segment "EPILOGUE"
_EpilogueText_0:
    .incbin "end_text.gtg.deflate"
_EpilogueText_1:
    .incbin "end_text.gtg.1.deflate"