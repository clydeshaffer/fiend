    .export _GameSprites, _HeroSprites, _HeroFrames, _DynaWave, _TestMap, _MainMusic, _TitleMusic, _DiedMusic
    .export _EnemyFrames_RAT, _EnemyFrames_BAT, _EnemyFrames_SPIDER, _EnemyFrames_ORC
    .export _EnemySprites_RAT, _EnemySprites_BAT, _EnemySprites_SPIDER, _EnemySprites_ORC
    .RODATA
_GameSprites:
    .incbin "gamesprites.gtg.deflate"

_HeroSprites:
    .incbin "chara_hero.gtg.deflate"

_HeroFrames:
    .incbin "chara_hero.gsi"


;Enemy sprites
_EnemySprites_SPIDER:
    .incbin "chara_spider.gtg.deflate"

_EnemyFrames_SPIDER:
    .incbin "chara_spider.gsi"

_EnemySprites_BAT:
    .incbin "chara_bat.gtg.deflate"

_EnemyFrames_BAT:
    .incbin "chara_bat.gsi"

_EnemySprites_RAT:
    .incbin "chara_rat.gtg.deflate"

_EnemyFrames_RAT:
    .incbin "chara_rat.gsi"

_EnemySprites_ORC:
    .incbin "chara_orc.gtg.deflate"

_EnemyFrames_ORC:
    .incbin "chara_orc.gsi"

_TestMap:
    .incbin "testmap.gtg.deflate"

_DynaWave:
    .incbin "audio_fw.bin.deflate"

_MainMusic:
    .incbin "fiend_loop.gtm2"

_TitleMusic:
    .incbin "title.gtm2"

_DiedMusic:
    .incbin "died.gtm2"