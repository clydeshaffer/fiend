    .export _GameSprites, _HeroSprites, _HeroFrames, _EnemySprites, _EnemyFrames, _DynaWave, _TestMap

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
    .incbin "dynawave.acp.deflate"