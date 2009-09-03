!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!
!! Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
!! Contact: Nokia Corporation (qt-info@nokia.com)
!!
!! This file is part of the QtGui module of the Qt Toolkit.
!!
!! $QT_BEGIN_LICENSE:LGPL$
!! No Commercial Usage
!! This file contains pre-release code and may not be distributed.
!! You may use this file in accordance with the terms and conditions
!! contained in the Technology Preview License Agreement accompanying
!! this package.
!!
!! GNU Lesser General Public License Usage
!! Alternatively, this file may be used under the terms of the GNU Lesser
!! General Public License version 2.1 as published by the Free Software
!! Foundation and appearing in the file LICENSE.LGPL included in the
!! packaging of this file.  Please review the following information to
!! ensure the GNU Lesser General Public License version 2.1 requirements
!! will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
!!
!! In addition, as a special exception, Nokia gives you certain
!! additional rights.  These rights are described in the Nokia Qt LGPL
!! Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
!! package.
!!
!! If you have questions regarding the use of this file, please contact
!! Nokia at qt-info@nokia.com.
!!
!!
!!
!!
!!
!!
!!
!!
!! $QT_END_LICENSE$
!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	.text

	.align 4,0x90
	.globl q_atomic_test_and_set_int
q_atomic_test_and_set_int:
	movl		 4(%esp),%ecx
	movl		 8(%esp),%eax
	movl		12(%esp),%edx
	lock
	cmpxchgl	%edx,(%ecx)
	mov		$0,%eax
 	sete		%al
	ret
	.align 4,0x90
	.type q_atomic_test_and_set_int,@function
	.size q_atomic_test_and_set_int,.-q_atomic_test_and_set_int

	.align 4,0x90
	.globl q_atomic_test_and_set_ptr
q_atomic_test_and_set_ptr:
	movl		 4(%esp),%ecx
	movl		 8(%esp),%eax
	movl		12(%esp),%edx
	lock 
	cmpxchgl	%edx,(%ecx)
	mov		$0,%eax
	sete		%al
	ret
	.align    4,0x90
	.type	q_atomic_test_and_set_ptr,@function
	.size	q_atomic_test_and_set_ptr,.-q_atomic_test_and_set_ptr

	.align 4,0x90
	.globl q_atomic_increment
q_atomic_increment:
	movl 4(%esp), %ecx
	lock 
	incl (%ecx)
	mov $0,%eax
	setne %al
	ret
	.align 4,0x90
	.type q_atomic_increment,@function
	.size	q_atomic_increment,.-q_atomic_increment

	.align 4,0x90
	.globl q_atomic_decrement
q_atomic_decrement:
	movl 4(%esp), %ecx
	lock 
	decl (%ecx)
	mov $0,%eax
	setne %al
	ret
	.align 4,0x90
	.type q_atomic_decrement,@function
	.size	q_atomic_decrement,.-q_atomic_decrement

	.align 4,0x90
	.globl q_atomic_set_int
q_atomic_set_int:
	mov 4(%esp),%ecx
	mov 8(%esp),%eax
	xchgl %eax,(%ecx)
	ret	
	.align 4,0x90
	.type q_atomic_set_int,@function
	.size	q_atomic_set_int,.-q_atomic_set_int

	.align 4,0x90
	.globl q_atomic_set_ptr
q_atomic_set_ptr:
	mov 4(%esp),%ecx
	mov 8(%esp),%eax
	xchgl %eax,(%ecx)
	ret	
	.align 4,0x90
	.type q_atomic_set_ptr,@function
	.size	q_atomic_set_ptr,.-q_atomic_set_ptr

        .align 4,0x90
        .globl q_atomic_fetch_and_add_int
q_atomic_fetch_and_add_int:
        mov 4(%esp),%ecx
        mov 8(%esp),%eax
        lock
        xadd %eax,(%ecx)
        ret
        .align 4,0x90
        .type q_atomic_fetch_and_add_int,@function
        .size q_atomic_fetch_and_add_int,.-q_atomic_fetch_and_add_int

        .align 4,0x90
        .globl q_atomic_fetch_and_add_ptr
q_atomic_fetch_and_add_ptr:
        mov 4(%esp),%ecx
        mov 8(%esp),%eax
        lock
        xadd %eax,(%ecx)
        ret
        .align 4,0x90
        .type q_atomic_fetch_and_add_ptr,@function
        .size q_atomic_fetch_and_add_ptr,.-q_atomic_fetch_and_add_ptr
