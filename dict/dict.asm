%include "lib.inc"

section .text

global find_word

find_word:					
	push r12				
	push r13
	mov r12, rdi			
	mov r13, rsi			
.loop:
	test r13, r13			
	je .miss
	add r13, 8 
	mov rdi, r12
	mov rsi, r13
	call string_equals
	cmp rax, 1
	je .hit				
	sub r13, 8
	mov r13, [r13]			
	jmp .loop
.miss:
	xor rax, rax
	jmp .end
.hit:
	mov rax, r13
.end:
	pop r13
	pop r12
	ret

