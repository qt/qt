/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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
