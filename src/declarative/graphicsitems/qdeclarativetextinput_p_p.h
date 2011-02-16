/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVETEXTINPUT_P_H
#define QDECLARATIVETEXTINPUT_P_H

#include "private/qdeclarativetextinput_p.h"

#include "private/qdeclarativepainteditem_p_p.h"

#include <qdeclarative.h>

#include <QPointer>

#include <private/qlinecontrol_p.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QT_NO_LINEEDIT

QT_BEGIN_NAMESPACE

class QDeclarativeTextInputPrivate : public QDeclarativePaintedItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeTextInput)
public:
    QDeclarativeTextInputPrivate() : control(new QLineControl(QString())),
                 color((QRgb)0), style(QDeclarativeText::Normal),
                 styleColor((QRgb)0), hAlign(QDeclarativeTextInput::AlignLeft),
                 hscroll(0), oldScroll(0), focused(false), focusOnPress(true),
                 showInputPanelOnFocus(true), clickCausedFocus(false), cursorVisible(false),
                 autoScroll(true), selectByMouse(false)
    {
#ifdef Q_OS_SYMBIAN
        if (QSysInfo::symbianVersion() == QSysInfo::SV_SF_1 || QSysInfo::symbianVersion() == QSysInfo::SV_SF_3) {
            showInputPanelOnFocus = false;
        }
#endif

    }

    ~QDeclarativeTextInputPrivate()
    {
        delete control;
    }

    int xToPos(int x, QTextLine::CursorPosition betweenOrOn = QTextLine::CursorBetweenCharacters) const
    {
        Q_Q(const QDeclarativeTextInput);
        QRect cr = q->boundingRect().toRect();
        x-= cr.x() - hscroll;
        return control->xToPos(x, betweenOrOn);
    }

    void init();
    void startCreatingCursor();
    void focusChanged(bool hasFocus);
    void updateHorizontalScroll();
    int calculateTextWidth();

    QLineControl* control;

    QFont font;
    QFont sourceFont;
    QColor  color;
    QColor  selectionColor;
    QColor  selectedTextColor;
    QDeclarativeText::TextStyle style;
    QColor  styleColor;
    QDeclarativeTextInput::HAlignment hAlign;
    QPointer<QDeclarativeComponent> cursorComponent;
    QPointer<QDeclarativeItem> cursorItem;

    int lastSelectionStart;
    int lastSelectionEnd;
    int oldHeight;
    int oldWidth;
    bool oldValidity;
    int hscroll;
    int oldScroll;
    bool focused;
    bool focusOnPress;
    bool showInputPanelOnFocus;
    bool clickCausedFocus;
    bool cursorVisible;
    bool autoScroll;
    bool selectByMouse;
};

QT_END_NAMESPACE

#endif // QT_NO_LINEEDIT

#endif

