    .export _GameSprites, _HeroSprites, _HeroMeta, _DynaWave, _TestMap

    .RODATA
_GameSprites:
    .incbin "gamesprites.gtg.deflate"

_HeroSprites:
    .incbin "chara_hero.gtg.deflate"

_HeroMeta:
    .incbin "chara_hero.gsi"

_TestMap:
    .incbin "testmap.gtg.deflate"

_DynaWave:
    .incbin "dynawave.acp.deflate"