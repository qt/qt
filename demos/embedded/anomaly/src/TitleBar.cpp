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
** contact the sales department at http://www.qtsoftware.com/contact.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "TitleBar.h"

#include <QtCore>
#include <QtGui>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , m_progress(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void TitleBar::setHost(const QString &host)
{
    m_host = host;
    update();
}

void TitleBar::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void TitleBar::setProgress(int percent)
{
    m_progress = percent;
    update();
}

QSize TitleBar::sizeHint() const
{
    return minimumSizeHint();
}

QSize TitleBar::minimumSizeHint() const
{
    QFontMetrics fm = fontMetrics();
    return QSize(100, fm.height());
}

void TitleBar::paintEvent(QPaintEvent *event)
{
    QString title = m_host;
    if (!m_title.isEmpty())
        title.append(": ").append(m_title);

    QPalette pal = palette();
    QPainter p(this);
    p.fillRect(event->rect(), pal.color(QPalette::Highlight));

    if (m_progress > 0) {

        QRect box = rect();
        box.setLeft(16);
        box.setWidth(width() - box.left() - 110);

        p.setPen(pal.color(QPalette::HighlightedText));
        p.setOpacity(0.8);
        p.drawText(box, Qt::AlignLeft + Qt::AlignVCenter, title);

        int x = width() - 100 - 5;
        int y = 1;
        int h = height() - 4;

        p.setOpacity(1.0);
        p.setBrush(Qt::NoBrush);
        p.setPen(pal.color(QPalette::HighlightedText));
        p.drawRect(x, y, 100, h);
        p.setPen(Qt::NoPen);
        p.setBrush(pal.color(QPalette::HighlightedText));
        p.drawRect(x, y, m_progress, h);
    } else {

        QRect box = rect();
        box.setLeft(16);
        box.setWidth(width() - box.left() - 5);
        p.setPen(pal.color(QPalette::HighlightedText));
        p.drawText(box, Qt::AlignLeft + Qt::AlignVCenter, title);
    }

    p.end();
}
