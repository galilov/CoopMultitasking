; Copyright (c) 2021 Alexander Galilov, alexander.galilov@gmail.com
; This code is a part of my cooperative multitasking DEMO for x64 C++ (Visual Studio 2019)
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
popall  macro
    pop     edi
    pop     esi
    pop     ebx
    endm
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
.code
PUBLIC yield, lowLevelEnqueueTask, lowLevelGetCurrentStack
taskManagerYield PROTO stackPointer:PTR
onTaskFinished PROTO
;----------------------------------------------------------------------------
; Get current stack pointer to provide it in C++ code
lowLevelGetCurrentStack PROC
    mov     eax, esp
    ret
lowLevelGetCurrentStack ENDP
;----------------------------------------------------------------------------
; Should be used from MAIN context to add a new task to task dispatcher
OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE
lowLevelEnqueueTask PROC pFunc:PTR, pData:PTR, pStack:PTR
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
    ; onTaskFinished is a kind of "completion" which is startet at the final stage of task.
    push    onTaskFinished
    push    ecx
    pushall
    push    eax
    mov     eax, esp    ; returns  - address of a new host's stack pointer in eax
    leave
    ret
lowLevelEnqueueTask ENDP
OPTION PROLOGUE:PROLOGUEDEF 
OPTION EPILOGUE:EPILOGUEDEF
;----------------------------------------------------------------------------
; Get a new stack pointer from passed argument and switch the stack
; to return into a different task
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
; void yield() is used to switch task. Should be called from running task.
; It is also used to run the initial task from main context
yield PROC
    pushall
    push    ebp
    mov     eax, esp
    invoke  taskManagerYield, eax
    pop     ebp
    popall
    ret
yield ENDP
;----------------------------------------------------------------------------
END
