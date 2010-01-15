/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
QToolButton *button = new QToolButton;
button->setIcon(QIcon("open.xpm"));
//! [0]


//! [1]
button->setIcon(QIcon());
//! [1]


//! [2]
void MyWidget::drawIcon(QPainter *painter, QPoint pos)
{
    QPixmap pixmap = icon.pixmap(QSize(22, 22),
                                   isEnabled() ? QIcon::Normal
                                               : QIcon::Disabled,
                                   isOn() ? QIcon::On
                                          : QIcon::Off);
    painter->drawPixmap(pos, pixmap);
}
//! [2]

//! [3]
    QIcon undoicon = QIcon::fromTheme("edit-undo");
//! [3]

//! [4]
    QIcon undoicon = QIcon::fromTheme("edit-undo", QIcon(":/undo.png"));
//! [4]

