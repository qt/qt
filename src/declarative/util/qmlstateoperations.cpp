/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/qobject_p.h>
#include <qml.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlexpression.h>
#include "qmlstateoperations.h"
#include <QtCore/qdebug.h>
#include <QtDeclarative/qmlinfo.h>
#include <private/qfxanchors_p.h>
#include <private/qfxitem_p.h>
#include <QtGui/qgraphicsitem.h>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

class QmlParentChangePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlParentChange)
public:
    QmlParentChangePrivate() : target(0), parent(0), origParent(0), origStackBefore(0) {}

    QFxItem *target;
    QFxItem *parent;
    QGuard<QFxItem> origParent;
    QGuard<QFxItem> origStackBefore;

    void doChange(QFxItem *targetParent, QFxItem *stackBefore = 0);
};

void QmlParentChangePrivate::doChange(QFxItem *targetParent, QFxItem *stackBefore)
{
    if (targetParent && target && target->parentItem()) {
        //### for backwards direction, we can just restore original x, y, scale, rotation
        Q_Q(QmlParentChange);
        const QTransform &transform = target->itemTransform(targetParent);
        if (transform.type() >= QTransform::TxShear) {
            qmlInfo(QObject::tr("Unable to preserve appearance under complex transform"), q);
        }

        qreal scale = 1;
        qreal rotation = 0;
        if (transform.type() != QTransform::TxRotate) {
            if (transform.m11() == transform.m22())
                scale = transform.m11();
            else
                qmlInfo(QObject::tr("Unable to preserve appearance under non-uniform scale"), q);
        } else if (transform.type() == QTransform::TxRotate) {
            if (transform.m11() == transform.m22())
                scale = qSqrt(transform.m11()*transform.m11() + transform.m12()*transform.m12());
            else
                qmlInfo(QObject::tr("Unable to preserve appearance under non-uniform scale"), q);

            if (scale != 0)
                rotation = atan2(transform.m12()/scale, transform.m11()/scale) * 180/M_PI;
            else
                qmlInfo(QObject::tr("Unable to preserve appearance under scale of 0"), q);
        }
        target->setParentItem(targetParent);
        //qDebug() << transform.dx() << transform.dy() << rotation << scale;
        target->setX(transform.dx());
        target->setY(transform.dy());
        target->setRotation(rotation);
        target->setScale(scale);
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
    \qmlclass ParentChange
    \brief The ParentChange element allows you to reparent an object in a state.
*/

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,ParentChange,QmlParentChange)
QmlParentChange::QmlParentChange(QObject *parent)
    : QmlStateOperation(*(new QmlParentChangePrivate), parent)
{
}

QmlParentChange::~QmlParentChange()
{
}

/*!
    \qmlproperty Object ParentChange::target
    This property holds the item to be reparented
*/

QFxItem *QmlParentChange::object() const
{
    Q_D(const QmlParentChange);
    return d->target;
}

void QmlParentChange::setObject(QFxItem *target)
{
    Q_D(QmlParentChange);
    d->target = target;
}

/*!
    \qmlproperty Item ParentChange::parent
    This property holds the parent for the item in this state
*/

QFxItem *QmlParentChange::parent() const
{
    Q_D(const QmlParentChange);
    return d->parent;
}

void QmlParentChange::setParent(QFxItem *parent)
{
    Q_D(QmlParentChange);
    d->parent = parent;
}

QmlStateOperation::ActionList QmlParentChange::actions()
{
    Q_D(QmlParentChange);
    if (!d->target || !d->parent)
        return ActionList();

    Action a;
    a.event = this;

    return ActionList() << a;
}

class AccessibleFxItem : public QFxItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QFxItem)
public:
    int siblingIndex() {
        Q_D(QFxItem);
        return d->siblingIndex;
    }
};

void QmlParentChange::saveOriginals()
{
    Q_D(QmlParentChange);
    if (!d->target) {
        d->origParent = 0;
        d->origStackBefore = 0;
        return;
    }

    d->origParent = d->target->parentItem();

    if (!d->origParent) {
        d->origStackBefore = 0;
        return;
    }

    //try to determine the item's original stack position so we can restore it
    int siblingIndex = ((AccessibleFxItem*)d->target)->siblingIndex() + 1;
    QList<QGraphicsItem*> children = d->origParent->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QFxItem *child = qobject_cast<QFxItem*>(children.at(i));
        if (!child)
            continue;
        if (((AccessibleFxItem*)child)->siblingIndex() == siblingIndex) {
            d->origStackBefore = child;
            break;
        }
    }
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

bool QmlParentChange::override(ActionEvent*other)
{
    Q_D(QmlParentChange);
    if (other->typeName() != QLatin1String("ParentChange"))
        return false;
    if (QmlParentChange *otherPC = static_cast<QmlParentChange*>(other))
        return (d->target == otherPC->object());
    return false;
}

class QmlStateChangeScriptPrivate : public QObjectPrivate
{
public:
    QmlStateChangeScriptPrivate() {}

    QString script;
    QString name;
};

/*!
    \qmlclass StateChangeScript QmlStateChangeScript
    \brief The StateChangeScript element allows you to run a script in a state.
*/
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,StateChangeScript,QmlStateChangeScript)
QmlStateChangeScript::QmlStateChangeScript(QObject *parent)
: QmlStateOperation(*(new QmlStateChangeScriptPrivate), parent)
{
}

QmlStateChangeScript::~QmlStateChangeScript()
{
}

/*!
    \qmlproperty string StateChangeScript::script
    This property holds the script to run when the state is current.
*/
QString QmlStateChangeScript::script() const
{
    Q_D(const QmlStateChangeScript);
    return d->script;
}

void QmlStateChangeScript::setScript(const QString &s)
{
    Q_D(QmlStateChangeScript);
    d->script = s;
}

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
    if (!d->script.isEmpty()) {
        QmlExpression expr(qmlContext(this), d->script, this);
        expr.setTrackChange(false);
        expr.value();
    }
}

QmlStateChangeScript::ActionList QmlStateChangeScript::actions()
{
    ActionList rv;
    Action a;
    a.event = this;
    rv << a;
    return rv;
}

/*!
    \qmlclass AnchorChanges
    \brief The AnchorChanges element allows you to change the anchors of an item in a state.

    \snippet examples/declarative/anchors/anchor-changes.qml 0

    For more information on anchors see \l {anchor-layout}{Anchor Layouts}.
*/

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,AnchorChanges,QmlAnchorChanges)

class QmlAnchorChangesPrivate : public QObjectPrivate
{
public:
    QmlAnchorChangesPrivate() : target(0) {}

    QString name;
    QFxItem *target;
    QString resetString;
    QStringList resetList;
    QFxAnchorLine left;
    QFxAnchorLine right;
    QFxAnchorLine horizontalCenter;
    QFxAnchorLine top;
    QFxAnchorLine bottom;
    QFxAnchorLine verticalCenter;
    QFxAnchorLine baseline;

    QFxAnchorLine origLeft;
    QFxAnchorLine origRight;
    QFxAnchorLine origHCenter;
    QFxAnchorLine origTop;
    QFxAnchorLine origBottom;
    QFxAnchorLine origVCenter;
    QFxAnchorLine origBaseline;
    qreal origX;
    qreal origY;
    qreal origWidth;
    qreal origHeight;
};

/*!
    \qmlproperty Object AnchorChanges::target
    This property holds the object that the anchors to change belong to
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
    Action a;
    a.event = this;
    return ActionList() << a;
}

QFxItem *QmlAnchorChanges::object() const
{
    Q_D(const QmlAnchorChanges);
    return d->target;
}

void QmlAnchorChanges::setObject(QFxItem *target)
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

QFxAnchorLine QmlAnchorChanges::left() const
{
    Q_D(const QmlAnchorChanges);
    return d->left;
}

void QmlAnchorChanges::setLeft(const QFxAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->left = edge;
}

QFxAnchorLine QmlAnchorChanges::right() const
{
    Q_D(const QmlAnchorChanges);
    return d->right;
}

void QmlAnchorChanges::setRight(const QFxAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->right = edge;
}

QFxAnchorLine QmlAnchorChanges::horizontalCenter() const
{
    Q_D(const QmlAnchorChanges);
    return d->horizontalCenter;
}

void QmlAnchorChanges::setHorizontalCenter(const QFxAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->horizontalCenter = edge;
}

QFxAnchorLine QmlAnchorChanges::top() const
{
    Q_D(const QmlAnchorChanges);
    return d->top;
}

void QmlAnchorChanges::setTop(const QFxAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->top = edge;
}

QFxAnchorLine QmlAnchorChanges::bottom() const
{
    Q_D(const QmlAnchorChanges);
    return d->bottom;
}

void QmlAnchorChanges::setBottom(const QFxAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->bottom = edge;
}

QFxAnchorLine QmlAnchorChanges::verticalCenter() const
{
    Q_D(const QmlAnchorChanges);
    return d->verticalCenter;
}

void QmlAnchorChanges::setVerticalCenter(const QFxAnchorLine &edge)
{
    Q_D(QmlAnchorChanges);
    d->verticalCenter = edge;
}

QFxAnchorLine QmlAnchorChanges::baseline() const
{
    Q_D(const QmlAnchorChanges);
    return d->baseline;
}

void QmlAnchorChanges::setBaseline(const QFxAnchorLine &edge)
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
    if (d->left.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->left);
    if (d->right.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setRight(d->right);
    if (d->horizontalCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setHorizontalCenter(d->horizontalCenter);
    if (d->top.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setTop(d->top);
    if (d->bottom.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->bottom);
    if (d->verticalCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setVerticalCenter(d->verticalCenter);
    if (d->baseline.anchorLine != QFxAnchorLine::Invalid)
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
    if (d->origLeft.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->origLeft);
    if (d->origRight.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setRight(d->origRight);
    if (d->origHCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setHorizontalCenter(d->origHCenter);
    if (d->origTop.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setTop(d->origTop);
    if (d->origBottom.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->origBottom);
    if (d->origVCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setVerticalCenter(d->origVCenter);
    if (d->origBaseline.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setBaseline(d->origBaseline);
}

QString QmlAnchorChanges::typeName() const
{
    return QLatin1String("AnchorChanges");
}

QList<Action> QmlAnchorChanges::extraActions()
{
    Q_D(QmlAnchorChanges);
    QList<Action> extra;

    //### try to be smarter about which ones we add.
    //    or short-circuit later on if they haven't actually changed.
    //    we shouldn't set explicit width if there wasn't one before.
    if (d->target) {
        Action a;
        a.fromValue = d->origX;
        a.property = QmlMetaProperty(d->target, QLatin1String("x"));
        extra << a;

        a.fromValue = d->origY;
        a.property = QmlMetaProperty(d->target, QLatin1String("y"));
        extra << a;

        a.fromValue = d->origWidth;
        a.property = QmlMetaProperty(d->target, QLatin1String("width"));
        extra << a;

        a.fromValue = d->origHeight;
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
}

void QmlAnchorChanges::clearForwardBindings()
{
    Q_D(QmlAnchorChanges);
    d->origX = d->target->x();
    d->origY = d->target->y();
    d->origWidth = d->target->width();
    d->origHeight = d->target->height();

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
    if (d->left.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->right.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->horizontalCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->top.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->bottom.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->verticalCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->baseline.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();
}

void QmlAnchorChanges::clearReverseBindings()
{
    Q_D(QmlAnchorChanges);
    d->origX = d->target->x();
    d->origY = d->target->y();
    d->origWidth = d->target->width();
    d->origHeight = d->target->height();

    //reset any anchors that were set in the state
    if (d->left.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->right.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->horizontalCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->top.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->bottom.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->verticalCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->baseline.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();

    //reset any anchors that were set in the original state
    if (d->origLeft.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->origRight.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->origHCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetHorizontalCenter();
    if (d->origTop.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->origBottom.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
    if (d->origVCenter.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetVerticalCenter();
    if (d->origBaseline.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetBaseline();
}

bool QmlAnchorChanges::override(ActionEvent*other)
{
    if (other->typeName() != QLatin1String("AnchorChanges"))
        return false;
    if (static_cast<ActionEvent*>(this) == other)
        return true;
    //### can we do any other meaningful comparison? Do we need to attempt to merge the two
    //    somehow if they have the same target and some of the same anchors?
    return false;
}

QT_END_NAMESPACE

#include "qmlstateoperations.moc"
#include "moc_qmlstateoperations.cpp"

