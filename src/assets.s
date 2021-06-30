    .export _GameSprites, _DynaWave, _TestMap

    .RODATA
_GameSprites:
    .incbin "gamesprites.gtg.deflate"

_TestMap:
    .incbin "testmap.gtg.deflate"

_DynaWave:
    .incbin "dynawave.acp.deflate"