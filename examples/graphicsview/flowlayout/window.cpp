/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtGui/qgraphicsproxywidget.h>
#include <QtGui/qlabel.h>
#include "flowlayout.h"
#include "window.h"

Window::Window()
: QGraphicsWidget(0, Qt::Window)
{
    FlowLayout *lay = new FlowLayout;
    QLatin1String wiseWords("I am not bothered by the fact that I am unknown."
    " I am bothered when I do not know others. (Confucius)");
    QString sentence(wiseWords);
    QStringList words = sentence.split(QLatin1Char(' '), QString::SkipEmptyParts);
    for (int i = 0; i < words.count(); ++i) {
        QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
        QLabel *label = new QLabel(words.at(i));
        label->setFrameStyle(QFrame::Box | QFrame::Plain);
        proxy->setWidget(label);
        lay->addItem(proxy);
    }
    setLayout(lay);
}
