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

#include "AddressBar.h"

#include <QtCore>
#include <QtGui>

class LineEdit: public QLineEdit
{
public:
    LineEdit(QWidget *parent = 0): QLineEdit(parent) {}

    void paintEvent(QPaintEvent *event) {
        QLineEdit::paintEvent(event);
        if (text().isEmpty()) {
            QPainter p(this);
            int flags = Qt::AlignLeft | Qt::AlignVCenter;
            p.setPen(palette().color(QPalette::Disabled, QPalette::Text));
            p.drawText(rect().adjusted(10, 0, 0, 0), flags, "Enter address or search terms");
            p.end();
        }
    }
};

AddressBar::AddressBar(QWidget *parent)
    : QWidget(parent)
{
    m_lineEdit = new LineEdit(parent);
    connect(m_lineEdit, SIGNAL(returnPressed()), SLOT(processAddress()));
    m_toolButton = new QToolButton(parent);
    m_toolButton->setText("Go");
    connect(m_toolButton, SIGNAL(clicked()), SLOT(processAddress()));
}

QSize AddressBar::sizeHint() const
{
    return m_lineEdit->sizeHint();
}

void AddressBar::processAddress()
{
    if (!m_lineEdit->text().isEmpty())
        emit addressEntered(m_lineEdit->text());
}

void AddressBar::resizeEvent(QResizeEvent *event)
{
    int x, y, w, h;

    m_toolButton->adjustSize();
    x = width() - m_toolButton->width();
    y = 0;
    w = m_toolButton->width();
    h = height() - 1;
    m_toolButton->setGeometry(x, y, w, h);
    m_toolButton->show();

    x = 0;
    y = 0;
    w = width() - m_toolButton->width();
    h = height() - 1;
    m_lineEdit->setGeometry(x, y, w, h);
    m_lineEdit->show();
}

void AddressBar::focusInEvent(QFocusEvent *event)
{
    m_lineEdit->setFocus();
    QWidget::focusInEvent(event);
}
