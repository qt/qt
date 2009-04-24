/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#ifndef QATOMIC_SYMBIAN_H
#define QATOMIC_SYMBIAN_H

QT_BEGIN_HEADER

#if defined(Q_CC_RVCT)
#  define QT_NO_ARM_EABI
#  include <QtCore/qatomic_arm.h>
#elif defined(Q_CC_NOKIAX86) || defined(Q_CC_GCCE)
#  include <QtCore/qatomic_generic.h>
#endif

QT_END_HEADER

#endif // QATOMIC_SYMBIAN_H
