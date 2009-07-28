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

QT_BEGIN_NAMESPACE

class QmlParentChangePrivate : public QObjectPrivate
{
public:
    QmlParentChangePrivate() : target(0), parent(0), origParent(0) {}

    QFxItem *target;
    QFxItem *parent;
    QFxItem *origParent;

    void doChange(QFxItem *targetParent);
};

void QmlParentChangePrivate::doChange(QFxItem *targetParent)
{
    if (targetParent && target && target->itemParent()) {
        QPointF me = target->itemParent()->mapToScene(QPointF(0,0));
        QPointF them = targetParent->mapToScene(QPointF(0,0));

        QPointF themx = targetParent->mapToScene(QPointF(1,0));
        QPointF themy = targetParent->mapToScene(QPointF(0,1));

        themx -= them;
        themy -= them;

        target->setItemParent(targetParent);

        // XXX - this is silly and will only work in a few cases

        /*
            xDiff = rx * themx_x + ry * themy_x
            yDiff = rx * themx_y + ry * themy_y
         */

        qreal rx = 0;
        qreal ry = 0;
        qreal xDiff = them.x() - me.x();
        qreal yDiff = them.y() - me.y();


        if (themx.x() == 0.) {
            ry = xDiff / themy.x();
            rx = (yDiff - ry * themy.y()) / themx.y();
        } else if (themy.x() == 0.) {
            rx = xDiff / themx.x();
            ry = (yDiff - rx * themx.y()) / themy.y();
        } else if (themx.y() == 0.) {
            ry = yDiff / themy.y();
            rx = (xDiff - ry * themy.x()) / themx.x();
        } else if (themy.y() == 0.) {
            rx = yDiff / themx.y();
            ry = (xDiff - rx * themx.x()) / themy.x();
        } else {
            qreal div = (themy.x() * themx.y() - themy.y() * themx.x());

            if (div != 0.)
                rx = (themx.y() * xDiff - themx.x() * yDiff) / div;

           if (themy.y() != 0.) ry = (yDiff - rx * themx.y()) / themy.y();
        }

        target->setX(target->x() - rx);
        target->setY(target->y() - ry);
    } else if (target) {
        target->setItemParent(targetParent);
    }
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

void QmlParentChange::execute()
{
    Q_D(QmlParentChange);
    if (d->target)
        d->origParent = d->target->itemParent();
    d->doChange(d->parent);
}

bool QmlParentChange::isReversable()
{
    return true;
}

void QmlParentChange::reverse()
{
    Q_D(QmlParentChange);
    d->doChange(d->origParent);
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

class QmlRunScriptPrivate : public QObjectPrivate
{
public:
    QmlRunScriptPrivate() {}

    QString script;
    QString name;
};

/*!
    \qmlclass RunScript QmlRunScript
    \brief The RunScript element allows you to run a script in a state.
*/
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,RunScript,QmlRunScript)
QmlRunScript::QmlRunScript(QObject *parent)
: QmlStateOperation(*(new QmlRunScriptPrivate), parent)
{
}

QmlRunScript::~QmlRunScript()
{
}

/*!
    \qmlproperty string RunScript::script
    This property holds the script to run when the state is current.
*/
QString QmlRunScript::script() const
{
    Q_D(const QmlRunScript);
    return d->script;
}

void QmlRunScript::setScript(const QString &s)
{
    Q_D(QmlRunScript);
    d->script = s;
}

QString QmlRunScript::name() const
{
    Q_D(const QmlRunScript);
    return d->name;
}

void QmlRunScript::setName(const QString &n)
{
    Q_D(QmlRunScript);
    d->name = n;
}

void QmlRunScript::execute()
{
    Q_D(QmlRunScript);
    if (!d->script.isEmpty()) {
        QmlExpression expr(qmlContext(this), d->script, this);
        expr.setTrackChange(false);
        expr.value();
    }
}

QmlRunScript::ActionList QmlRunScript::actions()
{
    ActionList rv;
    Action a;
    a.event = this;
    rv << a;
    return rv;
}

/*!
    \qmlclass SetAnchors
    \brief The SetAnchors element allows you to change anchors in a state.
*/

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,SetAnchors,QmlSetAnchors)

class QmlSetAnchorsPrivate : public QObjectPrivate
{
public:
    QmlSetAnchorsPrivate() : target(0) {}

    QString name;
    QFxItem *target;
    QString resetString;
    QStringList resetList;
    QFxAnchorLine left;
    QFxAnchorLine right;
    QFxAnchorLine top;
    QFxAnchorLine bottom;
    QFxAnchorLine origLeft;
    QFxAnchorLine origRight;
    QFxAnchorLine origTop;
    QFxAnchorLine origBottom;
    qreal origX;
    qreal origY;
    qreal origWidth;
    qreal origHeight;
};

QmlSetAnchors::QmlSetAnchors(QObject *parent)
 : QmlStateOperation(*(new QmlSetAnchorsPrivate), parent)
{
}

QmlSetAnchors::~QmlSetAnchors()
{
}

QmlSetAnchors::ActionList QmlSetAnchors::actions()
{
    Action a;
    a.event = this;
    return ActionList() << a;
}

QFxItem *QmlSetAnchors::object() const
{
    Q_D(const QmlSetAnchors);
    return d->target;
}

void QmlSetAnchors::setObject(QFxItem *target)
{
    Q_D(QmlSetAnchors);
    d->target = target;
}

QString QmlSetAnchors::reset() const
{
    Q_D(const QmlSetAnchors);
    return d->resetString;
}

void QmlSetAnchors::setReset(const QString &reset)
{
    Q_D(QmlSetAnchors);
    d->resetString = reset;
    d->resetList = d->resetString.split(QLatin1Char(','));
}

QFxAnchorLine QmlSetAnchors::left() const
{
    Q_D(const QmlSetAnchors);
    return d->left;
}

void QmlSetAnchors::setLeft(const QFxAnchorLine &edge)
{
    Q_D(QmlSetAnchors);
    d->left = edge;
}

QFxAnchorLine QmlSetAnchors::right() const
{
    Q_D(const QmlSetAnchors);
    return d->right;
}

void QmlSetAnchors::setRight(const QFxAnchorLine &edge)
{
    Q_D(QmlSetAnchors);
    d->right = edge;
}

QFxAnchorLine QmlSetAnchors::top() const
{
    Q_D(const QmlSetAnchors);
    return d->top;
}

void QmlSetAnchors::setTop(const QFxAnchorLine &edge)
{
    Q_D(QmlSetAnchors);
    d->top = edge;
}

QFxAnchorLine QmlSetAnchors::bottom() const
{
    Q_D(const QmlSetAnchors);
    return d->bottom;
}

void QmlSetAnchors::setBottom(const QFxAnchorLine &edge)
{
    Q_D(QmlSetAnchors);
    d->bottom = edge;
}

void QmlSetAnchors::execute()
{
    Q_D(QmlSetAnchors);
    if (!d->target)
        return;

    //set any anchors that have been specified
    if (d->left.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->left);
    if (d->right.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setRight(d->right);
    if (d->top.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setTop(d->top);
    if (d->bottom.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->bottom);
}

bool QmlSetAnchors::isReversable()
{
    return true;
}

void QmlSetAnchors::reverse()
{
    Q_D(QmlSetAnchors);
    if (!d->target)
        return;

    //restore previous anchors
    if (d->origLeft.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setLeft(d->origLeft);
    if (d->origRight.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setRight(d->origRight);
    if (d->origTop.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setTop(d->origTop);
    if (d->origBottom.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->setBottom(d->origBottom);

}

QString QmlSetAnchors::typeName() const
{
    return QLatin1String("SetAnchors");
}

QList<Action> QmlSetAnchors::extraActions()
{
    Q_D(QmlSetAnchors);
    QList<Action> extra;

    //### try to be smarter about which ones we add.
    //    or short-circuit later on if they haven't actually changed.
    //    we shouldn't set explicit width if there wasn't one before.
    if (d->target) {
        Action a;
        a.fromValue = d->origX;
        a.property = QmlMetaProperty(d->target, "x");
        extra << a;

        a.fromValue = d->origY;
        a.property = QmlMetaProperty(d->target, "y");
        extra << a;

        a.fromValue = d->origWidth;
        a.property = QmlMetaProperty(d->target, "width");
        extra << a;

        a.fromValue = d->origHeight;
        a.property = QmlMetaProperty(d->target, "height");
        extra << a;
    }
    
    return extra;
}

bool QmlSetAnchors::changesBindings()
{
    return true;
}

void QmlSetAnchors::clearForwardBindings()
{
    Q_D(QmlSetAnchors);
    d->origLeft = d->target->anchors()->left();
    d->origRight = d->target->anchors()->right();
    d->origTop = d->target->anchors()->top();
    d->origBottom = d->target->anchors()->bottom();
    d->origX = d->target->x();
    d->origY = d->target->y();
    d->origWidth = d->target->width();
    d->origHeight = d->target->height();

    //reset any anchors that have been specified
    if (d->resetList.contains(QLatin1String("left")))
        d->target->anchors()->resetLeft();
    if (d->resetList.contains(QLatin1String("right")))
        d->target->anchors()->resetRight();
    if (d->resetList.contains(QLatin1String("top")))
        d->target->anchors()->resetTop();
    if (d->resetList.contains(QLatin1String("bottom")))
        d->target->anchors()->resetBottom();
}

void QmlSetAnchors::clearReverseBindings()
{
    Q_D(QmlSetAnchors);
    d->origX = d->target->x();
    d->origY = d->target->y();
    d->origWidth = d->target->width();
    d->origHeight = d->target->height();

    //reset any anchors that were set in the state
    if (d->left.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetLeft();
    if (d->right.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetRight();
    if (d->top.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetTop();
    if (d->bottom.anchorLine != QFxAnchorLine::Invalid)
        d->target->anchors()->resetBottom();
}

QT_END_NAMESPACE
