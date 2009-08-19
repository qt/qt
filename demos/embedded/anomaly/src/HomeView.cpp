/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Anomaly project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
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
