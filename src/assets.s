    .export _GameSprites, _HeroSprites, _HeroFrames, _EnemySprites, _EnemyFrames, _DynaWave, _TestMap, _MainMusic, _TitleMusic, _DiedMusic

    .RODATA
_GameSprites:
    .incbin "gamesprites.gtg.deflate"

_HeroSprites:
    .incbin "chara_hero.gtg.deflate"

_HeroFrames:
    .incbin "chara_hero.gsi"


;Enemy sprites

_EnemySprites:
_EnemySprites_SPIDER:
    .incbin "chara_spider.gtg.deflate"

_EnemyFrames:
_EnemyFrames_SPIDER:
    .incbin "chara_spider.gsi"

_EnemySprites_BAT:
    .incbin "chara_bat.gtg.deflate"

_EnemyFrames_BAT:
    .incbin "chara_bat.gsi"

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