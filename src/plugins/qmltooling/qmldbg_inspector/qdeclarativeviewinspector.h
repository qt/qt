/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QDECLARATIVEVIEWINSPECTOR_H
#define QDECLARATIVEVIEWINSPECTOR_H

#include <private/qdeclarativeglobal_p.h>

#include "qmlinspectorconstants.h"
#include "abstractviewinspector.h"

#include <QtCore/QScopedPointer>
#include <QtDeclarative/QDeclarativeView>

namespace QmlJSDebugger {

class AbstractLiveEditTool;
class QDeclarativeViewInspectorPrivate;

class QDeclarativeViewInspector : public AbstractViewInspector
{
    Q_OBJECT

public:
    explicit QDeclarativeViewInspector(QDeclarativeView *view, QObject *parent = 0);
    ~QDeclarativeViewInspector();

    // AbstractViewInspector
    void changeCurrentObjects(const QList<QObject*> &objects);
    void reloadView();
    void reparentQmlObject(QObject *object, QObject *newParent);
    void changeTool(InspectorProtocol::Tool tool);
    QWidget *viewWidget() const { return declarativeView(); }
    QDeclarativeEngine *declarativeEngine() const;

    void setSelectedItems(QList<QGraphicsItem *> items);
    QList<QGraphicsItem *> selectedItems() const;

    QDeclarativeView *declarativeView() const;

    QRectF adjustToScreenBoundaries(const QRectF &boundingRectInSceneSpace);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

    bool leaveEvent(QEvent *);
    bool mouseMoveEvent(QMouseEvent *event);

    AbstractLiveEditTool *currentTool() const;

private:
    Q_DISABLE_COPY(QDeclarativeViewInspector)

    inline QDeclarativeViewInspectorPrivate *d_func() { return data.data(); }
    QScopedPointer<QDeclarativeViewInspectorPrivate> data;
    friend class QDeclarativeViewInspectorPrivate;
    friend class AbstractLiveEditTool;
};

} // namespace QmlJSDebugger

#endif // QDECLARATIVEVIEWINSPECTOR_H
