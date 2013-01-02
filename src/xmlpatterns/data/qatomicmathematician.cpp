/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qatomicmathematician_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

AtomicMathematician::~AtomicMathematician()
{
}

QString AtomicMathematician::displayName(const AtomicMathematician::Operator op)
{
    switch(op)
    {
        case AtomicMathematician::Div:
            return QLatin1String("div");
        case AtomicMathematician::IDiv:
            return QLatin1String("idiv");
        case AtomicMathematician::Substract:
            return QLatin1String("-");
        case AtomicMathematician::Mod:
            return QLatin1String("mod");
        case AtomicMathematician::Multiply:
            return QLatin1String("*");
        case AtomicMathematician::Add:
            return QLatin1String("+");
    }

    return QString(); /* Silence GCC warning. */
}

QT_END_NAMESPACE
