; ---------------------------------------------------------------------------
; interrupt.s
; ---------------------------------------------------------------------------
;
; Interrupt handler.
;
; Checks for a BRK instruction and returns from all valid interrupts.

.import   _stop, _frameflag, _queue_pending, _queue_start
.import   _queue_end, _queue_count, _flagsMirror, _frameflip
.import   _draw_queue
.export   _irq_int, _nmi_int

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
           PHX                    ; Save X register contents to stack
           PHA
           PHY

           LDA #0
           STA $4006 ;vram[START]
           STA _queue_pending
           LDA _queue_start
           CMP _queue_end
           BEQ finish_irq
           LDA #1
           STA _queue_pending
next_queue:
           LDY _queue_start
           LDA _draw_queue, y
           ORA _frameflip
           ORA #69 ; DMA_ENABLE | DMA_NMI | DMA_IRQ
           STA _flagsMirror
           STA $2007 ;dma_flags
           INY
           LDA _draw_queue, y
           STA $4000 ;VX
           INY
           LDA _draw_queue, y
           STA $4001 ;VY
           INY
           LDA _draw_queue, y
           STA $4002 ;GX
           INY
           LDA _draw_queue, y
           STA $4003 ;GY
           INY
           LDA _draw_queue, y
           STA $4004 ;WIDTH
           INY
           LDA _draw_queue, y
           STA $4005 ;HEIGHT
           INY
           LDA _draw_queue, y
           STA $4007 ;COLOR
           INY
           LDA 1
           STA $4006 ;START
           STY _queue_start
           DEC _queue_count
finish_irq:
           PLY
           PLA                    ; Restore accumulator contents
           PLX                    ; Restore X register contents
           RTI                    ; Return from all IRQ interrupts
