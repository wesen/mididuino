	.file	"MidiClock.cpp"
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__CCP__  = 0x34
__tmp_reg__ = 0
__zero_reg__ = 1
	.global __do_copy_data
	.global __do_clear_bss
 ;  GNU C++ (GCC) version 4.3.2 (avr)
 ; 	compiled by GNU C version 4.0.1 (Apple Inc. build 5484), GMP version 4.2.2, MPFR version 2.3.1.
 ;  GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 ;  options passed:  -I. -imultilib avr5 MidiClock.cpp -mmcu=atmega168
 ;  -auxbase-strip midiclock.s -Os -fverbose-asm -fno-rtti
 ;  -fno-enforce-eh-specs -fno-exceptions
 ;  options enabled:  -falign-loops -fargument-alias -fauto-inc-dec
 ;  -fbranch-count-reg -fcaller-saves -fcommon -fcprop-registers
 ;  -fcrossjumping -fcse-follow-jumps -fdefer-pop -fearly-inlining
 ;  -feliminate-unused-debug-types -fexpensive-optimizations
 ;  -fforward-propagate -ffunction-cse -fgcse -fgcse-lm
 ;  -fguess-branch-probability -fident -fif-conversion -fif-conversion2
 ;  -finline-functions -finline-functions-called-once
 ;  -finline-small-functions -fipa-pure-const -fipa-reference -fivopts
 ;  -fkeep-static-consts -fleading-underscore -fmath-errno
 ;  -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
 ;  -fomit-frame-pointer -foptimize-register-move -foptimize-sibling-calls
 ;  -fpeephole -fpeephole2 -freg-struct-return -fregmove
 ;  -freorder-functions -frerun-cse-after-loop -fsched-interblock
 ;  -fsched-spec -fsched-stalled-insns-dep -fsigned-zeros
 ;  -fsplit-ivs-in-unroller -fsplit-wide-types -fstrict-aliasing
 ;  -fstrict-overflow -fthread-jumps -ftoplevel-reorder -ftrapping-math
 ;  -ftree-ccp -ftree-copy-prop -ftree-copyrename -ftree-dce
 ;  -ftree-dominator-opts -ftree-dse -ftree-fre -ftree-loop-im
 ;  -ftree-loop-ivcanon -ftree-loop-optimize -ftree-parallelize-loops=
 ;  -ftree-reassoc -ftree-salias -ftree-scev-cprop -ftree-sink -ftree-sra
 ;  -ftree-store-ccp -ftree-ter -ftree-vect-loop-version -ftree-vrp
 ;  -funit-at-a-time -fverbose-asm -fzero-initialized-in-bss

 ;  Compiler executable checksum: a1412242361e23fb578bb4054fa4c07e

	.text
.global	_ZN14MidiClockClass4initEv
	.type	_ZN14MidiClockClass4initEv, @function
_ZN14MidiClockClass4initEv:
/* prologue: function */
/* frame size = 0 */
	movw r30,r24	 ;  this, this
	std Z+36,__zero_reg__	 ;  <variable>.state,
	std Z+35,__zero_reg__	 ;  <variable>.state,
	std Z+21,__zero_reg__	 ;  <variable>.counter,
	std Z+20,__zero_reg__	 ;  <variable>.counter,
	std Z+23,__zero_reg__	 ;  <variable>.last_clock,
	std Z+22,__zero_reg__	 ;  <variable>.last_clock,
	std Z+6,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+7,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+8,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+9,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+10,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+11,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+12,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+13,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+14,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+15,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+16,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+17,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+26,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+27,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+28,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+29,__zero_reg__	 ;  <variable>.indiv96th_counter,
	ldi r24,lo8(220)	 ;  tmp42,
	ldi r25,hi8(220)	 ;  tmp42,
	std Z+31,r25	 ;  <variable>.pll_x, tmp42
	std Z+30,r24	 ;  <variable>.pll_x, tmp42
/* epilogue start */
	ret
	.size	_ZN14MidiClockClass4initEv, .-_ZN14MidiClockClass4initEv
.global	_ZN14MidiClockClass11handleClockEv
	.type	_ZN14MidiClockClass11handleClockEv, @function
_ZN14MidiClockClass11handleClockEv:
	push r10	 ; 
	push r11	 ; 
	push r12	 ; 
	push r13	 ; 
	push r14	 ; 
	push r15	 ; 
	push r16	 ; 
	push r17	 ; 
	push r28	 ; 
	push r29	 ; 
/* prologue: function */
/* frame size = 0 */
	movw r28,r24	 ;  this, this
	lds r18,clock	 ;  cur_clock, clock
	lds r19,(clock)+1	 ;  cur_clock, clock
	ldd r22,Y+22	 ;  D.2774, <variable>.last_clock
	ldd r23,Y+23	 ;  D.2774, <variable>.last_clock
	cp r22,r18	 ;  D.2774, cur_clock
	cpc r23,r19	 ;  D.2774, cur_clock
	brsh .L4	 ; ,
	movw r24,r18	 ; , cur_clock
	sub r24,r22	 ; , D.2774
	sbc r25,r23	 ; , D.2774
	movw r22,r24	 ;  diff,
	rjmp .L5	 ; 
.L4:
	com r22	 ;  diff
	com r23	 ;  diff
	add r22,r18	 ;  diff, cur_clock
	adc r23,r19	 ;  diff, cur_clock
.L5:
	std Y+25,r23	 ;  <variable>.last_interval, diff
	std Y+24,r22	 ;  <variable>.last_interval, diff
	std Y+23,r19	 ;  <variable>.last_clock, cur_clock
	std Y+22,r18	 ;  <variable>.last_clock, cur_clock
	ldd r18,Y+26	 ;  D.2777, <variable>.indiv96th_counter
	ldd r19,Y+27	 ;  D.2777, <variable>.indiv96th_counter
	ldd r20,Y+28	 ;  D.2777, <variable>.indiv96th_counter
	ldd r21,Y+29	 ;  D.2777, <variable>.indiv96th_counter
	subi r18,lo8(-(1))	 ;  D.2777,
	sbci r19,hi8(-(1))	 ;  D.2777,
	sbci r20,hlo8(-(1))	 ;  D.2777,
	sbci r21,hhi8(-(1))	 ;  D.2777,
	std Y+26,r18	 ;  <variable>.indiv96th_counter, D.2777
	std Y+27,r19	 ;  <variable>.indiv96th_counter, D.2777
	std Y+28,r20	 ;  <variable>.indiv96th_counter, D.2777
	std Y+29,r21	 ;  <variable>.indiv96th_counter, D.2777
	ldd r30,Y+35	 ;  <variable>.state, <variable>.state
	ldd r31,Y+36	 ;  <variable>.state, <variable>.state
	cpi r30,1	 ;  <variable>.state,
	cpc r31,__zero_reg__	 ;  <variable>.state
	breq .L7	 ; ,
	sbiw r30,2	 ;  <variable>.state,
	brne .L10	 ; ,
	rjmp .L11	 ; 
.L7:
	ldi r24,lo8(2)	 ;  tmp50,
	ldi r25,hi8(2)	 ;  tmp50,
	std Y+36,r25	 ;  <variable>.state, tmp50
	std Y+35,r24	 ;  <variable>.state, tmp50
	rjmp .L10	 ; 
.L11:
	cpi r18,lo8(2)	 ;  D.2777,
	cpc r19,__zero_reg__	 ;  D.2777
	cpc r20,__zero_reg__	 ;  D.2777
	cpc r21,__zero_reg__	 ;  D.2777
	brne .L9	 ; ,
	std Y+19,r23	 ;  <variable>.interval, diff
	std Y+18,r22	 ;  <variable>.interval, diff
	rjmp .L10	 ; 
.L9:
	ldd r10,Y+30	 ;  D.2783, <variable>.pll_x
	ldd r11,Y+31	 ;  D.2783, <variable>.pll_x
	ldi r24,lo8(0)	 ;  diff,
	ldi r25,hi8(0)	 ;  diff,
	ldi r18,lo8(256)	 ;  tmp53,
	ldi r19,hi8(256)	 ;  tmp53,
	sub r18,r10	 ;  tmp53, D.2783
	sbc r19,r11	 ;  tmp53, D.2783
	ldi r20,lo8(0)	 ; ,
	ldi r21,hi8(0)	 ; ,
	call __mulsi3
	movw r14,r22	 ;  tmp55,
	movw r16,r24	 ;  tmp55,
	clr r12	 ;  D.2783
	clr r13	 ;  D.2783
	ldd r18,Y+18	 ;  <variable>.interval, <variable>.interval
	ldd r19,Y+19	 ;  <variable>.interval, <variable>.interval
	ldi r20,lo8(0)	 ;  <variable>.interval,
	ldi r21,hi8(0)	 ;  <variable>.interval,
	movw r24,r12	 ; , D.2783
	movw r22,r10	 ; , D.2783
	call __mulsi3
	add r14,r22	 ;  tmp55, tmp59
	adc r15,r23	 ;  tmp55, tmp59
	adc r16,r24	 ;  tmp55, tmp59
	adc r17,r25	 ;  tmp55, tmp59
	mov r14,r15	 ;  tmp61, tmp55
	mov r15,r16	 ;  tmp61, tmp55
	mov r16,r17	 ;  tmp61, tmp55
	clr r17	 ;  tmp61
	std Y+19,r15	 ;  <variable>.interval, tmp61
	std Y+18,r14	 ;  <variable>.interval, tmp61
.L10:
/* epilogue start */
	pop r29	 ; 
	pop r28	 ; 
	pop r17	 ; 
	pop r16	 ; 
	pop r15	 ; 
	pop r14	 ; 
	pop r13	 ; 
	pop r12	 ; 
	pop r11	 ; 
	pop r10	 ; 
	ret
	.size	_ZN14MidiClockClass11handleClockEv, .-_ZN14MidiClockClass11handleClockEv
.global	_ZN14MidiClockClass15handleMidiStartEv
	.type	_ZN14MidiClockClass15handleMidiStartEv, @function
_ZN14MidiClockClass15handleMidiStartEv:
/* prologue: function */
/* frame size = 0 */
	movw r30,r24	 ;  this, this
	ldd r24,Z+37	 ;  <variable>.mode, <variable>.mode
	ldd r25,Z+38	 ;  <variable>.mode, <variable>.mode
	sbiw r24,2	 ;  <variable>.mode,
	brne .L14	 ; ,
	std Z+23,__zero_reg__	 ;  <variable>.last_clock,
	std Z+22,__zero_reg__	 ;  <variable>.last_clock,
	std Z+26,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+27,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+28,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+29,__zero_reg__	 ;  <variable>.indiv96th_counter,
	ldi r24,lo8(220)	 ;  tmp43,
	ldi r25,hi8(220)	 ;  tmp43,
	std Z+31,r25	 ;  <variable>.pll_x, tmp43
	std Z+30,r24	 ;  <variable>.pll_x, tmp43
	ldi r24,lo8(1)	 ;  tmp44,
	ldi r25,hi8(1)	 ;  tmp44,
	std Z+36,r25	 ;  <variable>.state, tmp44
	std Z+35,r24	 ;  <variable>.state, tmp44
	std Z+6,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+7,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+8,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+9,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+10,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+11,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+12,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+13,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+14,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+15,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+16,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+17,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+21,__zero_reg__	 ;  <variable>.counter,
	std Z+20,__zero_reg__	 ;  <variable>.counter,
.L14:
	ret
	.size	_ZN14MidiClockClass15handleMidiStartEv, .-_ZN14MidiClockClass15handleMidiStartEv
.global	_ZN14MidiClockClass14handleMidiStopEv
	.type	_ZN14MidiClockClass14handleMidiStopEv, @function
_ZN14MidiClockClass14handleMidiStopEv:
/* prologue: function */
/* frame size = 0 */
	movw r30,r24	 ;  this, this
	ldd r24,Z+37	 ;  <variable>.mode, <variable>.mode
	ldd r25,Z+38	 ;  <variable>.mode, <variable>.mode
	sbiw r24,2	 ;  <variable>.mode,
	brne .L17	 ; ,
	std Z+36,__zero_reg__	 ;  <variable>.state,
	std Z+35,__zero_reg__	 ;  <variable>.state,
.L17:
	ret
	.size	_ZN14MidiClockClass14handleMidiStopEv, .-_ZN14MidiClockClass14handleMidiStopEv
.global	_ZN14MidiClockClass8setTempoEj
	.type	_ZN14MidiClockClass8setTempoEj, @function
_ZN14MidiClockClass8setTempoEj:
	push r15	 ; 
	push r16	 ; 
	push r17	 ; 
/* prologue: function */
/* frame size = 0 */
	movw r16,r24	 ;  this, this
	in r15,__SREG__	 ;  tmp
/* #APP */
 ;  111 "MidiClock.cpp" 1
	cli
 ;  0 "" 2
/* #NOAPP */
	movw r30,r24	 ; , this
	std Z+33,r23	 ;  <variable>.tempo, _tempo
	std Z+32,r22	 ;  <variable>.tempo, _tempo
	movw r18,r22	 ;  _tempo, _tempo
	ldi r20,lo8(0)	 ;  _tempo,
	ldi r21,hi8(0)	 ;  _tempo,
	ldi r22,lo8(156250)	 ; ,
	ldi r23,hi8(156250)	 ; ,
	ldi r24,hlo8(156250)	 ; ,
	ldi r25,hhi8(156250)	 ; ,
	call __udivmodsi4
	subi r18,lo8(-(-16))	 ;  tmp50,
	sbci r19,hi8(-(-16))	 ;  tmp50,
	movw r30,r16	 ; , this
	std Z+19,r19	 ;  <variable>.interval, tmp50
	std Z+18,r18	 ;  <variable>.interval, tmp50
	out __SREG__,r15	 ;  tmp
/* epilogue start */
	pop r17	 ; 
	pop r16	 ; 
	pop r15	 ; 
	ret
	.size	_ZN14MidiClockClass8setTempoEj, .-_ZN14MidiClockClass8setTempoEj
.global	_ZN14MidiClockClassC1Ev
	.type	_ZN14MidiClockClassC1Ev, @function
_ZN14MidiClockClassC1Ev:
	push r16	 ; 
	push r17	 ; 
/* prologue: function */
/* frame size = 0 */
	movw r16,r24	 ;  this, this
	movw r30,r24	 ; , this
	std Z+36,__zero_reg__	 ;  <variable>.state,
	std Z+35,__zero_reg__	 ;  <variable>.state,
	std Z+21,__zero_reg__	 ;  <variable>.counter,
	std Z+20,__zero_reg__	 ;  <variable>.counter,
	std Z+23,__zero_reg__	 ;  <variable>.last_clock,
	std Z+22,__zero_reg__	 ;  <variable>.last_clock,
	std Z+6,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+7,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+8,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+9,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+10,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+11,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+12,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+13,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+14,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+15,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+16,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+17,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+26,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+27,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+28,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+29,__zero_reg__	 ;  <variable>.indiv96th_counter,
	ldi r24,lo8(220)	 ;  tmp42,
	ldi r25,hi8(220)	 ;  tmp42,
	std Z+31,r25	 ;  <variable>.pll_x, tmp42
	std Z+30,r24	 ;  <variable>.pll_x, tmp42
	std Z+38,__zero_reg__	 ;  <variable>.mode,
	std Z+37,__zero_reg__	 ;  <variable>.mode,
	movw r24,r16	 ; , this
	ldi r22,lo8(120)	 ; ,
	ldi r23,hi8(120)	 ; ,
	call _ZN14MidiClockClass8setTempoEj	 ; 
	movw r30,r16	 ; , this
	std Z+1,__zero_reg__	 ;  <variable>.on96Callback,
	st Z,__zero_reg__	 ;  <variable>.on96Callback,
	std Z+3,__zero_reg__	 ;  <variable>.on32Callback,
	std Z+2,__zero_reg__	 ;  <variable>.on32Callback,
	std Z+5,__zero_reg__	 ;  <variable>.on16Callback,
	std Z+4,__zero_reg__	 ;  <variable>.on16Callback,
	std Z+34,__zero_reg__	 ;  <variable>.transmit,
/* epilogue start */
	pop r17	 ; 
	pop r16	 ; 
	ret
	.size	_ZN14MidiClockClassC1Ev, .-_ZN14MidiClockClassC1Ev
.global	_ZN14MidiClockClassC2Ev
	.type	_ZN14MidiClockClassC2Ev, @function
_ZN14MidiClockClassC2Ev:
	push r16	 ; 
	push r17	 ; 
/* prologue: function */
/* frame size = 0 */
	movw r16,r24	 ;  this, this
	movw r30,r24	 ; , this
	std Z+36,__zero_reg__	 ;  <variable>.state,
	std Z+35,__zero_reg__	 ;  <variable>.state,
	std Z+21,__zero_reg__	 ;  <variable>.counter,
	std Z+20,__zero_reg__	 ;  <variable>.counter,
	std Z+23,__zero_reg__	 ;  <variable>.last_clock,
	std Z+22,__zero_reg__	 ;  <variable>.last_clock,
	std Z+6,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+7,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+8,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+9,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+10,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+11,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+12,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+13,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+14,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+15,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+16,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+17,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+26,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+27,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+28,__zero_reg__	 ;  <variable>.indiv96th_counter,
	std Z+29,__zero_reg__	 ;  <variable>.indiv96th_counter,
	ldi r24,lo8(220)	 ;  tmp42,
	ldi r25,hi8(220)	 ;  tmp42,
	std Z+31,r25	 ;  <variable>.pll_x, tmp42
	std Z+30,r24	 ;  <variable>.pll_x, tmp42
	std Z+38,__zero_reg__	 ;  <variable>.mode,
	std Z+37,__zero_reg__	 ;  <variable>.mode,
	movw r24,r16	 ; , this
	ldi r22,lo8(120)	 ; ,
	ldi r23,hi8(120)	 ; ,
	call _ZN14MidiClockClass8setTempoEj	 ; 
	movw r30,r16	 ; , this
	std Z+1,__zero_reg__	 ;  <variable>.on96Callback,
	st Z,__zero_reg__	 ;  <variable>.on96Callback,
	std Z+3,__zero_reg__	 ;  <variable>.on32Callback,
	std Z+2,__zero_reg__	 ;  <variable>.on32Callback,
	std Z+5,__zero_reg__	 ;  <variable>.on16Callback,
	std Z+4,__zero_reg__	 ;  <variable>.on16Callback,
	std Z+34,__zero_reg__	 ;  <variable>.transmit,
/* epilogue start */
	pop r17	 ; 
	pop r16	 ; 
	ret
	.size	_ZN14MidiClockClassC2Ev, .-_ZN14MidiClockClassC2Ev
	.type	_GLOBAL__I__ZN14MidiClockClassC2Ev, @function
_GLOBAL__I__ZN14MidiClockClassC2Ev:
/* prologue: function */
/* frame size = 0 */
	ldi r24,lo8(MidiClock)	 ; ,
	ldi r25,hi8(MidiClock)	 ; ,
	call _ZN14MidiClockClassC1Ev	 ; 
/* epilogue start */
	ret
	.size	_GLOBAL__I__ZN14MidiClockClassC2Ev, .-_GLOBAL__I__ZN14MidiClockClassC2Ev
	.global __do_global_ctors
	.section .ctors,"a",@progbits
	.word	gs(_GLOBAL__I__ZN14MidiClockClassC2Ev)
	.text
.global	_ZN14MidiClockClass14handleTimerIntEv
	.type	_ZN14MidiClockClass14handleTimerIntEv, @function
_ZN14MidiClockClass14handleTimerIntEv:
	push __zero_reg__	 ; 
	push r0	 ; 
	in r0,__SREG__	 ; 
	push r0	 ; 
	clr __zero_reg__	 ; 
	push r14	 ; 
	push r15	 ; 
	push r16	 ; 
	push r17	 ; 
	push r18	 ; 
	push r19	 ; 
	push r20	 ; 
	push r21	 ; 
	push r22	 ; 
	push r23	 ; 
	push r24	 ; 
	push r25	 ; 
	push r26	 ; 
	push r27	 ; 
	push r28	 ; 
	push r29	 ; 
	push r30	 ; 
	push r31	 ; 
/* prologue: Signal */
/* frame size = 0 */
	movw r28,r24	 ;  this, this
	ldd r24,Y+20	 ;  D.2839, <variable>.counter
	ldd r25,Y+21	 ;  D.2839, <variable>.counter
	sbiw r24,0	 ;  D.2839
	breq .+2	 ; 
	rjmp .L27	 ; 
	ldd r24,Y+18	 ;  <variable>.interval, <variable>.interval
	ldd r25,Y+19	 ;  <variable>.interval, <variable>.interval
	std Y+21,r25	 ;  <variable>.counter, <variable>.interval
	std Y+20,r24	 ;  <variable>.counter, <variable>.interval
	ldd r24,Y+6	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r25,Y+7	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r26,Y+8	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r27,Y+9	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	adiw r24,1	 ;  <variable>.div96th_counter,
	adc r26,__zero_reg__	 ;  <variable>.div96th_counter
	adc r27,__zero_reg__	 ;  <variable>.div96th_counter
	std Y+6,r24	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	std Y+7,r25	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	std Y+8,r26	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	std Y+9,r27	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r24,Y+37	 ;  D.2843, <variable>.mode
	ldd r25,Y+38	 ;  D.2843, <variable>.mode
	cpi r24,2	 ;  D.2843,
	cpc r25,__zero_reg__	 ;  D.2843
	breq .+2	 ; 
	rjmp .L28	 ; 
	lds r22,clock	 ;  cur_clock, clock
	lds r23,(clock)+1	 ;  cur_clock, clock
	ldd r24,Y+22	 ; , <variable>.last_clock
	ldd r25,Y+23	 ; , <variable>.last_clock
	call _Z10clock_diffjj	 ; 
	movw r30,r24	 ;  diff,
	ldd r14,Y+6	 ;  temp.74, <variable>.div96th_counter
	ldd r15,Y+7	 ;  temp.74, <variable>.div96th_counter
	ldd r16,Y+8	 ;  temp.74, <variable>.div96th_counter
	ldd r17,Y+9	 ;  temp.74, <variable>.div96th_counter
	ldd r18,Y+26	 ;  D.2848, <variable>.indiv96th_counter
	ldd r19,Y+27	 ;  D.2848, <variable>.indiv96th_counter
	ldd r20,Y+28	 ;  D.2848, <variable>.indiv96th_counter
	ldd r21,Y+29	 ;  D.2848, <variable>.indiv96th_counter
	cp r14,r18	 ;  temp.74, D.2848
	cpc r15,r19	 ;  temp.74, D.2848
	cpc r16,r20	 ;  temp.74, D.2848
	cpc r17,r21	 ;  temp.74, D.2848
	brlo .L29	 ; ,
	movw r26,r20	 ;  tmp56, D.2848
	movw r24,r18	 ;  tmp56, D.2848
	adiw r24,1	 ;  tmp56,
	adc r26,__zero_reg__	 ;  tmp56
	adc r27,__zero_reg__	 ;  tmp56
	cp r24,r14	 ;  tmp56, temp.74
	cpc r25,r15	 ;  tmp56, temp.74
	cpc r26,r16	 ;  tmp56, temp.74
	cpc r27,r17	 ;  tmp56, temp.74
	brsh .L30	 ; ,
.L29:
	std Y+6,r18	 ;  <variable>.div96th_counter, D.2848
	std Y+7,r19	 ;  <variable>.div96th_counter, D.2848
	std Y+8,r20	 ;  <variable>.div96th_counter, D.2848
	std Y+9,r21	 ;  <variable>.div96th_counter, D.2848
.L30:
	ldd r18,Y+6	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r19,Y+7	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r20,Y+8	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r21,Y+9	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r24,Y+26	 ;  <variable>.indiv96th_counter, <variable>.indiv96th_counter
	ldd r25,Y+27	 ;  <variable>.indiv96th_counter, <variable>.indiv96th_counter
	ldd r26,Y+28	 ;  <variable>.indiv96th_counter, <variable>.indiv96th_counter
	ldd r27,Y+29	 ;  <variable>.indiv96th_counter, <variable>.indiv96th_counter
	ldd r22,Y+20	 ;  temp.78, <variable>.counter
	ldd r23,Y+21	 ;  temp.78, <variable>.counter
	cp r24,r18	 ;  <variable>.indiv96th_counter, <variable>.div96th_counter
	cpc r25,r19	 ;  <variable>.indiv96th_counter, <variable>.div96th_counter
	cpc r26,r20	 ;  <variable>.indiv96th_counter, <variable>.div96th_counter
	cpc r27,r21	 ;  <variable>.indiv96th_counter, <variable>.div96th_counter
	brlo .L31	 ; ,
	movw r24,r30	 ;  diff, diff
	ldi r26,lo8(0)	 ;  diff,
	ldi r27,hi8(0)	 ;  diff,
	ldi r19,4	 ; ,
1:	lsr r27	 ;  diff
	ror r26	 ;  diff
	ror r25	 ;  diff
	ror r24	 ;  diff
	dec r19	 ; 
	brne 1b
	sub r22,r24	 ;  temp.78, diff
	sbc r23,r25	 ;  temp.78, diff
	rjmp .L36	 ; 
.L31:
	cp r30,r22	 ;  diff, temp.78
	cpc r31,r23	 ;  diff, temp.78
	brsh .L32	 ; ,
	movw r24,r22	 ;  tmp64, temp.78
	sub r24,r30	 ;  tmp64, diff
	sbc r25,r31	 ;  tmp64, diff
	ldi r26,lo8(0)	 ; ,
	ldi r27,hi8(0)	 ; ,
	ldi r18,4	 ; ,
1:	lsr r27	 ;  tmp65
	ror r26	 ;  tmp65
	ror r25	 ;  tmp65
	ror r24	 ;  tmp65
	dec r18	 ; 
	brne 1b
	add r22,r24	 ;  temp.78, tmp65
	adc r23,r25	 ;  temp.78, tmp65
.L36:
	std Y+21,r23	 ;  <variable>.counter, temp.78
	std Y+20,r22	 ;  <variable>.counter, temp.78
	rjmp .L32	 ; 
.L28:
	sbiw r24,1	 ;  D.2843,
	brne .L32	 ; ,
	ldd r24,Y+34	 ;  <variable>.transmit, <variable>.transmit
	tst r24	 ;  <variable>.transmit
	breq .L32	 ; ,
	ldi r24,lo8(MidiUart)	 ; ,
	ldi r25,hi8(MidiUart)	 ; ,
	ldi r22,lo8(-8)	 ; ,
	call _ZN13MidiUartClass4putcEh	 ; 
.L32:
	ld r30,Y	 ;  D.2860, <variable>.on96Callback
	ldd r31,Y+1	 ;  D.2860, <variable>.on96Callback
	sbiw r30,0	 ;  D.2860
	breq .L33	 ; ,
	icall
.L33:
	ldd r22,Y+6	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r23,Y+7	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r24,Y+8	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldd r25,Y+9	 ;  <variable>.div96th_counter, <variable>.div96th_counter
	ldi r18,lo8(6)	 ; ,
	ldi r19,hi8(6)	 ; ,
	ldi r20,hlo8(6)	 ; ,
	ldi r21,hhi8(6)	 ; ,
	call __udivmodsi4
	cpi r22,lo8(1)	 ;  tmp75,
	cpc r23,__zero_reg__	 ;  tmp75
	cpc r24,__zero_reg__	 ;  tmp75
	cpc r25,__zero_reg__	 ;  tmp75
	brne .L35	 ; ,
	ldd r24,Y+14	 ;  <variable>.div16th_counter, <variable>.div16th_counter
	ldd r25,Y+15	 ;  <variable>.div16th_counter, <variable>.div16th_counter
	ldd r26,Y+16	 ;  <variable>.div16th_counter, <variable>.div16th_counter
	ldd r27,Y+17	 ;  <variable>.div16th_counter, <variable>.div16th_counter
	adiw r24,1	 ;  <variable>.div16th_counter,
	adc r26,__zero_reg__	 ;  <variable>.div16th_counter
	adc r27,__zero_reg__	 ;  <variable>.div16th_counter
	std Y+14,r24	 ;  <variable>.div16th_counter, <variable>.div16th_counter
	std Y+15,r25	 ;  <variable>.div16th_counter, <variable>.div16th_counter
	std Y+16,r26	 ;  <variable>.div16th_counter, <variable>.div16th_counter
	std Y+17,r27	 ;  <variable>.div16th_counter, <variable>.div16th_counter
	ldd r30,Y+4	 ;  D.2864, <variable>.on16Callback
	ldd r31,Y+5	 ;  D.2864, <variable>.on16Callback
	sbiw r30,0	 ;  D.2864
	breq .L35	 ; ,
	icall
	rjmp .L35	 ; 
.L27:
	sbiw r24,1	 ;  D.2839,
	std Y+21,r25	 ;  <variable>.counter, D.2839
	std Y+20,r24	 ;  <variable>.counter, D.2839
.L35:
/* epilogue start */
	pop r31	 ; 
	pop r30	 ; 
	pop r29	 ; 
	pop r28	 ; 
	pop r27	 ; 
	pop r26	 ; 
	pop r25	 ; 
	pop r24	 ; 
	pop r23	 ; 
	pop r22	 ; 
	pop r21	 ; 
	pop r20	 ; 
	pop r19	 ; 
	pop r18	 ; 
	pop r17	 ; 
	pop r16	 ; 
	pop r15	 ; 
	pop r14	 ; 
	pop r0	 ; 
	out __SREG__,r0	 ; 
	pop r0	 ; 
	pop __zero_reg__	 ; 
	reti
	.size	_ZN14MidiClockClass14handleTimerIntEv, .-_ZN14MidiClockClass14handleTimerIntEv
.global	_ZN14MidiClockClass5pauseEv
	.type	_ZN14MidiClockClass5pauseEv, @function
_ZN14MidiClockClass5pauseEv:
/* prologue: function */
/* frame size = 0 */
	movw r30,r24	 ;  this, this
	ldd r24,Z+37	 ;  <variable>.mode, <variable>.mode
	ldd r25,Z+38	 ;  <variable>.mode, <variable>.mode
	sbiw r24,1	 ;  <variable>.mode,
	brne .L40	 ; ,
	ldd r24,Z+35	 ;  <variable>.state, <variable>.state
	ldd r25,Z+36	 ;  <variable>.state, <variable>.state
	or r24,r25	 ;  <variable>.state
	brne .L39	 ; ,
	ldi r24,lo8(2)	 ;  tmp44,
	ldi r25,hi8(2)	 ;  tmp44,
	std Z+36,r25	 ;  <variable>.state, tmp44
	std Z+35,r24	 ;  <variable>.state, tmp44
	ldd r24,Z+34	 ;  <variable>.transmit, <variable>.transmit
	tst r24	 ;  <variable>.transmit
	breq .L40	 ; ,
	ldi r24,lo8(MidiUart)	 ; ,
	ldi r25,hi8(MidiUart)	 ; ,
	ldi r22,lo8(-5)	 ; ,
	call _ZN13MidiUartClass4putcEh	 ; 
	ret
.L39:
	std Z+36,__zero_reg__	 ;  <variable>.state,
	std Z+35,__zero_reg__	 ;  <variable>.state,
.L40:
	ret
	.size	_ZN14MidiClockClass5pauseEv, .-_ZN14MidiClockClass5pauseEv
.global	_ZN14MidiClockClass4stopEv
	.type	_ZN14MidiClockClass4stopEv, @function
_ZN14MidiClockClass4stopEv:
/* prologue: function */
/* frame size = 0 */
	movw r30,r24	 ;  this, this
	ldd r24,Z+37	 ;  <variable>.mode, <variable>.mode
	ldd r25,Z+38	 ;  <variable>.mode, <variable>.mode
	sbiw r24,1	 ;  <variable>.mode,
	brne .L43	 ; ,
	std Z+36,__zero_reg__	 ;  <variable>.state,
	std Z+35,__zero_reg__	 ;  <variable>.state,
	ldd r24,Z+34	 ;  <variable>.transmit, <variable>.transmit
	tst r24	 ;  <variable>.transmit
	breq .L43	 ; ,
	ldi r24,lo8(MidiUart)	 ; ,
	ldi r25,hi8(MidiUart)	 ; ,
	ldi r22,lo8(-4)	 ; ,
	call _ZN13MidiUartClass4putcEh	 ; 
.L43:
	ret
	.size	_ZN14MidiClockClass4stopEv, .-_ZN14MidiClockClass4stopEv
.global	_ZN14MidiClockClass5startEv
	.type	_ZN14MidiClockClass5startEv, @function
_ZN14MidiClockClass5startEv:
/* prologue: function */
/* frame size = 0 */
	movw r30,r24	 ;  this, this
	ldd r24,Z+37	 ;  <variable>.mode, <variable>.mode
	ldd r25,Z+38	 ;  <variable>.mode, <variable>.mode
	sbiw r24,1	 ;  <variable>.mode,
	brne .L46	 ; ,
	ldi r24,lo8(2)	 ;  tmp43,
	ldi r25,hi8(2)	 ;  tmp43,
	std Z+36,r25	 ;  <variable>.state, tmp43
	std Z+35,r24	 ;  <variable>.state, tmp43
	std Z+6,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+7,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+8,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+9,__zero_reg__	 ;  <variable>.div96th_counter,
	std Z+10,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+11,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+12,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+13,__zero_reg__	 ;  <variable>.div32th_counter,
	std Z+14,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+15,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+16,__zero_reg__	 ;  <variable>.div16th_counter,
	std Z+17,__zero_reg__	 ;  <variable>.div16th_counter,
	ldd r24,Z+34	 ;  <variable>.transmit, <variable>.transmit
	tst r24	 ;  <variable>.transmit
	breq .L46	 ; ,
	ldi r24,lo8(MidiUart)	 ; ,
	ldi r25,hi8(MidiUart)	 ; ,
	ldi r22,lo8(-6)	 ; ,
	call _ZN13MidiUartClass4putcEh	 ; 
.L46:
	ret
	.size	_ZN14MidiClockClass5startEv, .-_ZN14MidiClockClass5startEv
.global	MidiClock
.global	MidiClock
	.section .bss
	.type	MidiClock, @object
	.size	MidiClock, 39
MidiClock:
	.skip 39,0
