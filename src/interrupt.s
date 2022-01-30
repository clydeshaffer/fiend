; ---------------------------------------------------------------------------
; interrupt.s
; ---------------------------------------------------------------------------
;
; Interrupt handler.
;
; Checks for a BRK instruction and returns from all valid interrupts.

.import   _stop, _frameflag, _queue_pending, _queue_start
.import   _queue_end, _queue_count, _flagsMirror, _frameflip
.import   _banksMirror
.export   _irq_int, _nmi_int

Q_DMAFlags = $0200
Q_VX       = $0300
Q_VY       = $0400
Q_GX       = $0500
Q_GY       = $0600
Q_WIDTH    = $0700
Q_HEIGHT   = $0800
Q_Color    = $0900
Q_BankReg  = $0A00


BankReg = $2005
DMAFlags = $2007
DMA_VX = $4000
DMA_VY = $4001
DMA_GX = $4002
DMA_GY = $4003
DMA_WIDTH = $4004
DMA_HEIGHT = $4005
DMA_Start = $4006
DMA_Color = $4007

.segment  "CODE"

.PC02                             ; Force 65C02 assembly mode

; ---------------------------------------------------------------------------
; Non-maskable interrupt (NMI) service routine

_nmi_int:                    ; Save X register contents to stack
        PHA
        LDA #0
        STA _frameflag
        PLA                    ; Restore accumulator contents
        RTI                    ; Return from all NMI interrupts

; ---------------------------------------------------------------------------
; Maskable interrupt (IRQ) service routine

_irq_int:
        SEI
        PHX                    ; Save X register contents to stack
        PHA
        PHY

        ;make sure DMA isn't running then compare head and tail of queue
        ;to determine whether there is more to process
        LDA #0
        STA $4006 ;vram[START]
        STA _queue_pending
        LDA _queue_start
        CMP _queue_end
        BEQ finish_irq
        LDA #1
        STA _queue_pending
next_queue:
        ;determined that there is more to process
        ;so load next set of parameters
        LDA #0
        STA BankReg

        ;make sure DMA mode is set to input these params
        LDA _flagsMirror
        ORA #$01

        LDY _queue_start

        LDA #$40
        STA BankReg

        LDA Q_VX, y
        STA DMA_VX

        LDA Q_VY, y
        STA DMA_VY

        LDA Q_GX, y
        STA DMA_GX

        LDA Q_GY, y
        STA DMA_GY

        LDA Q_WIDTH, y
        STA DMA_WIDTH

        LDA Q_HEIGHT, y
        STA DMA_HEIGHT

        LDA Q_Color, y
        STA DMA_Color

        LDX Q_DMAFlags, y
        LDA Q_BankReg, y

        AND #$3F
        STA BankReg
        STA _banksMirror

        TXA ;retrieve Q_DMAFlags value
        ORA _frameflip
        ORA #$45 ; DMA_ENABLE | DMA_NMI | DMA_IRQ
        STA _flagsMirror
        STA DMAFlags

        INC _queue_start

        LDA #1
        STA DMA_Start ;START

        DEC _queue_count
finish_irq:
        PLY
        PLA                    ; Restore accumulator contents
        PLX                    ; Restore X register contents
        CLI
        RTI                    ; Return from all IRQ interrupts
