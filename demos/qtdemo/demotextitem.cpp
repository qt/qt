/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "demotextitem.h"
#include "colors.h"

DemoTextItem::DemoTextItem(const QString &text, const QFont &font, const QColor &textColor,
                           float textWidth, QGraphicsScene *scene, QGraphicsItem *parent, TYPE type, const QColor &bgColor)
                           : DemoItem(scene, parent)
{
    this->type = type;
    this->text = text;
    this->font = font;
    this->textColor = textColor;
    this->bgColor = bgColor;
    this->textWidth = textWidth;
    this->noSubPixeling = true;
}

void DemoTextItem::setText(const QString &text)
{
    this->text = text;
    this->update();
}

QImage *DemoTextItem::createImage(const QMatrix &matrix) const
{
    if (this->type == DYNAMIC_TEXT)
        return 0;

    float sx = qMin(matrix.m11(), matrix.m22());
    float sy = matrix.m22() < sx ? sx : matrix.m22();

    QGraphicsTextItem textItem(0, 0);
    textItem.setHtml(this->text);
    textItem.setTextWidth(this->textWidth);
    textItem.setFont(this->font);
    textItem.setDefaultTextColor(this->textColor);
    textItem.document()->setDocumentMargin(2);

    float w = textItem.boundingRect().width();
    float h = textItem.boundingRect().height();
    QImage *image = new QImage(int(w * sx), int(h * sy), QImage::Format_ARGB32_Premultiplied);
    image->fill(QColor(0, 0, 0, 0).rgba());
    QPainter painter(image);
    painter.scale(sx, sy);
    QStyleOptionGraphicsItem style;
    textItem.paint(&painter, &style, 0);
    return image;
}


void DemoTextItem::animationStarted(int)
{
    this->noSubPixeling = false;
}


void DemoTextItem::animationStopped(int)
{
    this->noSubPixeling = true;
}

QRectF DemoTextItem::boundingRect() const

{
    if (this->type == STATIC_TEXT)
        return DemoItem::boundingRect();
    return QRectF(0, 0, 50, 20); // Sorry for using magic number
}


void DemoTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (this->type == STATIC_TEXT) {
        DemoItem::paint(painter, option, widget);
        return;
    }

    painter->setPen(this->textColor);
    painter->drawText(0, 0, this->text);
}
