/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/
#include "tracer.h"

#include "topicchooser.h"

QT_BEGIN_NAMESPACE

TopicChooser::TopicChooser(QWidget *parent, const QString &keyword,
                         const QMap<QString, QUrl> &links)
    : QDialog(parent)
{
    TRACE_OBJ
    ui.setupUi(this);
    ui.label->setText(tr("Choose a topic for <b>%1</b>:").arg(keyword));

    QMap<QString, QUrl>::const_iterator it = links.constBegin();
    for (; it != links.constEnd(); ++it) {
        ui.listWidget->addItem(it.key());
        m_links.append(it.value());
    }
    
    if (ui.listWidget->count() != 0)
        ui.listWidget->setCurrentRow(0);
    ui.listWidget->setFocus();

    connect(ui.buttonDisplay, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui.buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui.listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this,
        SLOT(accept()));
}

QUrl TopicChooser::link() const
{
    TRACE_OBJ
    QListWidgetItem *item = ui.listWidget->currentItem();
    if (!item)
        return QUrl();

    QString title = item->text();
    if (title.isEmpty())
        return QUrl();

    const int row = ui.listWidget->row(item);
    Q_ASSERT(row < m_links.count());
    return m_links.at(row);
}

QT_END_NAMESPACE
