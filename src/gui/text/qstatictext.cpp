/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qstatictext.h"
#include "qstatictext_p.h"

QT_BEGIN_NAMESPACE

QStaticText::QStaticText(const QString &text, const QFont &font)
    : d_ptr(new QStaticTextPrivate) 
{
    Q_D(QStaticText);
    d->init(text, font);
}

QStaticTextPrivate *QStaticTextPrivate::get(const QStaticText *q)
{
    return q->d_ptr;
}

void QStaticTextPrivate::init(const QString &text, const QFont &font)
{
    engine.text = text; 
    engine.fnt = font;

    /*engine.option.setTextDirection(d->state->layoutDirection);
    if (tf & (Qt::TextForceLeftToRight|Qt::TextForceRightToLeft)) {
        engine.ignoreBidi = true;
        engine.option.setTextDirection((tf & Qt::TextForceLeftToRight) ? Qt::LeftToRight : Qt::RightToLeft);
    }*/

    engine.itemize();
    QScriptLine line;
    line.length = text.length();
    engine.shapeLine(line);

    int nItems = engine.layoutData->items.size();
    visualOrder = QVarLengthArray<int>(nItems);
    QVarLengthArray<uchar> levels(nItems);
    for (int i = 0; i < nItems; ++i)
        levels[i] = engine.layoutData->items[i].analysis.bidiLevel;
    QTextEngine::bidiReorder(nItems, levels.data(), visualOrder.data());

    /*if (justificationPadding > 0) {
        engine.option.setAlignment(Qt::AlignJustify);
        engine.forceJustification = true;
        // this works because justify() is only interested in the difference between width and textWidth
        line.width = justificationPadding;
        engine.justify(line);
    }*/
}

QT_END_NAMESPACE
