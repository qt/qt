/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QSGCANVAS_H
#define QSGCANVAS_H

#include <QtCore/qmetatype.h>
#include <QtOpenGL/qgl.h>
#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGItem;
class QSGEngine;
class QSGCanvasPrivate;
class Q_DECLARATIVE_EXPORT QSGCanvas : public QGLWidget
{
Q_OBJECT
Q_DECLARE_PRIVATE(QSGCanvas)
public:
    QSGCanvas(QWidget *parent = 0, Qt::WindowFlags f = 0);
    QSGCanvas(const QGLFormat &format, QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~QSGCanvas();

    QSGItem *rootItem() const;
    QSGItem *activeFocusItem() const;

    QSGItem *mouseGrabberItem() const;

    bool sendEvent(QSGItem *, QEvent *);

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    QSGEngine *sceneGraphEngine() const;

signals:
    void sceneGraphInitialized();

protected:
    QSGCanvas(QSGCanvasPrivate &dd, QWidget *parent = 0, Qt::WindowFlags f = 0);
    QSGCanvas(QSGCanvasPrivate &dd, const QGLFormat &format, QWidget *parent = 0, Qt::WindowFlags f = 0);

    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);

    virtual bool event(QEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void inputMethodEvent(QInputMethodEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent(QWheelEvent *);
#endif

private slots:
    void sceneGraphChanged();
    void maybeUpdate();

private:
    Q_DISABLE_COPY(QSGCanvas);
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QSGCanvas *);

QT_END_HEADER

#endif // QSGCANVAS_H

