/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "previewwidget.h"
#include <QtEvents>

QT_BEGIN_NAMESPACE

PreviewWidget::PreviewWidget( QWidget *parent, const char *name )
    : PreviewWidgetBase( parent, name )
{
    // install event filter on child widgets
    QObjectList l = queryList("QWidget");
    for (int i = 0; i < l.size(); ++i) {
        QObject * obj = l.at(i);
        obj->installEventFilter(this);
        ((QWidget*)obj)->setFocusPolicy(Qt::NoFocus);
    }
}


void PreviewWidget::closeEvent(QCloseEvent *e)
{
    e->ignore();
}


bool PreviewWidget::eventFilter(QObject *, QEvent *e)
{
    switch ( e->type() ) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::Enter:
    case QEvent::Leave:
        return true; // ignore;
    default:
        break;
    }
    return false;
}

QT_END_NAMESPACE
