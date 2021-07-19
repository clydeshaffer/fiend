.export _pushRect
.importzp sp, ptr1
.import incsp2, _draw_queue, _queue_end, _queue_count, pushax, _rect

; ---------------------------------------------------------------
; void __near__ pushRect ()
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_pushRect: near

.segment	"CODE"

    jsr     pushax

    ldy     _queue_end

    lda     _rect+7
    sta     _draw_queue,y
    iny
    lda     _rect+0
    ora     #128
    sta     _draw_queue,y
    iny
    lda     _rect+1
    ora     #128
    sta     _draw_queue,y
    iny
    lda     _rect+4
    sta     _draw_queue,y
    iny
    lda     _rect+5
    sta     _draw_queue,y
    iny
    lda     _rect+2
    sta     _draw_queue,y
    iny
    lda     _rect+3
    sta     _draw_queue,y
    iny
    lda     _rect+6
    sta     _draw_queue,y
    iny

    sty     _queue_end
    inc     _queue_count

    jmp     incsp2

.endproc