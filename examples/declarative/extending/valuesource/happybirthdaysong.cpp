/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "happybirthdaysong.h"
#include <QTimer>

HappyBirthdaySong::HappyBirthdaySong(QObject *parent)
: QObject(parent), m_line(-1)
{
    setName(QString());
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(advance()));
    timer->start(1000);
}

void HappyBirthdaySong::setTarget(const QDeclarativeProperty &p)
{
    m_target = p;
}

QString HappyBirthdaySong::name() const
{
    return m_name;
}

void HappyBirthdaySong::setName(const QString &name)
{
    m_name = name;

    m_lyrics.clear();
    m_lyrics << "Happy birthday to you,";
    m_lyrics << "Happy birthday to you,";
    m_lyrics << "Happy birthday dear " + m_name + ",";
    m_lyrics << "Happy birthday to you!";
    m_lyrics << "";
}
    
void HappyBirthdaySong::advance()
{
    m_line = (m_line + 1) % m_lyrics.count();

    m_target.write(m_lyrics.at(m_line));
}

