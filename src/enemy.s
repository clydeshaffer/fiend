.export		_LD_tempEnemy
.export		_ST_tempEnemy
.importzp sp, ptr1
.import incsp2, _tempEnemy, pushax

; ---------------------------------------------------------------
; void __near__ LD_tempEnemy (struct MobState *enemy)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_LD_tempEnemy: near

.segment	"CODE"

	jsr     pushax
	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	lda     (ptr1)
	sta     _tempEnemy
    lda     (ptr1),y
    sta     _tempEnemy, y
    iny
    lda     (ptr1),y
    sta     _tempEnemy, y
    iny
    lda     (ptr1),y
    sta     _tempEnemy, y
    iny
    lda     (ptr1),y
    sta     _tempEnemy, y
    iny
    lda     (ptr1),y
    sta     _tempEnemy, y
    iny
    lda     (ptr1),y
    sta     _tempEnemy, y
    iny
    lda     (ptr1),y
    sta     _tempEnemy, y
	jmp     incsp2

.endproc

; ---------------------------------------------------------------
; void __near__ ST_tempEnemy (struct MobState *enemy)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_ST_tempEnemy: near

.segment	"CODE"

	jsr     pushax
	ldy     #$01
	lda     (sp),y
	sta     ptr1+1
	lda     (sp)
	sta     ptr1
	lda     _tempEnemy
	sta     (ptr1)
    lda     _tempEnemy, y
    sta     (ptr1),y
    iny
    lda     _tempEnemy, y
    sta     (ptr1),y
    iny
    lda     _tempEnemy, y
    sta     (ptr1),y
    iny
    lda     _tempEnemy, y
    sta     (ptr1),y
    iny
    lda     _tempEnemy, y
    sta     (ptr1),y
    iny
    lda     _tempEnemy, y
    sta     (ptr1),y
    iny
    lda     _tempEnemy, y
    sta     (ptr1),y
	jmp     incsp2

.endproc