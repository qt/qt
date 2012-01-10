/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QObject>
#include <QSlider>
#include <QScrollBar>

#include <QtCore/qplugin.h>
#include "qtactileFeedback.h"

#include <touchfeedback.h>

void QTactileFeedback::touchFeedback(QEvent *event, const QWidget *widget)
{
    //Lets share the global instance for touch feedback (you are NOT allowed to try and delete it!).
    MTouchFeedback* feedback = MTouchFeedback::Instance();

    //If the widget itself is not handling focus, try to use focusProxy widget.
    const QWidget *w = ((widget->focusPolicy() == Qt::NoFocus) && (widget->focusProxy())) ? widget->focusProxy() : widget;

    //Only give tactile feedback for enabled widgets that take focus.
    if (feedback && w && w->isEnabled() && w->isWidgetType() && w->isVisible()) {
        //Scrollbars are 'special' that they don't take focus (nor they have focusProxy), yet we'd like to have tactile feedback for them
        if (w->focusPolicy() == Qt::NoFocus)
            if (!qobject_cast<const QScrollBar *>(w))
                return;

        //Don't give tactile feedback for widgets that are outside topmost dialog.
        QWidget *dialog = QApplication::activeModalWidget();
        if (dialog) {
            QList<const QWidget *> allChildren = dialog->findChildren<const QWidget *>();
            if (!allChildren.contains(w))
                return;
        }

        //Widget specific tactile feedback.
        if (qobject_cast<const QSlider *>(w) || qobject_cast<const QScrollBar *>(w))
            feedback->InstantFeedback(ETouchFeedbackSensitive);
        else
            feedback->InstantFeedback(ETouchFeedbackBasic);
    }
}

Q_EXPORT_PLUGIN2("feedback", QTactileFeedback);
