; Copyright (c) 2021, 2025 Alexander Galilov, alexander.galilov@gmail.com
; This code is a part of my cooperative multitasking (fibers) DEMO for x64 C++ (Visual Studio 2019)
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
.MODEL FLAT, C

;----------------------------------------------------------------------------
; See https://www.agner.org/optimize/calling_conventions.pdf and
; https://docs.microsoft.com/en-us/cpp/cpp/stdcall?view=msvc-160
pushall macro
    push    ebx
    push    esi
    push    edi
    endm
;----------------------------------------------------------------------------
; See https://www.agner.org/optimize/calling_conventions.pdf and
; https://docs.microsoft.com/en-us/cpp/cpp/stdcall?view=msvc-160
popall  macro
    pop     edi
    pop     esi
    pop     ebx
    endm
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
.code
PUBLIC yield, lowLevelEnqueueFiber, lowLevelGetCurrentStack
fiberManagerYield PROTO stackPointer:PTR
onFiberFinished PROTO
;----------------------------------------------------------------------------
; Get current stack pointer to provide it in C++ code
lowLevelGetCurrentStack PROC
    mov     eax, esp
    ret
lowLevelGetCurrentStack ENDP
;----------------------------------------------------------------------------
; Should be used from MAIN context to add a new fiber to fiber dispatcher
OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE
lowLevelEnqueueFiber PROC pFunc:PTR, pData:PTR, pStack:PTR
    mov     eax, ebp
    push    ebp
    mov     ebp, esp
    ; ecx <- pointer to function
    mov     ecx, pFunc
    ; edx <- void* data 
    mov     edx, pData
    ; esp <- pointer to function stack
    mov     esp, pStack
    push    edx
    ; onFiberFinished is a kind of "completion" which is started at the final stage of fiber.
    push    onFiberFinished
    push    ecx
    pushall
    push    eax
    mov     eax, esp    ; returns  - address of a new host's stack pointer in eax
    leave
    ret
lowLevelEnqueueFiber ENDP
OPTION PROLOGUE:PROLOGUEDEF 
OPTION EPILOGUE:EPILOGUEDEF
;----------------------------------------------------------------------------
; Get a new stack pointer from passed argument and switch the stack
; to return into a different fiber
OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE
lowLevelResume PROC
    mov     esp, [esp + 4] ; here is a stack pointer address
    pop     ebp
    popall
    ret
lowLevelResume ENDP
OPTION PROLOGUE:PROLOGUEDEF 
OPTION EPILOGUE:EPILOGUEDEF
;----------------------------------------------------------------------------
; void yield() is used to switch fiber. Should be called from running fiber.
; It is also used to run the initial fiber from main context
yield PROC
    pushall
    push    ebp
    mov     eax, esp
    invoke  fiberManagerYield, eax
    pop     ebp
    popall
    ret
yield ENDP
;----------------------------------------------------------------------------
END
