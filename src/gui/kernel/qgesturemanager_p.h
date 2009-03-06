/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
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

#ifndef QGESTUREMANAGER_P_H
#define QGESTUREMANAGER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qwidget.h"
#include "qlist.h"
#include "qset.h"
#include "qevent.h"
#include "qbasictimer.h"

#include "qgesturerecognizer.h"

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QGestureManager : public QObject
{
    Q_OBJECT
public:
    QGestureManager();

    // should be internal
    void setGestureTargetWidget(QWidget *widget);

    void addRecognizer(QGestureRecognizer *recognizer);
    void removeRecognizer(QGestureRecognizer *recognizer);

    bool filterEvent(QEvent *event);
    bool inGestureMode();

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void recognizerTriggered(QGestureRecognizer::Result);

private:
    QSet<QGestureRecognizer*> activeGestures;
    QMap<QGestureRecognizer*, int> maybeGestures;
    QSet<QGestureRecognizer*> recognizers;

    QWidget *targetWidget;
    QPoint lastPos;

    enum State {
        Gesture,
        NotGesture,
        MaybeGesture // that mean timers are up and waiting for some
                     // more events, and input events are handled by
                     // gesture recognizer explicitely
    } state;
};

QT_END_NAMESPACE

#endif // QGESTUREMANAGER_P_H
