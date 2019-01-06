
najpierw: nasm -f elf64 Zbior_Julii.asm

potem: gcc -m64 -no-pie -O3 main.c Zbior_Julii.o $(pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 --libs --cflags)

elf64 - kompilacja przez nasm jako 64bit program
m64 - jako kompilacja i linkowanie programu 64bit
-no-pie - dla instrukcji printf (PIC/PIE problem)
-O3 - całkowita optymalizacja
