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

#include "BookmarksView.h"

#include <QtGui>

BookmarksView::BookmarksView(QWidget *parent)
    : QWidget(parent)
{
    QListWidget *m_iconView = new QListWidget(this);
    connect(m_iconView, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(activate(QListWidgetItem*)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    layout->addWidget(m_iconView);

    m_iconView->addItem("www.google.com");
    m_iconView->addItem("qt.nokia.com/doc/4.5");
    m_iconView->addItem("news.bbc.co.uk/text_only.stm");
    m_iconView->addItem("mobile.wikipedia.org");
    m_iconView->addItem("qt.nokia.com");
    m_iconView->addItem("en.wikipedia.org");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void BookmarksView::activate(QListWidgetItem *item)
{
    QUrl url = item->text().prepend("http://");
    emit urlSelected(url);
}
