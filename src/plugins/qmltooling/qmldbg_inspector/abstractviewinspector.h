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

#ifndef ABSTRACTVIEWINSPECTOR_H
#define ABSTRACTVIEWINSPECTOR_H

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtGui/QColor>

#include "qdeclarativeinspectorprotocol.h"
#include "qmlinspectorconstants.h"

QT_BEGIN_NAMESPACE
class QDeclarativeEngine;
class QDeclarativeInspectorService;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
QT_END_NAMESPACE

namespace QmlJSDebugger {

class AbstractTool;

/*
 * The common code between QSGView and QDeclarativeView inspectors lives here,
 */
class AbstractViewInspector : public QObject
{
    Q_OBJECT

public:
    explicit AbstractViewInspector(QObject *parent = 0);

    virtual void changeCurrentObjects(const QList<QObject*> &objects) = 0;

    virtual void reloadView() = 0;

    void createQmlObject(const QString &qml, QObject *parent,
                         const QStringList &importList,
                         const QString &filename = QString());

    virtual void reparentQmlObject(QObject *object, QObject *newParent) = 0;

    virtual void changeTool(InspectorProtocol::Tool tool) = 0;

    void clearComponentCache();

    virtual QWidget *viewWidget() const = 0;
    virtual QDeclarativeEngine *declarativeEngine() const = 0;


    bool showAppOnTop() const { return m_showAppOnTop; }
    bool designModeBehavior() const { return m_designModeBehavior; }

    bool animationPaused() const { return m_animationPaused; }
    qreal slowDownFactor() const { return m_slowDownFactor; }

    void sendCurrentObjects(const QList<QObject*> &);
    void sendAnimationSpeed(qreal slowDownFactor);
    void sendAnimationPaused(bool paused);
    void sendCurrentTool(Constants::DesignTool toolId);
    void sendReloaded();
    void sendShowAppOnTop(bool showAppOnTop);

    QString idStringForObject(QObject *obj) const;

public slots:
    void sendDesignModeBehavior(bool inDesignMode);
    void sendColorChanged(const QColor &color);

    void changeToColorPickerTool();
    void changeToZoomTool();
    void changeToSingleSelectTool();
    void changeToMarqueeSelectTool();

    virtual void setDesignModeBehavior(bool value);

    void setShowAppOnTop(bool appOnTop);

    void setAnimationSpeed(qreal factor);
    void setAnimationPaused(bool paused);

signals:
    void designModeBehaviorChanged(bool inDesignMode);
    void showAppOnTopChanged(bool showAppOnTop);
    void reloadRequested();
    void marqueeSelectToolActivated();
    void selectToolActivated();
    void zoomToolActivated();
    void colorPickerActivated();
    void selectedColorChanged(const QColor &color);

    void animationSpeedChanged(qreal factor);
    void animationPausedChanged(bool paused);

protected:
    bool eventFilter(QObject *, QEvent *);

    virtual bool leaveEvent(QEvent *);
    virtual bool mousePressEvent(QMouseEvent *event);
    virtual bool mouseMoveEvent(QMouseEvent *event);
    virtual bool mouseReleaseEvent(QMouseEvent *event);
    virtual bool keyPressEvent(QKeyEvent *event);
    virtual bool keyReleaseEvent(QKeyEvent *keyEvent);
    virtual bool mouseDoubleClickEvent(QMouseEvent *event);
    virtual bool wheelEvent(QWheelEvent *event);

    AbstractTool *currentTool() const { return m_currentTool; }
    void setCurrentTool(AbstractTool *tool) { m_currentTool = tool; }

private slots:
    void handleMessage(const QByteArray &message);

private:
    void animationSpeedChangeRequested(qreal factor);
    void animationPausedChangeRequested(bool paused);

    AbstractTool *m_currentTool;

    bool m_showAppOnTop;
    bool m_designModeBehavior;

    bool m_animationPaused;
    qreal m_slowDownFactor;

    QHash<int, QString> m_stringIdForObjectId;
    QDeclarativeInspectorService *m_debugService;
};

} // namespace QmlJSDebugger

#endif // ABSTRACTVIEWINSPECTOR_H
