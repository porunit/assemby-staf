ASM_FLAGS=-f elf64
N = nasm
.PHONY: clean
clean:
	rm -f *.o

lib.o: lib.asm
	$(N) $(ASM_FLAGS) lib.asm -o $@

dict.o: dict.asm lib.inc colon.inc
	$(N) $(ASM_FLAGS) dict.asm -o $@

main.o: main.asm lib.inc dict.inc words.inc
	$(N) $(ASM_FLAGS) main.asm -o $@

program: main.o lib.o dict.o
	ld -o $@ $^

test: test.py
	python3 $<
