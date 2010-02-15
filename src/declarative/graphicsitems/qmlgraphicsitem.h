/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLGRAPHICSITEM_H
#define QMLGRAPHICSITEM_H

#include <qml.h>
#include <qmlcomponent.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtGui/qgraphicsitem.h>
#include <QtGui/qgraphicstransform.h>
#include <QtGui/qfont.h>
#include <QtGui/qaction.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlState;
class QmlGraphicsAnchorLine;
class QmlTransition;
class QmlGraphicsKeyEvent;
class QmlGraphicsAnchors;
class QmlGraphicsItemPrivate;
class Q_DECLARATIVE_EXPORT QmlGraphicsItem : public QGraphicsObject, public QmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QmlParserStatus)

    Q_PROPERTY(QmlGraphicsItem * parent READ parentItem WRITE setParentItem NOTIFY parentChanged DESIGNABLE false FINAL)
    Q_PROPERTY(QmlList<QObject *> *data READ data DESIGNABLE false)
    Q_PROPERTY(QmlList<QmlGraphicsItem *>* children READ fxChildren DESIGNABLE false NOTIFY childrenChanged)
    Q_PROPERTY(QmlList<QObject *>* resources READ resources DESIGNABLE false)
    Q_PROPERTY(QmlList<QmlState *>* states READ states DESIGNABLE false)
    Q_PROPERTY(QmlList<QmlTransition *>* transitions READ transitions DESIGNABLE false)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged RESET resetWidth FINAL)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged RESET resetHeight FINAL)
    Q_PROPERTY(QRectF childrenRect READ childrenRect NOTIFY childrenRectChanged DESIGNABLE false FINAL)
    Q_PROPERTY(QmlGraphicsAnchors * anchors READ anchors DESIGNABLE false CONSTANT FINAL)
    Q_PROPERTY(QmlGraphicsAnchorLine left READ left CONSTANT FINAL)
    Q_PROPERTY(QmlGraphicsAnchorLine right READ right CONSTANT FINAL)
    Q_PROPERTY(QmlGraphicsAnchorLine horizontalCenter READ horizontalCenter CONSTANT FINAL)
    Q_PROPERTY(QmlGraphicsAnchorLine top READ top CONSTANT FINAL)
    Q_PROPERTY(QmlGraphicsAnchorLine bottom READ bottom CONSTANT FINAL)
    Q_PROPERTY(QmlGraphicsAnchorLine verticalCenter READ verticalCenter CONSTANT FINAL)
    Q_PROPERTY(QmlGraphicsAnchorLine baseline READ baseline CONSTANT FINAL)
    Q_PROPERTY(qreal baselineOffset READ baselineOffset WRITE setBaselineOffset NOTIFY baselineOffsetChanged)
    Q_PROPERTY(bool clip READ clip WRITE setClip) // ### move to QGI/QGO, NOTIFY
    Q_PROPERTY(bool focus READ hasFocus WRITE setFocus NOTIFY focusChanged FINAL)
    Q_PROPERTY(bool wantsFocus READ wantsFocus NOTIFY wantsFocusChanged)
    Q_PROPERTY(QmlList<QGraphicsTransform *>* transform READ transform DESIGNABLE false FINAL)
    Q_PROPERTY(TransformOrigin transformOrigin READ transformOrigin WRITE setTransformOrigin NOTIFY transformOriginChanged)
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth)
    Q_PROPERTY(QGraphicsEffect *effect READ graphicsEffect WRITE setGraphicsEffect)
    Q_ENUMS(TransformOrigin)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    enum TransformOrigin {
        TopLeft, Top, TopRight,
        Left, Center, Right,
        BottomLeft, Bottom, BottomRight
    };

    QmlGraphicsItem(QmlGraphicsItem *parent = 0);
    virtual ~QmlGraphicsItem();

    QmlGraphicsItem *parentItem() const;
    void setParentItem(QmlGraphicsItem *parent);
    void setParent(QmlGraphicsItem *parent) { setParentItem(parent); }

    QmlList<QObject *> *data();
    QmlList<QmlGraphicsItem *> *fxChildren();
    QmlList<QObject *> *resources();

    QmlGraphicsAnchors *anchors();
    QRectF childrenRect();

    bool clip() const;
    void setClip(bool);

    QmlList<QmlState *>* states();
    QmlList<QmlTransition *>* transitions();

    QString state() const;
    void setState(const QString &);

    qreal baselineOffset() const;
    void setBaselineOffset(qreal);

    QmlList<QGraphicsTransform *> *transform();

    qreal width() const;
    void setWidth(qreal);
    void resetWidth();
    qreal implicitWidth() const;

    qreal height() const;
    void setHeight(qreal);
    void resetHeight();
    qreal implicitHeight() const;

    TransformOrigin transformOrigin() const;
    void setTransformOrigin(TransformOrigin);

    bool smooth() const;
    void setSmooth(bool);

    QRectF boundingRect() const;
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    bool wantsFocus() const;
    bool hasFocus() const;
    void setFocus(bool);

    bool keepMouseGrab() const;
    void setKeepMouseGrab(bool);

    QmlGraphicsAnchorLine left() const;
    QmlGraphicsAnchorLine right() const;
    QmlGraphicsAnchorLine horizontalCenter() const;
    QmlGraphicsAnchorLine top() const;
    QmlGraphicsAnchorLine bottom() const;
    QmlGraphicsAnchorLine verticalCenter() const;
    QmlGraphicsAnchorLine baseline() const;

Q_SIGNALS:
    void widthChanged();
    void heightChanged();
    void childrenChanged();
    void childrenRectChanged();
    void baselineOffsetChanged();
    void stateChanged(const QString &);
    void focusChanged();
    void wantsFocusChanged();
    void parentChanged();
    void transformOriginChanged(TransformOrigin);

protected:
    bool isComponentComplete() const;
    virtual bool sceneEvent(QEvent *);
    virtual bool event(QEvent *);
    virtual QVariant itemChange(GraphicsItemChange, const QVariant &);

    void setImplicitWidth(qreal);
    bool widthValid() const; // ### better name?
    void setImplicitHeight(qreal);
    bool heightValid() const; // ### better name?

    virtual void classBegin();
    virtual void componentComplete();
    virtual void focusChanged(bool);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void inputMethodEvent(QInputMethodEvent *);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

protected:
    QmlGraphicsItem(QmlGraphicsItemPrivate &dd, QmlGraphicsItem *parent = 0);

private:
    friend class QmlStatePrivate;
    friend class QmlGraphicsAnchors;
    Q_DISABLE_COPY(QmlGraphicsItem)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsItem)
};

template<typename T>
        T qobject_cast(QGraphicsObject *o)
{
    QObject *obj = o;
    return qobject_cast<T>(obj);
}

// ### move to QGO
template<typename T>
T qobject_cast(QGraphicsItem *item)
{
    if (!item) return 0;
    QObject *o = item->toGraphicsObject();
    return qobject_cast<T>(o);
}

QDebug Q_DECLARATIVE_EXPORT operator<<(QDebug debug, QmlGraphicsItem *item);

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsItem)
QML_DECLARE_TYPE(QGraphicsTransform)
QML_DECLARE_TYPE(QGraphicsScale)
QML_DECLARE_TYPE(QGraphicsRotation)
QML_DECLARE_TYPE(QAction)

QT_END_HEADER

#endif // QMLGRAPHICSITEM_H
