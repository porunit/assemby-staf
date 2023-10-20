%include "lib.inc"
%include "dict.inc"
%include "words.inc"

section .bss
buffer: resb 256				

section .rodata
error_msg: db "fail", 0


section .text

global _start

_start:
	mov rdi, buffer
	mov rsi, 256
	call read_word							
	test rax, rax
	je .error							
	mov rdi, buffer
	mov rsi, pointer
	call find_word						
	test rax, rax
	je .error						
	mov rdi, rax							
	add rdi, 8				
	push rdi
	call string_length					
	pop rdi
	add rdi, rax						
	inc rdi									
	call print_string
	call print_newline
	xor rdi, rdi
	call exit
.error:
	mov rdi, error_msg
	call string_length
    mov rdx, rax
    mov rax, 1
    mov rdi, 2
    mov rsi, error_msg
    syscall
.end:
	call print_newline
	call exit
