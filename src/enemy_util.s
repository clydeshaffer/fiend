.export _LD_enemy_index, _ST_enemy_index
.import _enemies, _tempEnemy

.segment "CODE"
    table16:
    .byte 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240

; void LD_enemy_index(char i) take index i from enemies array and fill tempEnemy
; MobState struct is 16 bytes long and MAX_ENEMIES is 32
.proc _LD_enemy_index: near
    PHY
    PHX
    TAY
    AND #$0F
    TAX
    TYA
    LDY table16, x
    CMP #16
    BCS LD_Enemy_UpperHalf
    LDA _enemies, y
    STA _tempEnemy
    INY
    LDA _enemies, y
    STA _tempEnemy+1
    INY
    LDA _enemies, y
    STA _tempEnemy+2
    INY
    LDA _enemies, y
    STA _tempEnemy+3
    INY
    LDA _enemies, y
    STA _tempEnemy+4
    INY
    LDA _enemies, y
    STA _tempEnemy+5
    INY
    LDA _enemies, y
    STA _tempEnemy+6
    INY
    LDA _enemies, y
    STA _tempEnemy+7
    INY
    LDA _enemies, y
    STA _tempEnemy+8
    INY
    LDA _enemies, y
    STA _tempEnemy+9
    INY
    LDA _enemies, y
    STA _tempEnemy+10
    PLX
    PLY
    RTS
LD_Enemy_UpperHalf:
    LDA _enemies+256, y
    STA _tempEnemy
    INY
    LDA _enemies+256, y
    STA _tempEnemy+1
    INY
    LDA _enemies+256, y
    STA _tempEnemy+2
    INY
    LDA _enemies+256, y
    STA _tempEnemy+3
    INY
    LDA _enemies+256, y
    STA _tempEnemy+4
    INY
    LDA _enemies+256, y
    STA _tempEnemy+5
    INY
    LDA _enemies+256, y
    STA _tempEnemy+6
    INY
    LDA _enemies+256, y
    STA _tempEnemy+7
    INY
    LDA _enemies+256, y
    STA _tempEnemy+8
    INY
    LDA _enemies+256, y
    STA _tempEnemy+9
    PLX
    PLY
    RTS
.endproc

.proc _ST_enemy_index: near
    PHY
    PHX
    TAY
    AND #$0F
    TAX
    TYA
    LDY table16, x
    CMP #16
    BCS ST_Enemy_UpperHalf
    LDA _tempEnemy+0
    STA _enemies, y
    INY
    LDA _tempEnemy+1
    STA _enemies, y
    INY
    LDA _tempEnemy+2
    STA _enemies, y
    INY
    LDA _tempEnemy+3
    STA _enemies, y
    INY
    LDA _tempEnemy+4
    STA _enemies, y
    INY
    LDA _tempEnemy+5
    STA _enemies, y
    INY
    LDA _tempEnemy+6
    STA _enemies, y
    INY
    LDA _tempEnemy+7
    STA _enemies, y
    INY
    LDA _tempEnemy+8
    STA _enemies, y
    INY
    LDA _tempEnemy+9
    STA _enemies, y
    PLX
    PLY
    RTS
ST_Enemy_UpperHalf:
LDA _tempEnemy+0
    STA _enemies+256, y
    INY
    LDA _tempEnemy+1
    STA _enemies+256, y
    INY
    LDA _tempEnemy+2
    STA _enemies+256, y
    INY
    LDA _tempEnemy+3
    STA _enemies+256, y
    INY
    LDA _tempEnemy+4
    STA _enemies+256, y
    INY
    LDA _tempEnemy+5
    STA _enemies+256, y
    INY
    LDA _tempEnemy+6
    STA _enemies+256, y
    INY
    LDA _tempEnemy+7
    STA _enemies+256, y
    INY
    LDA _tempEnemy+8
    STA _enemies+256, y
    INY
    LDA _tempEnemy+9
    STA _enemies+256, y
    PLX
    PLY
    RTS

.endproc