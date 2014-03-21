/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QWSMANAGER_QWS_H
#define QWSMANAGER_QWS_H

#include <QtGui/qpixmap.h>
#include <QtCore/qobject.h>
#include <QtGui/qdecoration_qws.h>
#include <QtGui/qevent.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_QWS_MANAGER

class QAction;
class QPixmap;
class QWidget;
class QPopupMenu;
class QRegion;
class QMouseEvent;
class QWSManagerPrivate;

class Q_GUI_EXPORT QWSManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QWSManager)
public:
    explicit QWSManager(QWidget *);
    ~QWSManager();

    static QDecoration *newDefaultDecoration();

    QWidget *widget();
    static QWidget *grabbedMouse();
    void maximize();
    void startMove();
    void startResize();

    QRegion region();
    QRegion &cachedRegion();

protected Q_SLOTS:
    void menuTriggered(QAction *action);

protected:
    void handleMove(QPoint g);

    virtual bool event(QEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);
    bool repaintRegion(int region, QDecoration::DecorationState state);

    void menu(const QPoint &);

private:
    friend class QWidget;
    friend class QETWidget;
    friend class QWidgetPrivate;
    friend class QApplication;
    friend class QApplicationPrivate;
    friend class QWidgetBackingStore;
    friend class QWSWindowSurface;
    friend class QGLDrawable;
};

QT_BEGIN_INCLUDE_NAMESPACE
#include <QtGui/qdecorationdefault_qws.h>
QT_END_INCLUDE_NAMESPACE

#endif // QT_NO_QWS_MANAGER

QT_END_NAMESPACE

QT_END_HEADER

#endif // QWSMANAGER_QWS_H
