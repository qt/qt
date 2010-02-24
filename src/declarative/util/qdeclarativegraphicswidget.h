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

#ifndef QDECLARATIVEGRAPHICSWIDGET_H
#define QDECLARATIVEGRAPHICSWIDGET_H

#include <QtCore/qdatetime.h>
#include <QtCore/qurl.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qgraphicswidget.h>
#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QGraphicsObject;
class QDeclarativeEngine;
class QDeclarativeContext;
class QDeclarativeError;

class QDeclarativeGraphicsWidgetPrivate;

class Q_DECLARATIVE_EXPORT QDeclarativeGraphicsWidget : public QGraphicsWidget
{
    Q_OBJECT
    Q_PROPERTY(ResizeMode resizeMode READ resizeMode WRITE setResizeMode)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource)

public:
    explicit QDeclarativeGraphicsWidget(QGraphicsItem *parent = 0);
    virtual ~QDeclarativeGraphicsWidget();

    QUrl source() const;
    void setSource(const QUrl&);

    QDeclarativeEngine* engine();
    QDeclarativeContext* rootContext();

    QGraphicsObject *rootObject() const;

    enum ResizeMode { SizeViewToRootObject, SizeRootObjectToView };
    ResizeMode resizeMode() const;
    void setResizeMode(ResizeMode);
    QSizeF sizeHint() const;

    enum Status { Null, Ready, Loading, Error };
    Status status() const;

    QList<QDeclarativeError> errors() const;

Q_SIGNALS:
    void sceneResized(QSizeF size);
    void statusChanged(QDeclarativeGraphicsWidget::Status);

private Q_SLOTS:
    void continueExecute();
    void sizeChanged();

protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent *);
    void timerEvent(QTimerEvent*);

private:
    QDeclarativeGraphicsWidgetPrivate* d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEGRAPHICSWIDGET_H
