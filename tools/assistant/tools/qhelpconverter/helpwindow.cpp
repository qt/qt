/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QTextEdit>

#include "helpwindow.h"

QT_BEGIN_NAMESPACE

HelpWindow::HelpWindow(QWidget *parent)
    : QWidget(parent, 0)
{
    setAutoFillBackground(true);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    QFrame *frame = new QFrame(this);
    QPalette p = palette();
    p.setColor(QPalette::Background, Qt::white);
    setPalette(p);
    frame->setFrameStyle(QFrame::Box | QFrame::Plain);
    layout->addWidget(frame);

    layout = new QVBoxLayout(frame);
    layout->setMargin(2);
    QLabel *l = new QLabel(tr("<center><b>Wizard Assistant</b></center>"));
    layout->addWidget(l);
    m_textEdit = new QTextEdit();
    m_textEdit->setFrameStyle(QFrame::NoFrame);
    m_textEdit->setReadOnly(true);
    layout->addWidget(m_textEdit);
}

void HelpWindow::setHelp(const QString &topic)
{
    QLatin1String fileStr(":/trolltech/qhelpconverter/doc/%1.html");
    QFile f(QString(fileStr).arg(topic.toLower()));
    f.open(QIODevice::ReadOnly);
    QTextStream s(&f);
    m_textEdit->setText(s.readAll());
}

QT_END_NAMESPACE
