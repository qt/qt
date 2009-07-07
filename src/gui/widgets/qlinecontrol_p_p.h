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

#ifndef QLINECONTROL_P_P_H
#define QLINECONTROL_P_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qobject_p.h"
#include "QtGui/qtextlayout.h"

QT_BEGIN_NAMESPACE

class QMimeData;
class QAbstractScrollArea;
class QInputContext;

class QLineControlPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QLineControl)
public:
    QLineControlPrivate()
        : cursor(0), preeditCursor(0), cursorWidth(1), cursorTimer(0),
        maxLength(32767), modifiedState(0), undoState(0),
        selStart(0), selEnd(0), textInteractionFlags(0),
        cursorVisible(0), hideCursor(0), echoMode(0)
    {}

    QPalette palette;
    QVector<QLineControl::Action> history;
    QTextLayout textLayout; // display text
    QString text;

    int cursor;
    int preeditCursor;
    int lastCursorPos;
    int cursorWidth;
    int cursorTimer;

    int maxLength;

    int modifiedState;
    int undoState;

    int selStart, selEnd;

    Qt::TextInteractionFlags textInteractionFlags;

    // keep together and at end
    uint cursorVisible : 1;
    uint hideCursor : 1; // used to hide the cursor inside preedit areas
    uint echoMode : 2;
};

QT_END_NAMESPACE

#endif // QLINECONTROL_P_H

