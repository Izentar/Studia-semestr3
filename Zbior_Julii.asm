section .data
  msg: db "Hello", 0xa
  msg_L: equ $-msg
  check:  db "%ld", 10, 0
  check32:  db "%d", 10, 0
  checkFloat: db "%f", 10, 0

section .text

global _Zbior_Julii
global Zbior_Julii

extern printf

; macors only for check this function
; preserve all register
%macro printCheckNumberInteger 1
  push rax
  push rcx
  push rdx
  push rbx
  push rsi
  push rdi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov rsi, %1
  mov rdi, check
  xor rax, rax
  call printf

  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rbx
  pop rdx
  pop rcx
  pop rax
%endmacro

%macro printCheckNumberInteger32 1
  push rax
  push rcx
  push rdx
  push rbx
  push rsi
  push rdi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov esi, %1
  mov rdi, check32
  xor rax, rax
  call printf

  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rbx
  pop rdx
  pop rcx
  pop rax
%endmacro

%macro printCheckNumberDouble 1
  push rax
  push rcx
  push rdx
  push rbx
  push rsi
  push rdi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov rsi, number
  mov rdi, checkFloat
  mov rax, 0
  call printf

  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rbx
  pop rdx
  pop rcx
  pop rax
%endmacro

%macro sayHello 0
  push rdi
  push rsi
  push rax
  push rdx

  mov rax, 1
  mov rdi, 1
  mov rsi, msg
  mov rdx, msg_L
  syscall

  pop rdx
  pop rdi
  pop rsi
  pop rax
%endmacro
; end of checking macros

%define maxIteration 100

%define pixelSize r13
%define approxLoop rcx
%define pixelAddress r15
%define structAddress r14
%define width r8d
%define height r9d
%define copyWidth r11d
%define copyHeight r12d
%define complexWidth [rbp-96]
%define complexHeight [rbp-104]
%define ratioXlocation [rbp-64]
%define ratioYlocation [rbp-72]
%define mouseXposition [structAddress+8]
%define mouseYposition [structAddress+12]
%define ReC [structAddress+24]
%define ImC [structAddress+32]
%define nextRow [rbp-56]
%define complexPixelX [rbp-80]
%define complexPixelY [rbp-88]
%define locationOf2 [rbp-112]
%define locationOf4 [rbp-152]
%define squaredRe [rbp-120]
%define squaredIm [rbp-128]
%define modulo [rbp-136]
%define ratioColour [rbp-144]
%define locationOf2F [rbp-160]
%define locationOf4F [rbp-168]

;%define testval [structAddress+104]
;%define testval2 [structAddress+112]
;  fst qword testval

%macro setRatioWidth 0
  fld qword [structAddress+56] ;right
  fsub qword [structAddress+40] ;right-left
  fst qword complexWidth
  fidiv dword [structAddress] ; div xmm0 by integer and store in xmm0
  fstp qword ratioXlocation ;store result
%endmacro
%macro setRatioHeight 0
  fld qword [structAddress+48] ;up
  fsub qword [structAddress+64] ;up-down
  fst qword complexHeight
  fidiv dword [structAddress+4] ; div xmm0 by integer and store in xmm0
  fstp qword ratioYlocation ;store result
%endmacro

%macro convertToComplexX 1
  fild dword %1
  fmul qword ratioXlocation
  fadd qword [structAddress+40]
  fstp qword complexPixelX
%endmacro
%macro convertToComplexXreg 1
  mov complexPixelX, %1
  fild dword complexPixelX
  fmul qword ratioXlocation
  fadd qword [structAddress+40]
  fstp qword complexPixelX
%endmacro

%macro convertToComplexY 1
  fild dword %1
  fmul qword ratioYlocation
  fadd qword [structAddress+64]
  fstp qword complexPixelY
%endmacro
%macro convertToComplexYreg 1
  mov complexPixelY, %1
  fild dword complexPixelY
  fmul qword ratioYlocation
  fadd qword [structAddress+64]
  fstp qword complexPixelY
%endmacro

%macro setNewBoundaryX 0
  convertToComplexX mouseXposition

  fld qword complexWidth
  fidiv word locationOf4
  fst qword complexWidth
  fld qword complexPixelX
  faddp ;xmm0, xmm1
  fstp qword [structAddress+56]

  fld qword complexPixelX
  fsub qword complexWidth
  fstp qword [structAddress+40]
%endmacro

%macro setNewBoundaryY 0
  convertToComplexY mouseYposition

  fld qword complexHeight
  fidiv word locationOf4
  fst qword complexHeight ;tmp
  fld qword complexPixelY
  faddp ;xmm0, xmm1

  fstp qword [structAddress+48]
  fld qword complexPixelY
  fsub qword complexHeight
  fstp qword [structAddress+64]
%endmacro

%macro calculateSquared 0
  fld qword complexPixelX
  fmul qword complexPixelX
  fstp qword squaredRe

  fld qword complexPixelY
  fmul qword complexPixelY
  fstp qword squaredIm
%endmacro

%macro calculateIm 0
  fld qword complexPixelX
  fmul qword complexPixelY
  fmul qword locationOf2F
  fstp qword complexPixelY
%endmacro

%macro squaredComplex 0
  calculateSquared
  calculateIm

  fld qword squaredRe
  fsub qword squaredIm
  fstp qword complexPixelX
%endmacro

%macro moduloComplex 0
  fld qword squaredRe
  fadd qword squaredIm
  fstp qword modulo
%endmacro

%macro setRatioColour 0
  mov rax, 0x00FFFFFF
  mov rbx, maxIteration
  div rbx
  mov ratioColour, rax
%endmacro

%macro setPixelColour 0
  mov rax, ratioColour
  mul approxLoop
  mov [pixelAddress], rax
  add pixelAddress, pixelSize
%endmacro

%macro setBasicFPNumber 0
  mov word locationOf2, 2
  mov word locationOf4, 4

  fild word locationOf4
  fstp qword locationOf4F
  fild word locationOf2
  fstp qword locationOf2F
%endmacro

; address of the allegro bitmap struct
%macro preparePitch 1
  movsxd pixelSize, dword [%1+16]

  mov eax, width
  mul pixelSize ; width in bytes
  mov rbx, rax
  movsxd rax, dword [%1+12] ; pitch

  sub rax, rbx
  mov nextRow, rax; what to add to get next line in bitmap
%endmacro

_Zbior_Julii:
Zbior_Julii:
prologue:
  push rbp
  mov rbp, rsp
  push rbx  ; 8
  push r12  ;16
  push r13  ;24
  push r14  ;32
  push r15  ;40
  push rsi  ; save mouse click 48
  ;
  ;prepare before loop

  mov structAddress, rdi
  mov r11, [structAddress+16]
  mov pixelAddress, [r11]

  mov width, [structAddress]
  mov height, [structAddress+4]

  sub rsp, 120; allocate

  preparePitch r11
  setBasicFPNumber

  ; for now macros in strict order, end of preparation macros
  setRatioColour

  ; check the click
  setRatioWidth
  setRatioHeight

  ; conditions
  cmp esi, 1
  je after_convertion
  jl left_click

right_click:  ;reset to default
  mov rax, [structAddress+72]
  mov [structAddress+40], rax
  mov rax, [structAddress+80]
  mov [structAddress+48], rax
  mov rax, [structAddress+88]
  mov [structAddress+56], rax
  mov rax, [structAddress+96]
  mov [structAddress+64], rax

  setRatioWidth
  setRatioHeight

  jmp after_convertion

left_click: ;approx
  ; set new complex boundary

  setNewBoundaryX
  setNewBoundaryY

  setRatioWidth
  setRatioHeight

after_convertion:
  xor copyHeight, copyHeight

outer_loop: ; begin of outer loop
  xor copyWidth, copyWidth  ; because I have only pointer to the upper left pixel in bitmap

inner_loop:

  xor approxLoop, approxLoop
  convertToComplexXreg copyWidth
  convertToComplexYreg copyHeight

approx_loop:
  squaredComplex
  moduloComplex

  ;add const C
  fld qword complexPixelX
  fadd qword ReC
  fstp qword complexPixelX

  fld qword complexPixelY
  fadd qword ImC
  fstp qword complexPixelY

  ; conditional jumps
  fld qword modulo
  fld qword locationOf4F
  fcomip
  fstp qword modulo
  jb end_approx_loop

  inc approxLoop
  cmp approxLoop, maxIteration
  je end_approx_loop

  jmp approx_loop

end_approx_loop:
  setPixelColour

end_inner_loop:
  inc copyWidth
  cmp copyWidth, width
  jne inner_loop

end_outer_loop:
  add pixelAddress, qword nextRow

  inc copyHeight
  cmp copyHeight, height
  jne outer_loop

epilogue:
  add rsp, 128 ; trash
  pop r15
  pop r14
  pop r13
  pop r12
  pop rbx
  mov rsp, rbp
  pop rbp
  ret
