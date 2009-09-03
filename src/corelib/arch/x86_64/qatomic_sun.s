;/****************************************************************************
;**
;** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
;** Contact: Nokia Corporation (qt-info@nokia.com)
;**
;** This file is part of the QtGui module of the Qt Toolkit.
;**
;** $QT_BEGIN_LICENSE:LGPL$
;** No Commercial Usage
;** This file contains pre-release code and may not be distributed.
;** You may use this file in accordance with the terms and conditions
;** contained in the Technology Preview License Agreement accompanying
;** this package.
;**
;** GNU Lesser General Public License Usage
;** Alternatively, this file may be used under the terms of the GNU Lesser
;** General Public License version 2.1 as published by the Free Software
;** Foundation and appearing in the file LICENSE.LGPL included in the
;** packaging of this file.  Please review the following information to
;** ensure the GNU Lesser General Public License version 2.1 requirements
;** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
;**
;** In addition, as a special exception, Nokia gives you certain
;** additional rights.  These rights are described in the Nokia Qt LGPL
;** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
;** package.
;**
;** If you have questions regarding the use of this file, please contact
;** Nokia at qt-info@nokia.com.
;**
;**
;**
;**
;**
;**
;**
;**
;** $QT_END_LICENSE$
;**
;****************************************************************************/
        .code64

	.globl q_atomic_increment
        .type q_atomic_increment,@function
        .section .text, "ax"
        .align 16
q_atomic_increment:
	lock
	incl (%rdi)
	setne %al
	ret
	.size q_atomic_increment,.-q_atomic_increment

	.globl q_atomic_decrement
        .type q_atomic_decrement,@function
        .section .text, "ax"
        .align 16
q_atomic_decrement:
	lock
	decl (%rdi)
	setne %al
	ret
	.size q_atomic_decrement,.-q_atomic_decrement

        .globl q_atomic_test_and_set_int
        .type q_atomic_test_and_set_int, @function
        .section .text, "ax"
        .align 16
q_atomic_test_and_set_int:
        movl %esi,%eax
        lock
        cmpxchgl %edx,(%rdi)
	movl $0,%eax
        sete %al
        ret
	.size q_atomic_test_and_set_int, . - q_atomic_test_and_set_int

	.globl q_atomic_set_int
        .type q_atomic_set_int,@function
        .section .text, "ax"
        .align 16
q_atomic_set_int:
        xchgl %esi,(%rdi)
	movl %esi,%eax
	ret
	.size q_atomic_set_int,.-q_atomic_set_int

        .globl q_atomic_fetch_and_add_int
        .type q_atomic_fetch_and_add_int,@function
        .section .text, "ax"
        .align 16
q_atomic_fetch_and_add_int:
        lock
        xaddl %esi,(%rdi)
        movl %esi, %eax
        ret
        .size q_atomic_fetch_and_add_int,.-q_atomic_fetch_and_add_int

        .globl q_atomic_test_and_set_ptr
        .type q_atomic_test_and_set_ptr, @function
        .section .text, "ax"
        .align 16
q_atomic_test_and_set_ptr:
        movq %rsi,%rax
        lock
        cmpxchgq %rdx,(%rdi)
	movq $0, %rax
        sete %al
        ret
        .size q_atomic_test_and_set_ptr, . - q_atomic_test_and_set_ptr

	.globl q_atomic_set_ptr
        .type q_atomic_set_ptr,@function
        .section .text, "ax"
        .align 16
q_atomic_set_ptr:
        xchgq %rsi,(%rdi)
	movq %rsi,%rax
	ret
	.size q_atomic_set_ptr,.-q_atomic_set_ptr

        .globl q_atomic_fetch_and_add_ptr
        .type q_atomic_fetch_and_add_ptr,@function
        .section .text, "ax"
        .align 16
q_atomic_fetch_and_add_ptr:
        lock
        xaddq %rsi,(%rdi)
        movq %rsi,%rax
        ret
        .size q_atomic_fetch_and_add_ptr,.-q_atomic_fetch_and_add_ptr
