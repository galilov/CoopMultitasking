; Copyright (c) 2021, 2025 Alexander Galilov, alexander.galilov@gmail.com
; This code is a part of my cooperative multitasking (fibers) DEMO for 32bit x86 C++ (Visual Studio 2019)
;
; Thanks to Henk-Jan Lebbink for AsmDude plugin:
; https://marketplace.visualstudio.com/items?itemName=Henk-JanLebbink.AsmDude
;
; Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
; and associated documentation files (the "Software"), to deal in the Software without restriction,
; including without limitation the rights to use, copy, modify, merge, publish, distribute,
; sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
; furnished to do so, subject to the following conditions:
; The above copyright notice and this permission notice shall be included in all copies or 
; substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
; NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
; NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
; DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;
.686
.MODEL FLAT, C ; use cdecl convention

;----------------------------------------------------------------------------
; See https://www.agner.org/optimize/calling_conventions.pdf and
; https://learn.microsoft.com/en-us/cpp/cpp/cdecl?view=msvc-160
; Non-vloatile registers are EBX, EBP, ESP, EDI, ESI, CS and DS
pushall macro
    push    ebx
    push    esi
    push    edi
    endm
;----------------------------------------------------------------------------
; See https://www.agner.org/optimize/calling_conventions.pdf and
; https://learn.microsoft.com/en-us/cpp/cpp/cdecl?view=msvc-160
; Non-vloatile registers are EBX, EBP, ESP, EDI, ESI, CS and DS
popall  macro
    pop     edi
    pop     esi
    pop     ebx
    endm
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
.code
PUBLIC yield, lowLevelEnqueueFiber, lowLevelGetCurrentStack ; exported names
fiberManagerYield PROTO stackPointer:PTR    ; the function is defined in C-code
onFiberFinished PROTO                       ; the function is defined in C-code
;----------------------------------------------------------------------------
; Get current stack pointer to provide it in C++ code
OPTION PROLOGUE:NONE ; do not generate prologue
OPTION EPILOGUE:NONE ; do not generate epilogue
lowLevelGetCurrentStack PROC
    mov     eax, esp
    ret
lowLevelGetCurrentStack ENDP
OPTION PROLOGUE:PROLOGUEDEF ; use default prologue
OPTION EPILOGUE:EPILOGUEDEF ; and epilogue
;----------------------------------------------------------------------------
; Should be used from MAIN context to add a new fiber to fiber dispatcher
; returns new stack pointer in eax
lowLevelEnqueueFiber PROC pFunc:PTR, pData:PTR, pStack:PTR
    ; we use an automcatically generated prologue which prepares the stack frame
    mov     eax, pFunc
    mov     ecx, pData
    mov     esp, pStack ; prepare the top of stack for a new fiber
    push    ecx         ; point to void* parameter will passed to the fiber via stack
    ; onFiberFinished is handler which is called at the fiber completion stage
    push    onFiberFinished
    push    eax         ; pointer to a fiber function
    ; allocate stack space to popping edi, esi, ebx, ebp in lowLevelResume()
    push    0
    push    0
    push    0
    push    0
    mov     eax, esp    ; result - address of a new host's stack pointer in eax.
    ret                 ; here we use automatically generated epilogue
lowLevelEnqueueFiber ENDP
;----------------------------------------------------------------------------
; Get a new stack pointer from passed argument and switch the stack
; to return into a different fiber
OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE
lowLevelResume PROC
    mov     esp, [esp + 4] ; update esp with a new address taken from a parameter passed via stack
    ; extract previously saved non-volatile registers
    pop     ebp
    popall
    ret
lowLevelResume ENDP
OPTION PROLOGUE:PROLOGUEDEF ; use default prologue
OPTION EPILOGUE:EPILOGUEDEF ; and epilogue
;----------------------------------------------------------------------------
; void yield() is used to switch fiber. Should be called from running fiber.
; It is also used to run the initial fiber from main context
OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE
yield PROC
    pushall
    push    ebp
    mov     eax, esp
    ; fiberManagerYield(sp) switches the stack to another fiber
    invoke  fiberManagerYield, esp ; pass a stack pointer to fiberManagerYield as parameter
    pop     ebp
    popall
    ret
yield ENDP
OPTION PROLOGUE:PROLOGUEDEF ; use default prologue
OPTION EPILOGUE:EPILOGUEDEF ; and epilogue
;----------------------------------------------------------------------------
END
