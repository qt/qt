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

#include "qmlstateoperations_p.h"

#include <qml.h>
#include <qmlcontext.h>
#include <qmlexpression.h>
#include <qmlinfo.h>
#include <qmlgraphicsanchors_p_p.h>
#include <qmlgraphicsitem_p.h>
#include <qmlguard_p.h>

#include <QtCore/qdebug.h>
#include <QtGui/qgraphicsitem.h>
#include <QtCore/qmath.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlParentChangePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlParentChange)
public:
    QmlParentChangePrivate() : target(0), parent(0), origParent(0), origStackBefore(0),
                               rewindParent(0), rewindStackBefore(0) {}

    QmlGraphicsItem *target;
    QmlGraphicsItem *parent;
    QmlGuard<QmlGraphicsItem> origParent;
    QmlGuard<QmlGraphicsItem> origStackBefore;
    QmlGraphicsItem *rewindParent;
    QmlGraphicsItem *rewindStackBefore;

    void doChange(QmlGraphicsItem *targetParent, QmlGraphicsItem *stackBefore = 0);
};

void QmlParentChangePrivate::doChange(QmlGraphicsItem *targetParent, QmlGraphicsItem *stackBefore)
{
    if (targetParent && target && target->parentItem()) {
        //### for backwards direction, can we just restore original x, y, scale, rotation
        Q_Q(QmlParentChange);
        bool ok;
        const QTransform &transform = target->itemTransform(targetParent, &ok);
        if (transform.type() >= QTransform::TxShear || !ok) {
            qmlInfo(q) << QmlParentChange::tr("Unable to preserve appearance under complex transform");
            ok = false;
        }

        qreal scale = 1;
        qreal rotation = 0;
        if (ok && transform.type() != QTransform::TxRotate) {
            if (transform.m11() == transform.m22())
                scale = transform.m11();
            else {
                qmlInfo(q) << QmlParentChange::tr("Unable to preserve appearance under non-uniform scale");
                ok = false;
            }
        } else if (ok && transform.type() == QTransform::TxRotate) {
            if (transform.m11() == transform.m22())
                scale = qSqrt(transform.m11()*transform.m11() + transform.m12()*transform.m12());
            else {
                qmlInfo(q) << QmlParentChange::tr("Unable to preserve appearance under non-uniform scale");
                ok = false;
            }

            if (scale != 0)
                rotation = atan2(transform.m12()/scale, transform.m11()/scale) * 180/M_PI;
            else {
                qmlInfo(q) << QmlParentChange::tr("Unable to preserve appearance under scale of 0");
                ok = false;
            }
        }

        qreal xt = transform.dx();
        qreal yt = transform.dy();
        if (ok && target->transformOrigin() != QmlGraphicsItem::TopLeft) {
            qreal tempxt = target->transformOriginPoint().x();
            qreal tempyt = target->transformOriginPoint().y();
            QTransform t;
            t.translate(-tempxt, -tempyt);
            t.rotate(rotation);
            t.scale(scale, scale);
            t.translate(tempxt, tempyt);
            QPointF offset = t.map(QPointF(0,0));
            xt += offset.x();
            yt += offset.y();
        }

        target->setParentItem(targetParent);
        if (ok) {
            //qDebug() << xt << yt << rotation << scale;
            target->setX(xt);
            target->setY(yt);
            target->setRotation(rotation);
            target->setScale(scale);
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
    \qmlclass ParentChange QmlParentChange
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

QML_DEFINE_TYPE(Qt,4,6,ParentChange,QmlParentChange)
QmlParentChange::QmlParentChange(QObject *parent)
    : QmlStateOperation(*(new QmlParentChangePrivate), parent)
{
}

QmlParentChange::~QmlParentChange()
{
}

/*!
    \qmlproperty Item ParentChange::target
    This property holds the item to be reparented
*/

QmlGraphicsItem *QmlParentChange::object() const
{
    Q_D(const QmlParentChange);
    return d->target;
}

void QmlParentChange::setObject(QmlGraphicsItem *target)
{
    Q_D(QmlParentChange);
    d->target = target;
}

/*!
    \qmlproperty Item ParentChange::parent
    This property holds the parent for the item in this state
*/

QmlGraphicsItem *QmlParentChange::parent() const
{
    Q_D(const QmlParentChange);
    return d->parent;
}

void QmlParentChange::setParent(QmlGraphicsItem *parent)
{
    Q_D(QmlParentChange);
    d->parent = parent;
}

QmlStateOperation::ActionList QmlParentChange::actions()
{
    Q_D(QmlParentChange);
    if (!d->target || !d->parent)
        return ActionList();

    QmlAction a;
    a.event = this;

    return ActionList() << a;
}

class AccessibleFxItem : public QmlGraphicsItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsItem)
public:
    int siblingIndex() {
        Q_D(QmlGraphicsItem);
        return d->siblingIndex;
    }
};

void QmlParentChange::saveOriginals()
{
    Q_D(QmlParentChange);
    saveCurrentValues();
    d->origParent = d->rewindParent;
    d->origStackBefore = d->rewindStackBefore;
}

void QmlParentChange::execute()
{
    Q_D(QmlParentChange);
    d->doChange(d->parent);
}

bool QmlParentChange::isReversable()
{
    return true;
}

void QmlParentChange::reverse()
{
    Q_D(QmlParentChange);
    d->doChange(d->origParent, d->origStackBefore);
}

QString QmlParentChange::typeName() const
{
    return QLatin1String("ParentChange");
}

bool QmlParentChange::override(QmlActionEvent*other)
{
    Q_D(QmlParentChange);
    if (other->typeName() != QLatin1String("ParentChange"))
        return false;
    if (QmlParentChange *otherPC = static_cast<QmlParentChange*>(other))
        return (d->target == otherPC->object());
    return false;
}

void QmlParentChange::saveCurrentValues()
{
    Q_D(QmlParentChange);
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
        QmlGraphicsItem *child = qobject_cast<QmlGraphicsItem*>(children.at(i));
        if (!child)
            continue;
        if (((AccessibleFxItem*)child)->siblingIndex() == siblingIndex) {
            d->rewindStackBefore = child;
            break;
        }
    }
}

void QmlParentChange::rewind()
{
    Q_D(QmlParentChange);
    d->doChange(d->rewindParent, d->rewindStackBefore);
}

class QmlStateChangeScriptPrivate : public QObjectPrivate
{
public:
    QmlStateChangeScriptPrivate() {}

    QmlScriptString script;
    QString name;
};

/*!
    \qmlclass StateChangeScript QmlStateChangeScript
    \brief The StateChangeScript element allows you to run a script in a state.

    The script specified will be run immediately when the state is made current.
    Alternatively you can use a ScriptAction to specify at which point in the transition
    you want the StateChangeScript to be run.
*/
QML_DEFINE_TYPE(Qt,4,6,StateChangeScript,QmlStateChangeScript)
QmlStateChangeScript::QmlStateChangeScript(QObject *parent)
: QmlStateOperation(*(new QmlStateChangeScriptPrivate), parent)
{
}

QmlStateChangeScript::~QmlStateChangeScript()
{
}

/*!
    \qmlproperty script StateChangeScript::script
    This property holds the script to run when the state is current.
*/
QmlScriptString QmlStateChangeScript::script() const
{
    Q_D(const QmlStateChangeScript);
    return d->script;
}

void QmlStateChangeScript::setScript(const QmlScriptString &s)
{
    Q_D(QmlStateChangeScript);
    d->script = s;
}

/*!
    \qmlproperty script StateChangeScript::script
    This property holds the name of the script. This name can be used by a
    ScriptAction to target a specific script.

    \sa ScriptAction::stateChangeScriptName
*/
QString QmlStateChangeScript::name() const
{
    Q_D(const QmlStateChangeScript);
    return d->name;
}

void QmlStateChangeScript::setName(const QString &n)
{
    Q_D(QmlStateChangeScript);
    d->name = n;
}

void QmlStateChangeScript::execute()
{
    Q_D(QmlStateChangeScript);
    const QString &script = d->script.script();
    if (!script.isEmpty()) {
        QmlExpression expr(d->script.context(), script, d->script.scopeObject());
        expr.setTrackChange(false);
        expr.value();
    }
}

QmlStateChangeScript::ActionList QmlStateChangeScript::actions()
{
    ActionList rv;
    QmlAction a;
    a.event = this;
    rv << a;
    return rv;
}

QString QmlStateChangeScript::typeName() const
{
    return QLatin1String("StateChangeScript");
}

/*!
    \qmlclass AnchorChanges QmlAnchorChanges
    \brief The AnchorChanges element allows you to change the anchors of an item in a state.

    In the following example we change the top and bottom anchors of an item:
    \snippet examples/declarative/anchors/anchor-changes.qml 0

    AnchorChanges will 'inject' \c x, \c y, \c width, and \c height changes into the transition,
    so you can animate them as you would normally changes to these properties:
    \qml
    //animate our anchor changes
    NumberAnimation { matchTargets: content; matchProperties: "x,y,width,height" }
    \endqml

    For more information on anchors see \l {anchor-layout}{Anchor Layouts}.
*/

QML_DEFINE_TYPE(Qt,4,6,AnchorChanges,QmlAnchorChanges)

class QmlAnchorChangesPrivate : public QObjectPrivate
{
public:
    QmlAnchorChangesPrivate() : target(0) {}

    QString name;
    QmlGraphicsItem *target;
    QString resetString;
    QStringList resetList;
    QmlGraphicsAnchorLine left;
    QmlGraphicsAnchorLine right;
    QmlGraphicsAnchorLine horizontalCenter;
    QmlGraphicsAnchorLine top;
    QmlGraphicsAnchorLine bottom;
    QmlGraphicsAnchorLine verticalCenter;
    QmlGraphicsAnchorLine baseline;

    QmlGraphicsAnchorLine origLeft;
    QmlGraphicsAnchorLine origRight;
    QmlGraphicsAnchorLine origHCenter;
    QmlGraphicsAnchorLine origTop;
    QmlGraphicsAnchorLine origBottom;
    QmlGraphicsAnchorLine origVCenter;
    QmlGraphicsAnchorLine origBaseline;

    QmlGraphicsAnchorLine rewindLeft;
    QmlGraphicsAnchorLine rewindRight;
    QmlGraphicsAnchorLine rewindHCenter;
    QmlGraphicsAnchorLine rewindTop;
    QmlGraphicsAnchorLine rewindBottom;
    QmlGraphicsAnchorLine rewindVCenter;
    QmlGraphicsAnchorLine rewindBaseline;

    qreal fromX;
    qreal fromY;
    qreal fromWidth;
    qreal fromHeight;
};

/*!
    \qmlproperty Item AnchorChanges::target
    This property holds the Item whose anchors will change
*/

QmlAnchorChanges::QmlAnchorChanges(QObject *parent)
 : QmlStateOperation(*(new QmlAnchorChangesPrivate), parent)
{
}

QmlAnchorChanges::~QmlAnchorChanges()
{
}

QmlAnchorChanges::ActionList QmlAnchorChanges::actions()
{
    QmlAction a;
    a.event = this;
    return ActionList() << a;
}

QmlGraphicsItem *QmlAnchorChanges::object() const
{
    Q_D(const QmlAnchorChanges);
    return d->target;
}

void QmlAnchorChanges::setObject(QmlGraphicsItem *target)
{
    Q_D(QmlAnchorChanges);
    d->target = target;
}

QString QmlAnchorChanges::reset() const
{
    Q_D(const QmlAnchorChanges);
    return d->resetString;
}

void QmlAnchorChanges::setReset(const QString &reset)
{
    Q_D(QmlAnchorChanges);
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

QmlGraphicsAnchorLine QmlAnchorChanges::left() const
{
    Q_D(const QmlAnchorChanges);
    return d->left;
}

void QmlAnchorChanges::setLeft(const QmlGraphicsAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->left = edge;
}

QmlGraphicsAnchorLine QmlAnchorChanges::right() const
{
    Q_D(const QmlAnchorChanges);
    return d->right;
}

void QmlAnchorChanges::setRight(const QmlGraphicsAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->right = edge;
}

QmlGraphicsAnchorLine QmlAnchorChanges::horizontalCenter() const
{
    Q_D(const QmlAnchorChanges);
    return d->horizontalCenter;
}

void QmlAnchorChanges::setHorizontalCenter(const QmlGraphicsAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->horizontalCenter = edge;
}

QmlGraphicsAnchorLine QmlAnchorChanges::top() const
{
    Q_D(const QmlAnchorChanges);
    return d->top;
}

void QmlAnchorChanges::setTop(const QmlGraphicsAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->top = edge;
}

QmlGraphicsAnchorLine QmlAnchorChanges::bottom() const
{
    Q_D(const QmlAnchorChanges);
    return d->bottom;
}

void QmlAnchorChanges::setBottom(const QmlGraphicsAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->bottom = edge;
}

QmlGraphicsAnchorLine QmlAnchorChanges::verticalCenter() const
{
    Q_D(const QmlAnchorChanges);
    return d->verticalCenter;
}

void QmlAnchorChanges::setVerticalCenter(const QmlGraphicsAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->verticalCenter = edge;
}

QmlGraphicsAnchorLine QmlAnchorChanges::baseline() const
{
    Q_D(const QmlAnchorChanges);
    return d->baseline;
}

void QmlAnchorChanges::setBaseline(const QmlGraphicsAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->baseline = edge;
}

void QmlAnchorChanges::execute()
{
    Q_D(QmlAnchorChanges);
    if (!d->target)
        return;

    //set any anchors that have been specified
    if (d->left.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->left);
    if (d->right.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setRight(d->right);
    if (d->horizontalCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setHorizontalCenter(d->horizontalCenter);
    if (d->top.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setTop(d->top);
    if (d->bottom.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->bottom);
    if (d->verticalCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setVerticalCenter(d->verticalCenter);
    if (d->baseline.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setBaseline(d->baseline);
}

bool QmlAnchorChanges::isReversable()
{
    return true;
}

void QmlAnchorChanges::reverse()
{
    Q_D(QmlAnchorChanges);
    if (!d->target)
        return;

    //restore previous anchors
    if (d->origLeft.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->origLeft);
    if (d->origRight.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setRight(d->origRight);
    if (d->origHCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setHorizontalCenter(d->origHCenter);
    if (d->origTop.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setTop(d->origTop);
    if (d->origBottom.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->origBottom);
    if (d->origVCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setVerticalCenter(d->origVCenter);
    if (d->origBaseline.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setBaseline(d->origBaseline);
}

QString QmlAnchorChanges::typeName() const
{
    return QLatin1String("AnchorChanges");
}

QList<QmlAction> QmlAnchorChanges::extraActions()
{
    Q_D(QmlAnchorChanges);
    QList<QmlAction> extra;

    //### try to be smarter about which ones we add.
    //    or short-circuit later on if they haven't actually changed.
    //    we shouldn't set explicit width if there wasn't one before.
    if (d->target) {
        QmlAction a;
        a.fromValue = d->fromX;
        a.property = QmlMetaProperty(d->target, QLatin1String("x"));
        extra << a;

        a.fromValue = d->fromY;
        a.property = QmlMetaProperty(d->target, QLatin1String("y"));
        extra << a;

        a.fromValue = d->fromWidth;
        a.property = QmlMetaProperty(d->target, QLatin1String("width"));
        extra << a;

        a.fromValue = d->fromHeight;
        a.property = QmlMetaProperty(d->target, QLatin1String("height"));
        extra << a;
    }

    return extra;
}

bool QmlAnchorChanges::changesBindings()
{
    return true;
}

void QmlAnchorChanges::saveOriginals()
{
    Q_D(QmlAnchorChanges);
    d->origLeft = d->target->anchors()->left();
    d->origRight = d->target->anchors()->right();
    d->origHCenter = d->target->anchors()->horizontalCenter();
    d->origTop = d->target->anchors()->top();
    d->origBottom = d->target->anchors()->bottom();
    d->origVCenter = d->target->anchors()->verticalCenter();
    d->origBaseline = d->target->anchors()->baseline();

    saveCurrentValues();
}

void QmlAnchorChanges::clearForwardBindings()
{
    Q_D(QmlAnchorChanges);
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
    if (d->left.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->right.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->horizontalCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->top.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->bottom.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->verticalCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->baseline.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();
}

void QmlAnchorChanges::clearReverseBindings()
{
    Q_D(QmlAnchorChanges);
    d->fromX = d->target->x();
    d->fromY = d->target->y();
    d->fromWidth = d->target->width();
    d->fromHeight = d->target->height();

    //reset any anchors that were set in the state
    if (d->left.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->right.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->horizontalCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->top.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->bottom.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->verticalCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->baseline.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();

    //reset any anchors that were set in the original state
    if (d->origLeft.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->origRight.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->origHCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->origTop.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->origBottom.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->origVCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->origBaseline.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();
}

bool QmlAnchorChanges::override(QmlActionEvent*other)
{
    if (other->typeName() != QLatin1String("AnchorChanges"))
        return false;
    if (static_cast<QmlActionEvent*>(this) == other)
        return true;
    if (static_cast<QmlAnchorChanges*>(other)->object() == object())
        return true;
    return false;
}

void QmlAnchorChanges::rewind()
{
    Q_D(QmlAnchorChanges);
    if (!d->target)
        return;

    //restore previous anchors
    if (d->rewindLeft.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->rewindLeft);
    if (d->rewindRight.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setRight(d->rewindRight);
    if (d->rewindHCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setHorizontalCenter(d->rewindHCenter);
    if (d->rewindTop.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setTop(d->rewindTop);
    if (d->rewindBottom.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->rewindBottom);
    if (d->rewindVCenter.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setVerticalCenter(d->rewindVCenter);
    if (d->rewindBaseline.anchorLine != QmlGraphicsAnchorLine::Invalid)
        d->target->anchors()->setBaseline(d->rewindBaseline);
}

void QmlAnchorChanges::saveCurrentValues()
{
    Q_D(QmlAnchorChanges);
    d->rewindLeft = d->target->anchors()->left();
    d->rewindRight = d->target->anchors()->right();
    d->rewindHCenter = d->target->anchors()->horizontalCenter();
    d->rewindTop = d->target->anchors()->top();
    d->rewindBottom = d->target->anchors()->bottom();
    d->rewindVCenter = d->target->anchors()->verticalCenter();
    d->rewindBaseline = d->target->anchors()->baseline();
}

#include <qmlstateoperations.moc>
#include <moc_qmlstateoperations_p.cpp>

QT_END_NAMESPACE

