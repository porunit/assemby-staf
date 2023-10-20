section .text
 
 
; Принимает код возврата и завершает текущий процесс
exit: 
    mov rax, 60
	mov rdi, 69
	syscall
     

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax
	.loop:
	cmp byte [rdi], 0
	je .end
	inc rax
	inc rdi
	jmp .loop
	.end:
    ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
	push rdi
	call string_length
	pop rsi
	mov rdx, rax
	mov rdi, 1
	mov rax, 1
	syscall
    ret

; Принимает код символа и выводит его в stdout
print_char:
    mov rax, 1
	mov rdx, 1
	push rdi 
	mov rsi, rsp
	mov rdi, 1
	syscall
	pop rdi
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
	mov rdi, 0xA
	call print_char
    ret

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
	mov rax, rdi
	push rbx
	mov rbx, 10
	xor rcx, rcx
.loop:
	xor rdx, rdx
	div rbx
	add rdx, '0'
	push rdx
	inc rcx
	test rax, rax
	jnz .loop
.write_l:
	pop rdi
	push rcx
	call print_char
	pop rcx
	dec rcx
	test rcx, rcx
	jnz .write_l
	pop rbx
	ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    cmp rdi, 0
	jns print_uint
	push rdi
	mov rdi, '-'
	call print_char
	pop rdi
	neg rdi
	jmp print_uint
	ret

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
	xor rcx, rcx
	xor rax, rax
.while:
	mov al, byte[rdi + rcx]
	mov ah, byte[rsi + rcx]
	cmp al, ah
	jne .not_equals
	cmp al, 0
	je .equals
	inc rcx
	jmp .while
.equals:
	mov rax, 1
	ret
.not_equals:
	mov rax, 0
	ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
	sub rsp, 8
	mov rsi, rsp
	xor rdi, rdi
	xor rax, rax
	mov rdx, 1
	syscall
	cmp rax, 0
	jle .ex
	pop rax
	ret
.ex:
	pop rax
	mov rax,0
	ret

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

read_word:
	push r12
	push r13
	push r14
	push rdi
	mov r12, rdi
	mov r13, rsi
	mov r14, rcx

.while_space:
	call read_char
	cmp rax, 0x20
	je .while_space
	cmp rax, 0x9
	je .while_space
	cmp rax, 0xA
	je .while_space

	xor rcx, rcx

.loop:
	inc r14
	cmp r14, r13
	jg .overflow
	mov byte[r12], al
	cmp rax, 0
	jz .end
	cmp rax, 0x20
	je .end
	cmp rax, 0x9
	je .end
	cmp rax, 0xA
	je .end
	inc r12
	call read_char

	jmp .loop

.overflow:
	mov rcx, r14
	dec rcx
	pop rdi
	mov rax, 0
	jmp .final

.end:
	mov rdx, r14
	dec rdx
	pop rax

.final:
	pop r14
	pop r13
	pop r12
	ret


 




; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
	push rbx 
	xor rdx, rdx
	xor rax, rax
.loop:
	mov bl, byte[rdi+rdx]
	test bl, bl
	jz .end
	cmp bl, '0'
	jl .err
	cmp bl, '9'
	jg .err
	sub rbx, '0'
	imul rax, 10
	add rax, rbx
	inc rdx
	jmp .loop
.err:
	cmp rax, 0
	jnz .end
	xor rdx, rdx
.end:
	pop rbx
	ret



; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
	cmp byte[rdi], '-'
	jne parse_uint
	inc rdi
	call parse_uint
	test rdx, rdx
	jz .end
	inc rdx
	neg rax
	.end:
	ret


; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    push rdi
	push rsi
	push rdx
	call string_length
	xor rcx, rcx
	pop rdx
	pop rsi
	pop rdi
	inc rax
	cmp rax, rdx
	jg .err
.loop:
	cmp rax, rcx
	je .end
	mov dl, byte [rdi + rcx]
	mov [rsi + rcx], dl
	inc rcx
	jmp .loop	
.end:
	ret    
.err: 
	xor rax, rax
	ret

