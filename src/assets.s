    .export _GameSprites, _HeroSprites, _HeroFrames, _EnemySprites, _EnemyFrames, _DynaWave, _TestMap, _MainMusic, _TitleMusic, _DiedMusic

    .RODATA
_GameSprites:
    .incbin "gamesprites.gtg.deflate"

_HeroSprites:
    .incbin "chara_hero.gtg.deflate"

_HeroFrames:
    .incbin "chara_hero.gsi"

_EnemySprites:
    .incbin "chara_spider.gtg.deflate"

_EnemyFrames:
    .incbin "chara_spider.gsi"

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