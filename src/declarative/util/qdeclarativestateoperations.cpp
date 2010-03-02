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

#include "qdeclarativestateoperations_p.h"

#include <qdeclarative.h>
#include <qdeclarativecontext.h>
#include <qdeclarativeexpression.h>
#include <qdeclarativeinfo.h>
#include <qdeclarativeanchors_p_p.h>
#include <qdeclarativeitem_p.h>
#include <qdeclarativeguard_p.h>
#include <qdeclarativenullablevalue_p_p.h>

#include <QtCore/qdebug.h>
#include <QtGui/qgraphicsitem.h>
#include <QtCore/qmath.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeParentChangePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeParentChange)
public:
    QDeclarativeParentChangePrivate() : target(0), parent(0), origParent(0), origStackBefore(0),
        rewindParent(0), rewindStackBefore(0) {}

    QDeclarativeItem *target;
    QDeclarativeItem *parent;
    QDeclarativeGuard<QDeclarativeItem> origParent;
    QDeclarativeGuard<QDeclarativeItem> origStackBefore;
    QDeclarativeItem *rewindParent;
    QDeclarativeItem *rewindStackBefore;

    QDeclarativeNullableValue<qreal> x;
    QDeclarativeNullableValue<qreal> y;
    QDeclarativeNullableValue<qreal> width;
    QDeclarativeNullableValue<qreal> height;
    QDeclarativeNullableValue<qreal> scale;
    QDeclarativeNullableValue<qreal> rotation;

    void doChange(QDeclarativeItem *targetParent, QDeclarativeItem *stackBefore = 0);
};

void QDeclarativeParentChangePrivate::doChange(QDeclarativeItem *targetParent, QDeclarativeItem *stackBefore)
{
    if (targetParent && target && target->parentItem()) {
        //### for backwards direction, can we just restore original x, y, scale, rotation
        Q_Q(QDeclarativeParentChange);
        bool ok;
        const QTransform &transform = target->parentItem()->itemTransform(targetParent, &ok);
        if (transform.type() >= QTransform::TxShear || !ok) {
            qmlInfo(q) << QDeclarativeParentChange::tr("Unable to preserve appearance under complex transform");
            ok = false;
        }

        qreal scale = 1;
        qreal rotation = 0;
        if (ok && transform.type() != QTransform::TxRotate) {
            if (transform.m11() == transform.m22())
                scale = transform.m11();
            else {
                qmlInfo(q) << QDeclarativeParentChange::tr("Unable to preserve appearance under non-uniform scale");
                ok = false;
            }
        } else if (ok && transform.type() == QTransform::TxRotate) {
            if (transform.m11() == transform.m22())
                scale = qSqrt(transform.m11()*transform.m11() + transform.m12()*transform.m12());
            else {
                qmlInfo(q) << QDeclarativeParentChange::tr("Unable to preserve appearance under non-uniform scale");
                ok = false;
            }

            if (scale != 0)
                rotation = atan2(transform.m12()/scale, transform.m11()/scale) * 180/M_PI;
            else {
                qmlInfo(q) << QDeclarativeParentChange::tr("Unable to preserve appearance under scale of 0");
                ok = false;
            }
        }

        const QPointF &point = transform.map(QPointF(target->x(),target->y()));
        qreal x = point.x();
        qreal y = point.y();
        if (ok && target->transformOrigin() != QDeclarativeItem::TopLeft) {
            qreal tempxt = target->transformOriginPoint().x();
            qreal tempyt = target->transformOriginPoint().y();
            QTransform t;
            t.translate(-tempxt, -tempyt);
            t.rotate(rotation);
            t.scale(scale, scale);
            t.translate(tempxt, tempyt);
            const QPointF &offset = t.map(QPointF(0,0));
            x += offset.x();
            y += offset.y();
        }

        target->setParentItem(targetParent);
        if (ok) {
            //qDebug() << x << y << rotation << scale;
            target->setX(x);
            target->setY(y);
            target->setRotation(target->rotation() + rotation);
            target->setScale(target->scale() * scale);
        }
    } else if (target) {
        target->setParentItem(targetParent);
    }

    //restore the original stack position.
    //### if stackBefore has also been reparented this won't work
    if (stackBefore)
        target->stackBefore(stackBefore);
}

/*!
    \preliminary
    \qmlclass ParentChange QDeclarativeParentChange
    \brief The ParentChange element allows you to reparent an Item in a state change.

    ParentChange reparents an Item while preserving its visual appearance (position, rotation,
    and scale) on screen. You can then specify a transition to move/rotate/scale the Item to
    its final intended appearance.

    ParentChange can only preserve visual appearance if no complex transforms are involved.
    More specifically, it will not work if the transform property has been set for any
    Items involved in the reparenting (defined as any Items in the common ancestor tree
    for the original and new parent).

    You can specify at which point in a transition you want a ParentChange to occur by
    using a ParentAction.
*/


QDeclarativeParentChange::QDeclarativeParentChange(QObject *parent)
    : QDeclarativeStateOperation(*(new QDeclarativeParentChangePrivate), parent)
{
}

QDeclarativeParentChange::~QDeclarativeParentChange()
{
}

qreal QDeclarativeParentChange::x() const
{
    Q_D(const QDeclarativeParentChange);
    return d->x.isNull ? qreal(0.) : d->x.value;
}

void QDeclarativeParentChange::setX(qreal x)
{
    Q_D(QDeclarativeParentChange);
    d->x = x;
}

bool QDeclarativeParentChange::xIsSet() const
{
    Q_D(const QDeclarativeParentChange);
    return d->x.isValid();
}

qreal QDeclarativeParentChange::y() const
{
    Q_D(const QDeclarativeParentChange);
    return d->y.isNull ? qreal(0.) : d->y.value;
}

void QDeclarativeParentChange::setY(qreal y)
{
    Q_D(QDeclarativeParentChange);
    d->y = y;
}

bool QDeclarativeParentChange::yIsSet() const
{
    Q_D(const QDeclarativeParentChange);
    return d->y.isValid();
}

qreal QDeclarativeParentChange::width() const
{
    Q_D(const QDeclarativeParentChange);
    return d->width.isNull ? qreal(0.) : d->width.value;
}

void QDeclarativeParentChange::setWidth(qreal width)
{
    Q_D(QDeclarativeParentChange);
    d->width = width;
}

bool QDeclarativeParentChange::widthIsSet() const
{
    Q_D(const QDeclarativeParentChange);
    return d->width.isValid();
}

qreal QDeclarativeParentChange::height() const
{
    Q_D(const QDeclarativeParentChange);
    return d->height.isNull ? qreal(0.) : d->height.value;
}

void QDeclarativeParentChange::setHeight(qreal height)
{
    Q_D(QDeclarativeParentChange);
    d->height = height;
}

bool QDeclarativeParentChange::heightIsSet() const
{
    Q_D(const QDeclarativeParentChange);
    return d->height.isValid();
}

qreal QDeclarativeParentChange::scale() const
{
    Q_D(const QDeclarativeParentChange);
    return d->scale.isNull ? qreal(1.) : d->scale.value;
}

void QDeclarativeParentChange::setScale(qreal scale)
{
    Q_D(QDeclarativeParentChange);
    d->scale = scale;
}

bool QDeclarativeParentChange::scaleIsSet() const
{
    Q_D(const QDeclarativeParentChange);
    return d->scale.isValid();
}

qreal QDeclarativeParentChange::rotation() const
{
    Q_D(const QDeclarativeParentChange);
    return d->rotation.isNull ? qreal(0.) : d->rotation.value;
}

void QDeclarativeParentChange::setRotation(qreal rotation)
{
    Q_D(QDeclarativeParentChange);
    d->rotation = rotation;
}

bool QDeclarativeParentChange::rotationIsSet() const
{
    Q_D(const QDeclarativeParentChange);
    return d->rotation.isValid();
}

QDeclarativeItem *QDeclarativeParentChange::originalParent() const
{
    Q_D(const QDeclarativeParentChange);
    return d->origParent;
}

/*!
    \qmlproperty Item ParentChange::target
    This property holds the item to be reparented
*/

QDeclarativeItem *QDeclarativeParentChange::object() const
{
    Q_D(const QDeclarativeParentChange);
    return d->target;
}

void QDeclarativeParentChange::setObject(QDeclarativeItem *target)
{
    Q_D(QDeclarativeParentChange);
    d->target = target;
}

/*!
    \qmlproperty Item ParentChange::parent
    This property holds the parent for the item in this state
*/

QDeclarativeItem *QDeclarativeParentChange::parent() const
{
    Q_D(const QDeclarativeParentChange);
    return d->parent;
}

void QDeclarativeParentChange::setParent(QDeclarativeItem *parent)
{
    Q_D(QDeclarativeParentChange);
    d->parent = parent;
}

QDeclarativeStateOperation::ActionList QDeclarativeParentChange::actions()
{
    Q_D(QDeclarativeParentChange);
    if (!d->target || !d->parent)
        return ActionList();

    ActionList actions;

    QDeclarativeAction a;
    a.event = this;
    actions << a;

    if (d->x.isValid()) {
        QDeclarativeAction xa(d->target, QLatin1String("x"), x());
        actions << xa;
    }

    if (d->y.isValid()) {
        QDeclarativeAction ya(d->target, QLatin1String("y"), y());
        actions << ya;
    }

    if (d->scale.isValid()) {
        QDeclarativeAction sa(d->target, QLatin1String("scale"), scale());
        actions << sa;
    }

    if (d->rotation.isValid()) {
        QDeclarativeAction ra(d->target, QLatin1String("rotation"), rotation());
        actions << ra;
    }

    if (d->width.isValid()) {
        QDeclarativeAction wa(d->target, QLatin1String("width"), width());
        actions << wa;
    }

    if (d->height.isValid()) {
        QDeclarativeAction ha(d->target, QLatin1String("height"), height());
        actions << ha;
    }

    return actions;
}

class AccessibleFxItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeItem)
public:
    int siblingIndex() {
        Q_D(QDeclarativeItem);
        return d->siblingIndex;
    }
};

void QDeclarativeParentChange::saveOriginals()
{
    Q_D(QDeclarativeParentChange);
    saveCurrentValues();
    d->origParent = d->rewindParent;
    d->origStackBefore = d->rewindStackBefore;
}

void QDeclarativeParentChange::execute()
{
    Q_D(QDeclarativeParentChange);
    d->doChange(d->parent);
}

bool QDeclarativeParentChange::isReversable()
{
    return true;
}

void QDeclarativeParentChange::reverse()
{
    Q_D(QDeclarativeParentChange);
    d->doChange(d->origParent, d->origStackBefore);
}

QString QDeclarativeParentChange::typeName() const
{
    return QLatin1String("ParentChange");
}

bool QDeclarativeParentChange::override(QDeclarativeActionEvent*other)
{
    Q_D(QDeclarativeParentChange);
    if (other->typeName() != QLatin1String("ParentChange"))
        return false;
    if (QDeclarativeParentChange *otherPC = static_cast<QDeclarativeParentChange*>(other))
        return (d->target == otherPC->object());
    return false;
}

void QDeclarativeParentChange::saveCurrentValues()
{
    Q_D(QDeclarativeParentChange);
    if (!d->target) {
        d->rewindParent = 0;
        d->rewindStackBefore = 0;
        return;
    }

    d->rewindParent = d->target->parentItem();

    if (!d->rewindParent) {
        d->rewindStackBefore = 0;
        return;
    }

    //try to determine the item's original stack position so we can restore it
    int siblingIndex = ((AccessibleFxItem*)d->target)->siblingIndex() + 1;
    QList<QGraphicsItem*> children = d->rewindParent->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QDeclarativeItem *child = qobject_cast<QDeclarativeItem*>(children.at(i));
        if (!child)
            continue;
        if (((AccessibleFxItem*)child)->siblingIndex() == siblingIndex) {
            d->rewindStackBefore = child;
            break;
        }
    }
}

void QDeclarativeParentChange::rewind()
{
    Q_D(QDeclarativeParentChange);
    d->doChange(d->rewindParent, d->rewindStackBefore);
}

class QDeclarativeStateChangeScriptPrivate : public QObjectPrivate
{
public:
    QDeclarativeStateChangeScriptPrivate() {}

    QDeclarativeScriptString script;
    QString name;
};

/*!
    \qmlclass StateChangeScript QDeclarativeStateChangeScript
    \brief The StateChangeScript element allows you to run a script in a state.

    The script specified will be run immediately when the state is made current.
    Alternatively you can use a ScriptAction to specify at which point in the transition
    you want the StateChangeScript to be run.
*/

QDeclarativeStateChangeScript::QDeclarativeStateChangeScript(QObject *parent)
: QDeclarativeStateOperation(*(new QDeclarativeStateChangeScriptPrivate), parent)
{
}

QDeclarativeStateChangeScript::~QDeclarativeStateChangeScript()
{
}

/*!
    \qmlproperty script StateChangeScript::script
    This property holds the script to run when the state is current.
*/
QDeclarativeScriptString QDeclarativeStateChangeScript::script() const
{
    Q_D(const QDeclarativeStateChangeScript);
    return d->script;
}

void QDeclarativeStateChangeScript::setScript(const QDeclarativeScriptString &s)
{
    Q_D(QDeclarativeStateChangeScript);
    d->script = s;
}

/*!
    \qmlproperty script StateChangeScript::script
    This property holds the name of the script. This name can be used by a
    ScriptAction to target a specific script.

    \sa ScriptAction::stateChangeScriptName
*/
QString QDeclarativeStateChangeScript::name() const
{
    Q_D(const QDeclarativeStateChangeScript);
    return d->name;
}

void QDeclarativeStateChangeScript::setName(const QString &n)
{
    Q_D(QDeclarativeStateChangeScript);
    d->name = n;
}

void QDeclarativeStateChangeScript::execute()
{
    Q_D(QDeclarativeStateChangeScript);
    const QString &script = d->script.script();
    if (!script.isEmpty()) {
        QDeclarativeExpression expr(d->script.context(), script, d->script.scopeObject());
        expr.value();
    }
}

QDeclarativeStateChangeScript::ActionList QDeclarativeStateChangeScript::actions()
{
    ActionList rv;
    QDeclarativeAction a;
    a.event = this;
    rv << a;
    return rv;
}

QString QDeclarativeStateChangeScript::typeName() const
{
    return QLatin1String("StateChangeScript");
}

/*!
    \qmlclass AnchorChanges QDeclarativeAnchorChanges
    \brief The AnchorChanges element allows you to change the anchors of an item in a state.

    In the following example we change the top and bottom anchors of an item:
    \snippet examples/declarative/anchors/anchor-changes.qml 0

    AnchorChanges will 'inject' \c x, \c y, \c width, and \c height changes into the transition,
    so you can animate them as you would normally changes to these properties:
    \qml
    //animate our anchor changes
    NumberAnimation { targets: content; properties: "x,y,width,height" }
    \endqml

    For more information on anchors see \l {anchor-layout}{Anchor Layouts}.
*/



class QDeclarativeAnchorChangesPrivate : public QObjectPrivate
{
public:
    QDeclarativeAnchorChangesPrivate() : target(0) {}

    QString name;
    QDeclarativeItem *target;
    QString resetString;
    QStringList resetList;
    QDeclarativeAnchorLine left;
    QDeclarativeAnchorLine right;
    QDeclarativeAnchorLine horizontalCenter;
    QDeclarativeAnchorLine top;
    QDeclarativeAnchorLine bottom;
    QDeclarativeAnchorLine verticalCenter;
    QDeclarativeAnchorLine baseline;

    QDeclarativeAnchorLine origLeft;
    QDeclarativeAnchorLine origRight;
    QDeclarativeAnchorLine origHCenter;
    QDeclarativeAnchorLine origTop;
    QDeclarativeAnchorLine origBottom;
    QDeclarativeAnchorLine origVCenter;
    QDeclarativeAnchorLine origBaseline;

    QDeclarativeAnchorLine rewindLeft;
    QDeclarativeAnchorLine rewindRight;
    QDeclarativeAnchorLine rewindHCenter;
    QDeclarativeAnchorLine rewindTop;
    QDeclarativeAnchorLine rewindBottom;
    QDeclarativeAnchorLine rewindVCenter;
    QDeclarativeAnchorLine rewindBaseline;

    qreal fromX;
    qreal fromY;
    qreal fromWidth;
    qreal fromHeight;
};

/*!
    \qmlproperty Item AnchorChanges::target
    This property holds the Item whose anchors will change
*/

QDeclarativeAnchorChanges::QDeclarativeAnchorChanges(QObject *parent)
 : QDeclarativeStateOperation(*(new QDeclarativeAnchorChangesPrivate), parent)
{
}

QDeclarativeAnchorChanges::~QDeclarativeAnchorChanges()
{
}

QDeclarativeAnchorChanges::ActionList QDeclarativeAnchorChanges::actions()
{
    QDeclarativeAction a;
    a.event = this;
    return ActionList() << a;
}

QDeclarativeItem *QDeclarativeAnchorChanges::object() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->target;
}

void QDeclarativeAnchorChanges::setObject(QDeclarativeItem *target)
{
    Q_D(QDeclarativeAnchorChanges);
    d->target = target;
}

QString QDeclarativeAnchorChanges::reset() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->resetString;
}

void QDeclarativeAnchorChanges::setReset(const QString &reset)
{
    Q_D(QDeclarativeAnchorChanges);
    d->resetString = reset;
    d->resetList = d->resetString.split(QLatin1Char(','));
    for (int i = 0; i < d->resetList.count(); ++i)
        d->resetList[i] = d->resetList.at(i).trimmed();
}

/*!
    \qmlproperty AnchorLine AnchorChanges::left
    \qmlproperty AnchorLine AnchorChanges::right
    \qmlproperty AnchorLine AnchorChanges::horizontalCenter
    \qmlproperty AnchorLine AnchorChanges::top
    \qmlproperty AnchorLine AnchorChanges::bottom
    \qmlproperty AnchorLine AnchorChanges::verticalCenter
    \qmlproperty AnchorLine AnchorChanges::baseline

    These properties change the respective anchors of the item.
*/

QDeclarativeAnchorLine QDeclarativeAnchorChanges::left() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->left;
}

void QDeclarativeAnchorChanges::setLeft(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchorChanges);
    d->left = edge;
}

QDeclarativeAnchorLine QDeclarativeAnchorChanges::right() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->right;
}

void QDeclarativeAnchorChanges::setRight(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchorChanges);
    d->right = edge;
}

QDeclarativeAnchorLine QDeclarativeAnchorChanges::horizontalCenter() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->horizontalCenter;
}

void QDeclarativeAnchorChanges::setHorizontalCenter(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchorChanges);
    d->horizontalCenter = edge;
}

QDeclarativeAnchorLine QDeclarativeAnchorChanges::top() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->top;
}

void QDeclarativeAnchorChanges::setTop(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchorChanges);
    d->top = edge;
}

QDeclarativeAnchorLine QDeclarativeAnchorChanges::bottom() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->bottom;
}

void QDeclarativeAnchorChanges::setBottom(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchorChanges);
    d->bottom = edge;
}

QDeclarativeAnchorLine QDeclarativeAnchorChanges::verticalCenter() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->verticalCenter;
}

void QDeclarativeAnchorChanges::setVerticalCenter(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchorChanges);
    d->verticalCenter = edge;
}

QDeclarativeAnchorLine QDeclarativeAnchorChanges::baseline() const
{
    Q_D(const QDeclarativeAnchorChanges);
    return d->baseline;
}

void QDeclarativeAnchorChanges::setBaseline(const QDeclarativeAnchorLine &edge)
{
    Q_D(QDeclarativeAnchorChanges);
    d->baseline = edge;
}

void QDeclarativeAnchorChanges::execute()
{
    Q_D(QDeclarativeAnchorChanges);
    if (!d->target)
        return;

    //set any anchors that have been specified
    if (d->left.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->left);
    if (d->right.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setRight(d->right);
    if (d->horizontalCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setHorizontalCenter(d->horizontalCenter);
    if (d->top.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setTop(d->top);
    if (d->bottom.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->bottom);
    if (d->verticalCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setVerticalCenter(d->verticalCenter);
    if (d->baseline.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setBaseline(d->baseline);
}

bool QDeclarativeAnchorChanges::isReversable()
{
    return true;
}

void QDeclarativeAnchorChanges::reverse()
{
    Q_D(QDeclarativeAnchorChanges);
    if (!d->target)
        return;

    //restore previous anchors
    if (d->origLeft.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->origLeft);
    if (d->origRight.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setRight(d->origRight);
    if (d->origHCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setHorizontalCenter(d->origHCenter);
    if (d->origTop.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setTop(d->origTop);
    if (d->origBottom.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->origBottom);
    if (d->origVCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setVerticalCenter(d->origVCenter);
    if (d->origBaseline.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setBaseline(d->origBaseline);
}

QString QDeclarativeAnchorChanges::typeName() const
{
    return QLatin1String("AnchorChanges");
}

QList<QDeclarativeAction> QDeclarativeAnchorChanges::extraActions()
{
    Q_D(QDeclarativeAnchorChanges);
    QList<QDeclarativeAction> extra;

    //### try to be smarter about which ones we add.
    //    or short-circuit later on if they haven't actually changed.
    //    we shouldn't set explicit width if there wasn't one before.
    if (d->target) {
        QDeclarativeAction a;
        a.fromValue = d->fromX;
        a.property = QDeclarativeProperty(d->target, QLatin1String("x"));
        extra << a;

        a.fromValue = d->fromY;
        a.property = QDeclarativeProperty(d->target, QLatin1String("y"));
        extra << a;

        a.fromValue = d->fromWidth;
        a.property = QDeclarativeProperty(d->target, QLatin1String("width"));
        extra << a;

        a.fromValue = d->fromHeight;
        a.property = QDeclarativeProperty(d->target, QLatin1String("height"));
        extra << a;
    }

    return extra;
}

bool QDeclarativeAnchorChanges::changesBindings()
{
    return true;
}

void QDeclarativeAnchorChanges::saveOriginals()
{
    Q_D(QDeclarativeAnchorChanges);
    d->origLeft = d->target->anchors()->left();
    d->origRight = d->target->anchors()->right();
    d->origHCenter = d->target->anchors()->horizontalCenter();
    d->origTop = d->target->anchors()->top();
    d->origBottom = d->target->anchors()->bottom();
    d->origVCenter = d->target->anchors()->verticalCenter();
    d->origBaseline = d->target->anchors()->baseline();

    saveCurrentValues();
}

void QDeclarativeAnchorChanges::clearForwardBindings()
{
    Q_D(QDeclarativeAnchorChanges);
    d->fromX = d->target->x();
    d->fromY = d->target->y();
    d->fromWidth = d->target->width();
    d->fromHeight = d->target->height();

    //reset any anchors that have been specified
    if (d->resetList.contains(QLatin1String("left")))
        d->target->anchors()->resetLeft();
    if (d->resetList.contains(QLatin1String("right")))
        d->target->anchors()->resetRight();
    if (d->resetList.contains(QLatin1String("horizontalCenter")))
        d->target->anchors()->resetHorizontalCenter();
    if (d->resetList.contains(QLatin1String("top")))
        d->target->anchors()->resetTop();
    if (d->resetList.contains(QLatin1String("bottom")))
        d->target->anchors()->resetBottom();
    if (d->resetList.contains(QLatin1String("verticalCenter")))
        d->target->anchors()->resetVerticalCenter();
    if (d->resetList.contains(QLatin1String("baseline")))
        d->target->anchors()->resetBaseline();

    //reset any anchors that we'll be setting in the state
    if (d->left.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->right.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->horizontalCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->top.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->bottom.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->verticalCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->baseline.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();
}

void QDeclarativeAnchorChanges::clearReverseBindings()
{
    Q_D(QDeclarativeAnchorChanges);
    d->fromX = d->target->x();
    d->fromY = d->target->y();
    d->fromWidth = d->target->width();
    d->fromHeight = d->target->height();

    //reset any anchors that were set in the state
    if (d->left.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->right.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->horizontalCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->top.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->bottom.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->verticalCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->baseline.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();

    //reset any anchors that were set in the original state
    if (d->origLeft.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->origRight.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->origHCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->origTop.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->origBottom.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->origVCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->origBaseline.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();
}

bool QDeclarativeAnchorChanges::override(QDeclarativeActionEvent*other)
{
    if (other->typeName() != QLatin1String("AnchorChanges"))
        return false;
    if (static_cast<QDeclarativeActionEvent*>(this) == other)
        return true;
    if (static_cast<QDeclarativeAnchorChanges*>(other)->object() == object())
        return true;
    return false;
}

void QDeclarativeAnchorChanges::rewind()
{
    Q_D(QDeclarativeAnchorChanges);
    if (!d->target)
        return;

    //restore previous anchors
    if (d->rewindLeft.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->rewindLeft);
    if (d->rewindRight.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setRight(d->rewindRight);
    if (d->rewindHCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setHorizontalCenter(d->rewindHCenter);
    if (d->rewindTop.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setTop(d->rewindTop);
    if (d->rewindBottom.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->rewindBottom);
    if (d->rewindVCenter.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setVerticalCenter(d->rewindVCenter);
    if (d->rewindBaseline.anchorLine != QDeclarativeAnchorLine::Invalid)
        d->target->anchors()->setBaseline(d->rewindBaseline);
}

void QDeclarativeAnchorChanges::saveCurrentValues()
{
    Q_D(QDeclarativeAnchorChanges);
    d->rewindLeft = d->target->anchors()->left();
    d->rewindRight = d->target->anchors()->right();
    d->rewindHCenter = d->target->anchors()->horizontalCenter();
    d->rewindTop = d->target->anchors()->top();
    d->rewindBottom = d->target->anchors()->bottom();
    d->rewindVCenter = d->target->anchors()->verticalCenter();
    d->rewindBaseline = d->target->anchors()->baseline();
}

#include <qdeclarativestateoperations.moc>
#include <moc_qdeclarativestateoperations_p.cpp>

QT_END_NAMESPACE

