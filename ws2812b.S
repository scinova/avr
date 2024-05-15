;#include <avr/io.h>

#define __SFR_OFFSET 0

.global ws2812b_write

ws2812b_write:
	mov   r28, r24
	mov   r29, r24  ; r29 = port
	andi  r28, 0x0F ; r28 = pin
	lsr   r29
	lsr   r29
	lsr   r29
	lsr   r29
	movw  r26, r22  ;r26:27 = X = *data
	movw  r24, r20  ;r24:25 = len

; make mask out of r28 to r30 and r31
	inc   r28
	ldi   r30, 0x01
mask:
	lsl   r30
	dec   r28
	brne  mask
	lsr   r30
	mov   r31, r30
	com   r31
; put PORTX address into Y (r28:29)
	ldi   r23, 0
	ldi   r22, 0x32
	mov   r21, r29
	inc   r21
mull:
	subi  r22, -3
	dec   r21
	brne  mull
	movw  r28, r22

	in    r22, SREG
	cli

	ld    r20, Y
	mov   r21, r20
	or    r20, r30  ;our '1' output
	and   r21, r31  ;our '0' output
	ldi   r19, 7    ;7 bit counter (8th bit is different)
	ld    r18, X+   ;get first data byte

;0.125us
;0.0625us 6.4/13.6 3.2/6.8 3/7 6 14
;         12.8/7.2 6.4/3.6 7/3 13 7

loop1:
	st    Y, r20    ; 2   +0 start of a bit pulse
	lsl   r18       ; 1   +2 next bit into C, MSB first
	brcs  L1        ; 1/2 +3 branch if 1
	st    Y, r21    ; 2   +5 end hi for '0' bit (3 clocks hi)
	bst   r18, 7    ; 1   +7 save last bit of data for fast branching
	subi  r19, 1    ; 1   +8 how many more bits for this byte?
	breq  bit8      ; 1/2 +9 last bit, do differently
	rjmp  loop1     ; 2   +10 10 total for 0 bit
L1:
	bst   r18, 7    ; 1   +5 save last bit of data for fast branching
	subi  r19, 1    ; 1   +6 how many more bits for this byte
	st    Y, r21    ; 1   +7 end hi for '1' bit (7 clocks hi)
	brne  loop1     ; 2/1 +10 10 total for 1 bit (fall thru if last bit)
bit8:
	ldi   r19, 7    ; 1   +11 bit count for next byte
	st    Y, r20    ; 1  +0 start of a bit pulse
	brts  L2        ; 1/2 +2 branch if last bit is a 1
	st    Y, r21    ; 2   +3 end hi for '0' bit (3 clocks hi)
	ld    r18, X+   ; 2   +5 fetch next byte
	sbiw  r24, 1    ; 2   +7 dec byte counter
	brne  loop1     ; 2   +9 loop back or return
	out   SREG, r22 ; restore global int flag
	rjmp end
L2:
	ld    r18, X+   ; 2   +3 fetch next byte
	sbiw  r24, 1    ; 2   +5 dec byte counter
	st    Y, r21    ; 2   +7
	brne  loop1     ; 2   +9 loop back or return
	out   SREG, r22 ; restore global int flag
end:
	ret
