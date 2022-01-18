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
PUBLIC yield, lowLevelEnqueueTask, lowLevelGetCurrentStack
EXTERN taskManagerYield:PROC, onTaskFinished:PROC
;----------------------------------------------------------------------------
; Shadow Space (see https://docs.microsoft.com/en-us/cpp/build/stack-usage?view=msvc-160 ) 
SHADOWSIZE equ 32
;----------------------------------------------------------------------------
; Align stack at 16 (see https://docs.microsoft.com/en-us/cpp/build/stack-usage?view=msvc-160 ) 
alignstack macro
    and     spl, 0f0h
    endm
;----------------------------------------------------------------------------
pushmmx macro mmxreg
    sub     rsp, 16
    movdqu  [rsp], mmxreg
    endm
;----------------------------------------------------------------------------
popmmx macro mmxreg
    movdqu  mmxreg, [rsp]
    add     rsp, 16
    endm
;----------------------------------------------------------------------------
; See https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-160#callercallee-saved-registers
pushall macro
    push    rbx
    push    rsi
    push    rdi
    push    r12
    push    r13
    push    r14
    push    r15
    pushmmx xmm6
    pushmmx xmm7
    pushmmx xmm8
    pushmmx xmm9
    pushmmx xmm10
    pushmmx xmm11
    pushmmx xmm12
    pushmmx xmm13
    pushmmx xmm14
    pushmmx xmm15
    endm
;----------------------------------------------------------------------------
popall  macro
    popmmx  xmm15
    popmmx  xmm14
    popmmx  xmm13
    popmmx  xmm12
    popmmx  xmm11
    popmmx  xmm10
    popmmx  xmm9
    popmmx  xmm8
    popmmx  xmm7
    popmmx  xmm6
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rdi
    pop     rsi
    pop     rbx
    endm
;----------------------------------------------------------------------------
.data
; no data
;----------------------------------------------------------------------------
.code
;----------------------------------------------------------------------------
; Get current stack pointer to provide it in C++ code
lowLevelGetCurrentStack PROC
    mov     rax, rsp
    ret
lowLevelGetCurrentStack ENDP
;----------------------------------------------------------------------------
; Task entry code is used to prepare an input parameter in RCX
taskEntry PROC
    pop     rdx             ; Target task function address
    pop     rcx             ; Argument pointer
    enter   SHADOWSIZE, 0   
    alignstack        

    call    rdx

    leave                   ; Restore stack (rsp) & frame pointer (rbp)
    ret
taskEntry ENDP
;----------------------------------------------------------------------------
; Should be used from MAIN context to add a new task to task dispatcher
lowLevelEnqueueTask PROC
    mov     rax, rbp
    enter   SHADOWSIZE, 0
    alignstack

    ; rcx - pointer to function
    ; rdx - void* data
    ; r8  - pointer to function stack
    ; returns  - address of a new host's stack pointer
    mov     rsp, r8
    sub     rsp, SHADOWSIZE ; THIS SPACE IN TASK STACK IS REALLY USED!
    alignstack
    ; onTaskFinished is a kind of "completion" which is startet at the final stage of task.
    lea     r8, onTaskFinished
    push    r8
    push    rdx
    push    rcx
    ; task entry code
    lea     r8, taskEntry
    push    r8
    pushall
    push    rax
    mov     rax, rsp

    leave                   ; Restore stack (rsp) & frame pointer (rbp)
    ret
lowLevelEnqueueTask ENDP
;----------------------------------------------------------------------------
; Get a new stack pointer from passed argument (rcx) and switch the stack
; to return into a different task
lowLevelResume PROC
    mov     rsp, rcx ; here is a stack pointer address
    pop     rbp
    popall
    ret
lowLevelResume ENDP
;----------------------------------------------------------------------------
; void yield() is used to switch task. Should be called from running task.
; It is also used to run the initial task from main context
yield PROC
    pushall
    enter   0, 0
    mov     rcx, rsp
    sub     rsp, SHADOWSIZE
    alignstack
    
    call    taskManagerYield

    leave                   ; Restore stack (rsp) & frame pointer (rbp)
    popall
    ret
yield ENDP
;----------------------------------------------------------------------------
END
