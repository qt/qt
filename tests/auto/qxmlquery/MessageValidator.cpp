/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifdef QTEST_XMLPATTERNS
#include <QXmlStreamReader>

#include "MessageValidator.h"

MessageValidator::MessageValidator() : m_success(false)
                                     , m_hasChecked(false)
{
}

MessageValidator::~MessageValidator()
{
    Q_ASSERT_X(m_hasChecked,
               Q_FUNC_INFO,
               "You must call success().");
}

void MessageValidator::handleMessage(QtMsgType type,
                                     const QString &description,
                                     const QUrl &identifier,
                                     const QSourceLocation &sourceLocation)
{
    Q_UNUSED(type);
    Q_UNUSED(description);
    Q_UNUSED(sourceLocation);
    Q_UNUSED(identifier);

    QXmlStreamReader reader(description);

    m_received =   QLatin1String("Type:")
                 + QString::number(type)
                 + QLatin1String("\nDescription: ")
                 + description
                 + QLatin1String("\nIdentifier: ")
                 + identifier.toString()
                 + QLatin1String("\nLocation: ")
                 + sourceLocation.uri().toString()
                 + QLatin1String("#")
                 + QString::number(sourceLocation.line())
                 + QLatin1String(",")
                 + QString::number(sourceLocation.column());

    /* We just walk through it, to check that it's valid. */
    while(!reader.atEnd())
        reader.readNext();

    m_success = !reader.hasError();
}

bool MessageValidator::success()
{
    m_hasChecked = true;
    return m_success;
}

QString MessageValidator::received() const
{
    return m_received;
}

#endif //QTEST_XMLPATTERNS
