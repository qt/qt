/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demos of the Qt Toolkit.
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

#include "HomeView.h"

#include <QtCore>
#include <QtGui>

#include "AddressBar.h"
#include "BookmarksView.h"

HomeView::HomeView(QWidget *parent)
    : QWidget(parent)
    , m_addressBar(0)
{
    m_addressBar = new AddressBar(parent);
    connect(m_addressBar, SIGNAL(addressEntered(QString)), SLOT(gotoAddress(QString)));

    m_bookmarks = new BookmarksView(parent);
    connect(m_bookmarks, SIGNAL(urlSelected(QUrl)), SIGNAL(urlActivated(QUrl)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(4);
    layout->setSpacing(4);
    layout->addWidget(m_addressBar);
    layout->addWidget(m_bookmarks);
}

void HomeView::gotoAddress(const QString &address)
{
    emit addressEntered(address);
}

void HomeView::focusInEvent(QFocusEvent *event)
{
    m_addressBar->setFocus();
    QWidget::focusInEvent(event);
}
