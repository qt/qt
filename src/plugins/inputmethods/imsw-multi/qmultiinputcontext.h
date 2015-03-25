/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

/****************************************************************************
**
** Definition of QMultiInputContext class
**
** Copyright (C) 2004 immodule for Qt Project.  All rights reserved.
**
** This file is written to contribute to Nokia Corporation and/or its subsidiary(-ies) under their own
** license. You may use this file under your Qt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef QMULTIINPUTCONTEXT_H
#define QMULTIINPUTCONTEXT_H

#ifndef QT_NO_IM

#include <QtGui/qwidget.h>
#include <QtGui/qinputcontext.h>
#include <QtCore/qstring.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qmap.h>
#include <QtCore/qpointer.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QMultiInputContext : public QInputContext
{
    Q_OBJECT
public:
    QMultiInputContext();
    ~QMultiInputContext();

    QString identifierName();
    QString language();

#if defined(Q_WS_X11)
    bool x11FilterEvent( QWidget *keywidget, XEvent *event );
#endif // Q_WS_X11
    bool filterEvent( const QEvent *event );

    void reset();
    void update();
    void mouseHandler( int x, QMouseEvent *event );
    QFont font() const;
    bool isComposing() const;

    QList<QAction *> actions();

    QWidget *focusWidget() const;
    void setFocusWidget(QWidget *w);

    void widgetDestroyed( QWidget *w );

    QInputContext *slave() { return slaves.at(current); }
    const QInputContext *slave() const { return slaves.at(current); }

protected Q_SLOTS:
    void changeSlave(QAction *);
private:
    void *unused;
    int current;
    QList<QInputContext *> slaves;
    QMenu *menu;
    QAction *separator;
    QStringList keys;
};

#endif // Q_NO_IM

QT_END_NAMESPACE

#endif // QMULTIINPUTCONTEXT_H
