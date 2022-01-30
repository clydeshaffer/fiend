.export _pushRect
.importzp sp, ptr1
.import incsp2, _queue_end, _queue_count, pushax
.import _rect, _banksMirror, _queue_flags_param

BankReg = $2005
DMAFlags = $2007

Q_DMAFlags = $0200
Q_VX       = $0300
Q_VY       = $0400
Q_GX       = $0500
Q_GY       = $0600
Q_WIDTH    = $0700
Q_HEIGHT   = $0800
Q_Color    = $0900
Q_BankReg  = $0A00

; ---------------------------------------------------------------
; void __near__ pushRect ()
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_pushRect: near

.segment	"CODE"

    jsr     pushax

    ldy     _queue_end
    inc     _queue_end
    inc     _queue_count

    lda     _banksMirror
    ora     #$40
    sta     BankReg
    tax
    and     #$3F
    sta     $0 ;storing at zero of BANK 1
    sta     BankReg


    lda     _rect+7 ;bank
    stx     BankReg
    sta     Q_BankReg, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    lda     _rect+0 ;x
    stx     BankReg
    sta     Q_VX, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    lda     _rect+1 ;y
    stx     BankReg
    sta     Q_VY, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    lda     _rect+4 ;gx
    stx     BankReg
    sta     Q_GX, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    lda     _rect+5 ;gy
    stx     BankReg
    sta     Q_GY, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    lda     _rect+2 ;w
    stx     BankReg
    sta     Q_WIDTH, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    lda     _rect+3 ;h
    stx     BankReg
    sta     Q_HEIGHT, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    lda     _rect+6 ;color
    stx     BankReg
    sta     Q_Color, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    lda     _queue_flags_param ;flags
    stx     BankReg
    sta     Q_DMAFlags, y
    lda     $0 ;loading at zero of BANK 1
    sta     BankReg

    jmp     incsp2

.endproc