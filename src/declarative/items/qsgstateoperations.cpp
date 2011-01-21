// Commit: 726a8b16c52fe4608c89d740b47361a2b073ce01
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

#include "qsgstateoperations_p.h"
#include "qsgitem_p.h"

#include <private/qdeclarativestate_p_p.h>

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

class QSGParentChangePrivate : public QDeclarativeStateOperationPrivate
{
    Q_DECLARE_PUBLIC(QSGParentChange)
public:
    QSGParentChangePrivate() : target(0), parent(0), origParent(0), origStackBefore(0),
        rewindParent(0), rewindStackBefore(0) {}

    QSGItem *target;
    QDeclarativeGuard<QSGItem> parent;
    QDeclarativeGuard<QSGItem> origParent;
    QDeclarativeGuard<QSGItem> origStackBefore;
    QSGItem *rewindParent;
    QSGItem *rewindStackBefore;

    QDeclarativeNullableValue<QDeclarativeScriptString> xString;
    QDeclarativeNullableValue<QDeclarativeScriptString> yString;
    QDeclarativeNullableValue<QDeclarativeScriptString> widthString;
    QDeclarativeNullableValue<QDeclarativeScriptString> heightString;
    QDeclarativeNullableValue<QDeclarativeScriptString> scaleString;
    QDeclarativeNullableValue<QDeclarativeScriptString> rotationString;

    void doChange(QSGItem *targetParent, QSGItem *stackBefore = 0);
};

void QSGParentChangePrivate::doChange(QSGItem *targetParent, QSGItem *stackBefore)
{
    if (targetParent && target && target->parentItem()) {
        Q_Q(QSGParentChange);
        bool ok;
        const QTransform &transform = target->parentItem()->itemTransform(targetParent, &ok);
        if (transform.type() >= QTransform::TxShear || !ok) {
            qmlInfo(q) << QSGParentChange::tr("Unable to preserve appearance under complex transform");
            ok = false;
        }

        qreal scale = 1;
        qreal rotation = 0;
        bool isRotate = (transform.type() == QTransform::TxRotate) || (transform.m11() < 0);
        if (ok && !isRotate) {
            if (transform.m11() == transform.m22())
                scale = transform.m11();
            else {
                qmlInfo(q) << QSGParentChange::tr("Unable to preserve appearance under non-uniform scale");
                ok = false;
            }
        } else if (ok && isRotate) {
            if (transform.m11() == transform.m22())
                scale = qSqrt(transform.m11()*transform.m11() + transform.m12()*transform.m12());
            else {
                qmlInfo(q) << QSGParentChange::tr("Unable to preserve appearance under non-uniform scale");
                ok = false;
            }

            if (scale != 0)
                rotation = atan2(transform.m12()/scale, transform.m11()/scale) * 180/M_PI;
            else {
                qmlInfo(q) << QSGParentChange::tr("Unable to preserve appearance under scale of 0");
                ok = false;
            }
        }

        const QPointF &point = transform.map(QPointF(target->x(),target->y()));
        qreal x = point.x();
        qreal y = point.y();

        // setParentItem will update the transformOriginPoint if needed
        target->setParentItem(targetParent);

        if (ok && target->transformOrigin() != QSGItem::TopLeft) {
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

QSGParentChange::QSGParentChange(QObject *parent)
    : QDeclarativeStateOperation(*(new QSGParentChangePrivate), parent)
{
}

QSGParentChange::~QSGParentChange()
{
}

QDeclarativeScriptString QSGParentChange::x() const
{
    Q_D(const QSGParentChange);
    return d->xString.value;
}

void QSGParentChange::setX(QDeclarativeScriptString x)
{
    Q_D(QSGParentChange);
    d->xString = x;
}

bool QSGParentChange::xIsSet() const
{
    Q_D(const QSGParentChange);
    return d->xString.isValid();
}

QDeclarativeScriptString QSGParentChange::y() const
{
    Q_D(const QSGParentChange);
    return d->yString.value;
}

void QSGParentChange::setY(QDeclarativeScriptString y)
{
    Q_D(QSGParentChange);
    d->yString = y;
}

bool QSGParentChange::yIsSet() const
{
    Q_D(const QSGParentChange);
    return d->yString.isValid();
}

QDeclarativeScriptString QSGParentChange::width() const
{
    Q_D(const QSGParentChange);
    return d->widthString.value;
}

void QSGParentChange::setWidth(QDeclarativeScriptString width)
{
    Q_D(QSGParentChange);
    d->widthString = width;
}

bool QSGParentChange::widthIsSet() const
{
    Q_D(const QSGParentChange);
    return d->widthString.isValid();
}

QDeclarativeScriptString QSGParentChange::height() const
{
    Q_D(const QSGParentChange);
    return d->heightString.value;
}

void QSGParentChange::setHeight(QDeclarativeScriptString height)
{
    Q_D(QSGParentChange);
    d->heightString = height;
}

bool QSGParentChange::heightIsSet() const
{
    Q_D(const QSGParentChange);
    return d->heightString.isValid();
}

QDeclarativeScriptString QSGParentChange::scale() const
{
    Q_D(const QSGParentChange);
    return d->scaleString.value;
}

void QSGParentChange::setScale(QDeclarativeScriptString scale)
{
    Q_D(QSGParentChange);
    d->scaleString = scale;
}

bool QSGParentChange::scaleIsSet() const
{
    Q_D(const QSGParentChange);
    return d->scaleString.isValid();
}

QDeclarativeScriptString QSGParentChange::rotation() const
{
    Q_D(const QSGParentChange);
    return d->rotationString.value;
}

void QSGParentChange::setRotation(QDeclarativeScriptString rotation)
{
    Q_D(QSGParentChange);
    d->rotationString = rotation;
}

bool QSGParentChange::rotationIsSet() const
{
    Q_D(const QSGParentChange);
    return d->rotationString.isValid();
}

QSGItem *QSGParentChange::originalParent() const
{
    Q_D(const QSGParentChange);
    return d->origParent;
}

QSGItem *QSGParentChange::object() const
{
    Q_D(const QSGParentChange);
    return d->target;
}

void QSGParentChange::setObject(QSGItem *target)
{
    Q_D(QSGParentChange);
    d->target = target;
}

QSGItem *QSGParentChange::parent() const
{
    Q_D(const QSGParentChange);
    return d->parent;
}

void QSGParentChange::setParent(QSGItem *parent)
{
    Q_D(QSGParentChange);
    d->parent = parent;
}

QDeclarativeStateOperation::ActionList QSGParentChange::actions()
{
    Q_D(QSGParentChange);
    if (!d->target || !d->parent)
        return ActionList();

    ActionList actions;

    QDeclarativeAction a;
    a.event = this;
    actions << a;

    if (d->xString.isValid()) {
        bool ok = false;
        QString script = d->xString.value.script();
        qreal x = script.toFloat(&ok);
        if (ok) {
            QDeclarativeAction xa(d->target, QLatin1String("x"), x);
            actions << xa;
        } else {
            QDeclarativeBinding *newBinding = new QDeclarativeBinding(script, d->target, qmlContext(this));
            newBinding->setTarget(QDeclarativeProperty(d->target, QLatin1String("x")));
            QDeclarativeAction xa;
            xa.property = newBinding->property();
            xa.toBinding = newBinding;
            xa.fromValue = xa.property.read();
            xa.deletableToBinding = true;
            actions << xa;
        }
    }

    if (d->yString.isValid()) {
        bool ok = false;
        QString script = d->yString.value.script();
        qreal y = script.toFloat(&ok);
        if (ok) {
            QDeclarativeAction ya(d->target, QLatin1String("y"), y);
            actions << ya;
        } else {
            QDeclarativeBinding *newBinding = new QDeclarativeBinding(script, d->target, qmlContext(this));
            newBinding->setTarget(QDeclarativeProperty(d->target, QLatin1String("y")));
            QDeclarativeAction ya;
            ya.property = newBinding->property();
            ya.toBinding = newBinding;
            ya.fromValue = ya.property.read();
            ya.deletableToBinding = true;
            actions << ya;
        }
    }

    if (d->scaleString.isValid()) {
        bool ok = false;
        QString script = d->scaleString.value.script();
        qreal scale = script.toFloat(&ok);
        if (ok) {
            QDeclarativeAction sa(d->target, QLatin1String("scale"), scale);
            actions << sa;
        } else {
            QDeclarativeBinding *newBinding = new QDeclarativeBinding(script, d->target, qmlContext(this));
            newBinding->setTarget(QDeclarativeProperty(d->target, QLatin1String("scale")));
            QDeclarativeAction sa;
            sa.property = newBinding->property();
            sa.toBinding = newBinding;
            sa.fromValue = sa.property.read();
            sa.deletableToBinding = true;
            actions << sa;
        }
    }

    if (d->rotationString.isValid()) {
        bool ok = false;
        QString script = d->rotationString.value.script();
        qreal rotation = script.toFloat(&ok);
        if (ok) {
            QDeclarativeAction ra(d->target, QLatin1String("rotation"), rotation);
            actions << ra;
        } else {
            QDeclarativeBinding *newBinding = new QDeclarativeBinding(script, d->target, qmlContext(this));
            newBinding->setTarget(QDeclarativeProperty(d->target, QLatin1String("rotation")));
            QDeclarativeAction ra;
            ra.property = newBinding->property();
            ra.toBinding = newBinding;
            ra.fromValue = ra.property.read();
            ra.deletableToBinding = true;
            actions << ra;
        }
    }

    if (d->widthString.isValid()) {
        bool ok = false;
        QString script = d->widthString.value.script();
        qreal width = script.toFloat(&ok);
        if (ok) {
            QDeclarativeAction wa(d->target, QLatin1String("width"), width);
            actions << wa;
        } else {
            QDeclarativeBinding *newBinding = new QDeclarativeBinding(script, d->target, qmlContext(this));
            newBinding->setTarget(QDeclarativeProperty(d->target, QLatin1String("width")));
            QDeclarativeAction wa;
            wa.property = newBinding->property();
            wa.toBinding = newBinding;
            wa.fromValue = wa.property.read();
            wa.deletableToBinding = true;
            actions << wa;
        }
    }

    if (d->heightString.isValid()) {
        bool ok = false;
        QString script = d->heightString.value.script();
        qreal height = script.toFloat(&ok);
        if (ok) {
            QDeclarativeAction ha(d->target, QLatin1String("height"), height);
            actions << ha;
        } else {
            QDeclarativeBinding *newBinding = new QDeclarativeBinding(script, d->target, qmlContext(this));
            newBinding->setTarget(QDeclarativeProperty(d->target, QLatin1String("height")));
            QDeclarativeAction ha;
            ha.property = newBinding->property();
            ha.toBinding = newBinding;
            ha.fromValue = ha.property.read();
            ha.deletableToBinding = true;
            actions << ha;
        }
    }

    return actions;
}

void QSGParentChange::saveOriginals()
{
    Q_D(QSGParentChange);
    saveCurrentValues();
    d->origParent = d->rewindParent;
    d->origStackBefore = d->rewindStackBefore;
}

/*void QSGParentChange::copyOriginals(QDeclarativeActionEvent *other)
{
    Q_D(QSGParentChange);
    QSGParentChange *pc = static_cast<QSGParentChange*>(other);

    d->origParent = pc->d_func()->rewindParent;
    d->origStackBefore = pc->d_func()->rewindStackBefore;

    saveCurrentValues();
}*/

void QSGParentChange::execute(Reason)
{
    Q_D(QSGParentChange);
    d->doChange(d->parent);
}

bool QSGParentChange::isReversable()
{
    return true;
}

void QSGParentChange::reverse(Reason)
{
    Q_D(QSGParentChange);
    d->doChange(d->origParent, d->origStackBefore);
}

QString QSGParentChange::typeName() const
{
    return QLatin1String("ParentChange");
}

bool QSGParentChange::override(QDeclarativeActionEvent*other)
{
    Q_D(QSGParentChange);
    if (other->typeName() != QLatin1String("ParentChange"))
        return false;
    if (QSGParentChange *otherPC = static_cast<QSGParentChange*>(other))
        return (d->target == otherPC->object());
    return false;
}

void QSGParentChange::saveCurrentValues()
{
    Q_D(QSGParentChange);
    if (!d->target) {
        d->rewindParent = 0;
        d->rewindStackBefore = 0;
        return;
    }

    d->rewindParent = d->target->parentItem();
    d->rewindStackBefore = 0;

    if (!d->rewindParent)
        return;

    QList<QSGItem *> children = d->rewindParent->childItems();
    for (int ii = 0; ii < children.count() - 1; ++ii) {
        if (children.at(ii) == d->target) {
            d->rewindStackBefore = children.at(ii + 1);
            break;
        }
    }
}

void QSGParentChange::rewind()
{
    Q_D(QSGParentChange);
    d->doChange(d->rewindParent, d->rewindStackBefore);
}

class QSGAnchorSetPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSGAnchorSet)
public:
    QSGAnchorSetPrivate()
      : usedAnchors(0), resetAnchors(0), fill(0),
        centerIn(0)/*, leftMargin(0), rightMargin(0), topMargin(0), bottomMargin(0),
        margins(0), vCenterOffset(0), hCenterOffset(0), baselineOffset(0)*/
    {
    }

    QSGAnchors::Anchors usedAnchors;
    QSGAnchors::Anchors resetAnchors;

    QSGItem *fill;
    QSGItem *centerIn;

    QDeclarativeScriptString leftScript;
    QDeclarativeScriptString rightScript;
    QDeclarativeScriptString topScript;
    QDeclarativeScriptString bottomScript;
    QDeclarativeScriptString hCenterScript;
    QDeclarativeScriptString vCenterScript;
    QDeclarativeScriptString baselineScript;

    /*qreal leftMargin;
    qreal rightMargin;
    qreal topMargin;
    qreal bottomMargin;
    qreal margins;
    qreal vCenterOffset;
    qreal hCenterOffset;
    qreal baselineOffset;*/
};

QSGAnchorSet::QSGAnchorSet(QObject *parent)
  : QObject(*new QSGAnchorSetPrivate, parent)
{
}

QSGAnchorSet::~QSGAnchorSet()
{
}

QDeclarativeScriptString QSGAnchorSet::top() const
{
    Q_D(const QSGAnchorSet);
    return d->topScript;
}

void QSGAnchorSet::setTop(const QDeclarativeScriptString &edge)
{
    Q_D(QSGAnchorSet);
    d->usedAnchors |= QSGAnchors::TopAnchor;
    d->topScript = edge;
    if (edge.script() == QLatin1String("undefined"))
        resetTop();
}

void QSGAnchorSet::resetTop()
{
    Q_D(QSGAnchorSet);
    d->usedAnchors &= ~QSGAnchors::TopAnchor;
    d->topScript = QDeclarativeScriptString();
    d->resetAnchors |= QSGAnchors::TopAnchor;
}

QDeclarativeScriptString QSGAnchorSet::bottom() const
{
    Q_D(const QSGAnchorSet);
    return d->bottomScript;
}

void QSGAnchorSet::setBottom(const QDeclarativeScriptString &edge)
{
    Q_D(QSGAnchorSet);
    d->usedAnchors |= QSGAnchors::BottomAnchor;
    d->bottomScript = edge;
    if (edge.script() == QLatin1String("undefined"))
        resetBottom();
}

void QSGAnchorSet::resetBottom()
{
    Q_D(QSGAnchorSet);
    d->usedAnchors &= ~QSGAnchors::BottomAnchor;
    d->bottomScript = QDeclarativeScriptString();
    d->resetAnchors |= QSGAnchors::BottomAnchor;
}

QDeclarativeScriptString QSGAnchorSet::verticalCenter() const
{
    Q_D(const QSGAnchorSet);
    return d->vCenterScript;
}

void QSGAnchorSet::setVerticalCenter(const QDeclarativeScriptString &edge)
{
    Q_D(QSGAnchorSet);
    d->usedAnchors |= QSGAnchors::VCenterAnchor;
    d->vCenterScript = edge;
    if (edge.script() == QLatin1String("undefined"))
        resetVerticalCenter();
}

void QSGAnchorSet::resetVerticalCenter()
{
    Q_D(QSGAnchorSet);
    d->usedAnchors &= ~QSGAnchors::VCenterAnchor;
    d->vCenterScript = QDeclarativeScriptString();
    d->resetAnchors |= QSGAnchors::VCenterAnchor;
}

QDeclarativeScriptString QSGAnchorSet::baseline() const
{
    Q_D(const QSGAnchorSet);
    return d->baselineScript;
}

void QSGAnchorSet::setBaseline(const QDeclarativeScriptString &edge)
{
    Q_D(QSGAnchorSet);
    d->usedAnchors |= QSGAnchors::BaselineAnchor;
    d->baselineScript = edge;
    if (edge.script() == QLatin1String("undefined"))
        resetBaseline();
}

void QSGAnchorSet::resetBaseline()
{
    Q_D(QSGAnchorSet);
    d->usedAnchors &= ~QSGAnchors::BaselineAnchor;
    d->baselineScript = QDeclarativeScriptString();
    d->resetAnchors |= QSGAnchors::BaselineAnchor;
}

QDeclarativeScriptString QSGAnchorSet::left() const
{
    Q_D(const QSGAnchorSet);
    return d->leftScript;
}

void QSGAnchorSet::setLeft(const QDeclarativeScriptString &edge)
{
    Q_D(QSGAnchorSet);
    d->usedAnchors |= QSGAnchors::LeftAnchor;
    d->leftScript = edge;
    if (edge.script() == QLatin1String("undefined"))
        resetLeft();
}

void QSGAnchorSet::resetLeft()
{
    Q_D(QSGAnchorSet);
    d->usedAnchors &= ~QSGAnchors::LeftAnchor;
    d->leftScript = QDeclarativeScriptString();
    d->resetAnchors |= QSGAnchors::LeftAnchor;
}

QDeclarativeScriptString QSGAnchorSet::right() const
{
    Q_D(const QSGAnchorSet);
    return d->rightScript;
}

void QSGAnchorSet::setRight(const QDeclarativeScriptString &edge)
{
    Q_D(QSGAnchorSet);
    d->usedAnchors |= QSGAnchors::RightAnchor;
    d->rightScript = edge;
    if (edge.script() == QLatin1String("undefined"))
        resetRight();
}

void QSGAnchorSet::resetRight()
{
    Q_D(QSGAnchorSet);
    d->usedAnchors &= ~QSGAnchors::RightAnchor;
    d->rightScript = QDeclarativeScriptString();
    d->resetAnchors |= QSGAnchors::RightAnchor;
}

QDeclarativeScriptString QSGAnchorSet::horizontalCenter() const
{
    Q_D(const QSGAnchorSet);
    return d->hCenterScript;
}

void QSGAnchorSet::setHorizontalCenter(const QDeclarativeScriptString &edge)
{
    Q_D(QSGAnchorSet);
    d->usedAnchors |= QSGAnchors::HCenterAnchor;
    d->hCenterScript = edge;
    if (edge.script() == QLatin1String("undefined"))
        resetHorizontalCenter();
}

void QSGAnchorSet::resetHorizontalCenter()
{
    Q_D(QSGAnchorSet);
    d->usedAnchors &= ~QSGAnchors::HCenterAnchor;
    d->hCenterScript = QDeclarativeScriptString();
    d->resetAnchors |= QSGAnchors::HCenterAnchor;
}

QSGItem *QSGAnchorSet::fill() const
{
    Q_D(const QSGAnchorSet);
    return d->fill;
}

void QSGAnchorSet::setFill(QSGItem *f)
{
    Q_D(QSGAnchorSet);
    d->fill = f;
}

void QSGAnchorSet::resetFill()
{
    setFill(0);
}

QSGItem *QSGAnchorSet::centerIn() const
{
    Q_D(const QSGAnchorSet);
    return d->centerIn;
}

void QSGAnchorSet::setCenterIn(QSGItem* c)
{
    Q_D(QSGAnchorSet);
    d->centerIn = c;
}

void QSGAnchorSet::resetCenterIn()
{
    setCenterIn(0);
}


class QSGAnchorChangesPrivate : public QDeclarativeStateOperationPrivate
{
public:
    QSGAnchorChangesPrivate()
        : target(0), anchorSet(new QSGAnchorSet),
          leftBinding(0), rightBinding(0), hCenterBinding(0),
          topBinding(0), bottomBinding(0), vCenterBinding(0), baselineBinding(0),
          origLeftBinding(0), origRightBinding(0), origHCenterBinding(0),
          origTopBinding(0), origBottomBinding(0), origVCenterBinding(0),
          origBaselineBinding(0)
    {

    }
    ~QSGAnchorChangesPrivate() { delete anchorSet; }

    QSGItem *target;
    QSGAnchorSet *anchorSet;

    QDeclarativeBinding *leftBinding;
    QDeclarativeBinding *rightBinding;
    QDeclarativeBinding *hCenterBinding;
    QDeclarativeBinding *topBinding;
    QDeclarativeBinding *bottomBinding;
    QDeclarativeBinding *vCenterBinding;
    QDeclarativeBinding *baselineBinding;

    QDeclarativeAbstractBinding *origLeftBinding;
    QDeclarativeAbstractBinding *origRightBinding;
    QDeclarativeAbstractBinding *origHCenterBinding;
    QDeclarativeAbstractBinding *origTopBinding;
    QDeclarativeAbstractBinding *origBottomBinding;
    QDeclarativeAbstractBinding *origVCenterBinding;
    QDeclarativeAbstractBinding *origBaselineBinding;

    QSGAnchorLine rewindLeft;
    QSGAnchorLine rewindRight;
    QSGAnchorLine rewindHCenter;
    QSGAnchorLine rewindTop;
    QSGAnchorLine rewindBottom;
    QSGAnchorLine rewindVCenter;
    QSGAnchorLine rewindBaseline;

    qreal fromX;
    qreal fromY;
    qreal fromWidth;
    qreal fromHeight;

    qreal toX;
    qreal toY;
    qreal toWidth;
    qreal toHeight;

    qreal rewindX;
    qreal rewindY;
    qreal rewindWidth;
    qreal rewindHeight;

    bool applyOrigLeft;
    bool applyOrigRight;
    bool applyOrigHCenter;
    bool applyOrigTop;
    bool applyOrigBottom;
    bool applyOrigVCenter;
    bool applyOrigBaseline;

    QDeclarativeNullableValue<qreal> origWidth;
    QDeclarativeNullableValue<qreal> origHeight;
    qreal origX;
    qreal origY;

    QList<QDeclarativeAbstractBinding*> oldBindings;

    QDeclarativeProperty leftProp;
    QDeclarativeProperty rightProp;
    QDeclarativeProperty hCenterProp;
    QDeclarativeProperty topProp;
    QDeclarativeProperty bottomProp;
    QDeclarativeProperty vCenterProp;
    QDeclarativeProperty baselineProp;
};

QSGAnchorChanges::QSGAnchorChanges(QObject *parent)
 : QDeclarativeStateOperation(*(new QSGAnchorChangesPrivate), parent)
{
}

QSGAnchorChanges::~QSGAnchorChanges()
{
}

QSGAnchorChanges::ActionList QSGAnchorChanges::actions()
{
    Q_D(QSGAnchorChanges);
    d->leftBinding = d->rightBinding = d->hCenterBinding = d->topBinding
                   = d->bottomBinding = d->vCenterBinding = d->baselineBinding = 0;

    d->leftProp = QDeclarativeProperty(d->target, QLatin1String("anchors.left"));
    d->rightProp = QDeclarativeProperty(d->target, QLatin1String("anchors.right"));
    d->hCenterProp = QDeclarativeProperty(d->target, QLatin1String("anchors.horizontalCenter"));
    d->topProp = QDeclarativeProperty(d->target, QLatin1String("anchors.top"));
    d->bottomProp = QDeclarativeProperty(d->target, QLatin1String("anchors.bottom"));
    d->vCenterProp = QDeclarativeProperty(d->target, QLatin1String("anchors.verticalCenter"));
    d->baselineProp = QDeclarativeProperty(d->target, QLatin1String("anchors.baseline"));

    if (d->anchorSet->d_func()->usedAnchors & QSGAnchors::LeftAnchor) {
        d->leftBinding = new QDeclarativeBinding(d->anchorSet->d_func()->leftScript.script(), d->target, qmlContext(this));
        d->leftBinding->setTarget(d->leftProp);
    }
    if (d->anchorSet->d_func()->usedAnchors & QSGAnchors::RightAnchor) {
        d->rightBinding = new QDeclarativeBinding(d->anchorSet->d_func()->rightScript.script(), d->target, qmlContext(this));
        d->rightBinding->setTarget(d->rightProp);
    }
    if (d->anchorSet->d_func()->usedAnchors & QSGAnchors::HCenterAnchor) {
        d->hCenterBinding = new QDeclarativeBinding(d->anchorSet->d_func()->hCenterScript.script(), d->target, qmlContext(this));
        d->hCenterBinding->setTarget(d->hCenterProp);
    }
    if (d->anchorSet->d_func()->usedAnchors & QSGAnchors::TopAnchor) {
        d->topBinding = new QDeclarativeBinding(d->anchorSet->d_func()->topScript.script(), d->target, qmlContext(this));
        d->topBinding->setTarget(d->topProp);
    }
    if (d->anchorSet->d_func()->usedAnchors & QSGAnchors::BottomAnchor) {
        d->bottomBinding = new QDeclarativeBinding(d->anchorSet->d_func()->bottomScript.script(), d->target, qmlContext(this));
        d->bottomBinding->setTarget(d->bottomProp);
    }
    if (d->anchorSet->d_func()->usedAnchors & QSGAnchors::VCenterAnchor) {
        d->vCenterBinding = new QDeclarativeBinding(d->anchorSet->d_func()->vCenterScript.script(), d->target, qmlContext(this));
        d->vCenterBinding->setTarget(d->vCenterProp);
    }
    if (d->anchorSet->d_func()->usedAnchors & QSGAnchors::BaselineAnchor) {
        d->baselineBinding = new QDeclarativeBinding(d->anchorSet->d_func()->baselineScript.script(), d->target, qmlContext(this));
        d->baselineBinding->setTarget(d->baselineProp);
    }

    QDeclarativeAction a;
    a.event = this;
    return ActionList() << a;
}

QSGAnchorSet *QSGAnchorChanges::anchors()
{
    Q_D(QSGAnchorChanges);
    return d->anchorSet;
}

QSGItem *QSGAnchorChanges::object() const
{
    Q_D(const QSGAnchorChanges);
    return d->target;
}

void QSGAnchorChanges::setObject(QSGItem *target)
{
    Q_D(QSGAnchorChanges);
    d->target = target;
}

void QSGAnchorChanges::execute(Reason reason)
{
    Q_D(QSGAnchorChanges);
    if (!d->target)
        return;

    QSGItemPrivate *targetPrivate = QSGItemPrivate::get(d->target);
    //incorporate any needed "reverts"
    if (d->applyOrigLeft) {
        if (!d->origLeftBinding)
            targetPrivate->anchors()->resetLeft();
        QDeclarativePropertyPrivate::setBinding(d->leftProp, d->origLeftBinding);
    }
    if (d->applyOrigRight) {
        if (!d->origRightBinding)
            targetPrivate->anchors()->resetRight();
        QDeclarativePropertyPrivate::setBinding(d->rightProp, d->origRightBinding);
    }
    if (d->applyOrigHCenter) {
        if (!d->origHCenterBinding)
            targetPrivate->anchors()->resetHorizontalCenter();
        QDeclarativePropertyPrivate::setBinding(d->hCenterProp, d->origHCenterBinding);
    }
    if (d->applyOrigTop) {
        if (!d->origTopBinding)
            targetPrivate->anchors()->resetTop();
        QDeclarativePropertyPrivate::setBinding(d->topProp, d->origTopBinding);
    }
    if (d->applyOrigBottom) {
        if (!d->origBottomBinding)
            targetPrivate->anchors()->resetBottom();
        QDeclarativePropertyPrivate::setBinding(d->bottomProp, d->origBottomBinding);
    }
    if (d->applyOrigVCenter) {
        if (!d->origVCenterBinding)
            targetPrivate->anchors()->resetVerticalCenter();
        QDeclarativePropertyPrivate::setBinding(d->vCenterProp, d->origVCenterBinding);
    }
    if (d->applyOrigBaseline) {
        if (!d->origBaselineBinding)
            targetPrivate->anchors()->resetBaseline();
        QDeclarativePropertyPrivate::setBinding(d->baselineProp, d->origBaselineBinding);
    }

    //destroy old bindings
    if (reason == ActualChange) {
        for (int i = 0; i < d->oldBindings.size(); ++i) {
            QDeclarativeAbstractBinding *binding = d->oldBindings.at(i);
            if (binding)
                binding->destroy();
        }
        d->oldBindings.clear();
    }

    //reset any anchors that have been specified as "undefined"
    if (d->anchorSet->d_func()->resetAnchors & QSGAnchors::LeftAnchor) {
        targetPrivate->anchors()->resetLeft();
        QDeclarativePropertyPrivate::setBinding(d->leftProp, 0);
    }
    if (d->anchorSet->d_func()->resetAnchors & QSGAnchors::RightAnchor) {
        targetPrivate->anchors()->resetRight();
        QDeclarativePropertyPrivate::setBinding(d->rightProp, 0);
    }
    if (d->anchorSet->d_func()->resetAnchors & QSGAnchors::HCenterAnchor) {
        targetPrivate->anchors()->resetHorizontalCenter();
        QDeclarativePropertyPrivate::setBinding(d->hCenterProp, 0);
    }
    if (d->anchorSet->d_func()->resetAnchors & QSGAnchors::TopAnchor) {
        targetPrivate->anchors()->resetTop();
        QDeclarativePropertyPrivate::setBinding(d->topProp, 0);
    }
    if (d->anchorSet->d_func()->resetAnchors & QSGAnchors::BottomAnchor) {
        targetPrivate->anchors()->resetBottom();
        QDeclarativePropertyPrivate::setBinding(d->bottomProp, 0);
    }
    if (d->anchorSet->d_func()->resetAnchors & QSGAnchors::VCenterAnchor) {
        targetPrivate->anchors()->resetVerticalCenter();
        QDeclarativePropertyPrivate::setBinding(d->vCenterProp, 0);
    }
    if (d->anchorSet->d_func()->resetAnchors & QSGAnchors::BaselineAnchor) {
        targetPrivate->anchors()->resetBaseline();
        QDeclarativePropertyPrivate::setBinding(d->baselineProp, 0);
    }

    //set any anchors that have been specified
    if (d->leftBinding)
        QDeclarativePropertyPrivate::setBinding(d->leftBinding->property(), d->leftBinding);
    if (d->rightBinding)
        QDeclarativePropertyPrivate::setBinding(d->rightBinding->property(), d->rightBinding);
    if (d->hCenterBinding)
        QDeclarativePropertyPrivate::setBinding(d->hCenterBinding->property(), d->hCenterBinding);
    if (d->topBinding)
        QDeclarativePropertyPrivate::setBinding(d->topBinding->property(), d->topBinding);
    if (d->bottomBinding)
        QDeclarativePropertyPrivate::setBinding(d->bottomBinding->property(), d->bottomBinding);
    if (d->vCenterBinding)
        QDeclarativePropertyPrivate::setBinding(d->vCenterBinding->property(), d->vCenterBinding);
    if (d->baselineBinding)
        QDeclarativePropertyPrivate::setBinding(d->baselineBinding->property(), d->baselineBinding);
}

bool QSGAnchorChanges::isReversable()
{
    return true;
}

void QSGAnchorChanges::reverse(Reason reason)
{
    Q_D(QSGAnchorChanges);
    if (!d->target)
        return;

    QSGItemPrivate *targetPrivate = QSGItemPrivate::get(d->target);
    //reset any anchors set by the state
    if (d->leftBinding) {
        targetPrivate->anchors()->resetLeft();
        QDeclarativePropertyPrivate::setBinding(d->leftBinding->property(), 0);
        if (reason == ActualChange) {
            d->leftBinding->destroy(); d->leftBinding = 0;
        }
    }
    if (d->rightBinding) {
        targetPrivate->anchors()->resetRight();
        QDeclarativePropertyPrivate::setBinding(d->rightBinding->property(), 0);
        if (reason == ActualChange) {
            d->rightBinding->destroy(); d->rightBinding = 0;
        }
    }
    if (d->hCenterBinding) {
        targetPrivate->anchors()->resetHorizontalCenter();
        QDeclarativePropertyPrivate::setBinding(d->hCenterBinding->property(), 0);
        if (reason == ActualChange) {
            d->hCenterBinding->destroy(); d->hCenterBinding = 0;
        }
    }
    if (d->topBinding) {
        targetPrivate->anchors()->resetTop();
        QDeclarativePropertyPrivate::setBinding(d->topBinding->property(), 0);
        if (reason == ActualChange) {
            d->topBinding->destroy(); d->topBinding = 0;
        }
    }
    if (d->bottomBinding) {
        targetPrivate->anchors()->resetBottom();
        QDeclarativePropertyPrivate::setBinding(d->bottomBinding->property(), 0);
        if (reason == ActualChange) {
            d->bottomBinding->destroy(); d->bottomBinding = 0;
        }
    }
    if (d->vCenterBinding) {
        targetPrivate->anchors()->resetVerticalCenter();
        QDeclarativePropertyPrivate::setBinding(d->vCenterBinding->property(), 0);
        if (reason == ActualChange) {
            d->vCenterBinding->destroy(); d->vCenterBinding = 0;
        }
    }
    if (d->baselineBinding) {
        targetPrivate->anchors()->resetBaseline();
        QDeclarativePropertyPrivate::setBinding(d->baselineBinding->property(), 0);
        if (reason == ActualChange) {
            d->baselineBinding->destroy(); d->baselineBinding = 0;
        }
    }

    //restore previous anchors
    if (d->origLeftBinding)
        QDeclarativePropertyPrivate::setBinding(d->leftProp, d->origLeftBinding);
    if (d->origRightBinding)
        QDeclarativePropertyPrivate::setBinding(d->rightProp, d->origRightBinding);
    if (d->origHCenterBinding)
        QDeclarativePropertyPrivate::setBinding(d->hCenterProp, d->origHCenterBinding);
    if (d->origTopBinding)
        QDeclarativePropertyPrivate::setBinding(d->topProp, d->origTopBinding);
    if (d->origBottomBinding)
        QDeclarativePropertyPrivate::setBinding(d->bottomProp, d->origBottomBinding);
    if (d->origVCenterBinding)
        QDeclarativePropertyPrivate::setBinding(d->vCenterProp, d->origVCenterBinding);
    if (d->origBaselineBinding)
        QDeclarativePropertyPrivate::setBinding(d->baselineProp, d->origBaselineBinding);

    //restore any absolute geometry changed by the state's anchors
    QSGAnchors::Anchors stateVAnchors = d->anchorSet->d_func()->usedAnchors & QSGAnchors::Vertical_Mask;
    QSGAnchors::Anchors origVAnchors = targetPrivate->anchors()->usedAnchors() & QSGAnchors::Vertical_Mask;
    QSGAnchors::Anchors stateHAnchors = d->anchorSet->d_func()->usedAnchors & QSGAnchors::Horizontal_Mask;
    QSGAnchors::Anchors origHAnchors = targetPrivate->anchors()->usedAnchors() & QSGAnchors::Horizontal_Mask;

    bool stateSetWidth = (stateHAnchors &&
                          stateHAnchors != QSGAnchors::LeftAnchor &&
                          stateHAnchors != QSGAnchors::RightAnchor &&
                          stateHAnchors != QSGAnchors::HCenterAnchor);
    bool origSetWidth = (origHAnchors &&
                         origHAnchors != QSGAnchors::LeftAnchor &&
                         origHAnchors != QSGAnchors::RightAnchor &&
                         origHAnchors != QSGAnchors::HCenterAnchor);
    if (d->origWidth.isValid() && stateSetWidth && !origSetWidth)
        d->target->setWidth(d->origWidth.value);

    bool stateSetHeight = (stateVAnchors &&
                           stateVAnchors != QSGAnchors::TopAnchor &&
                           stateVAnchors != QSGAnchors::BottomAnchor &&
                           stateVAnchors != QSGAnchors::VCenterAnchor &&
                           stateVAnchors != QSGAnchors::BaselineAnchor);
    bool origSetHeight = (origVAnchors &&
                          origVAnchors != QSGAnchors::TopAnchor &&
                          origVAnchors != QSGAnchors::BottomAnchor &&
                          origVAnchors != QSGAnchors::VCenterAnchor &&
                          origVAnchors != QSGAnchors::BaselineAnchor);
    if (d->origHeight.isValid() && stateSetHeight && !origSetHeight)
        d->target->setHeight(d->origHeight.value);

    if (stateHAnchors && !origHAnchors)
        d->target->setX(d->origX);

    if (stateVAnchors && !origVAnchors)
        d->target->setY(d->origY);
}

QString QSGAnchorChanges::typeName() const
{
    return QLatin1String("AnchorChanges");
}

QList<QDeclarativeAction> QSGAnchorChanges::additionalActions()
{
    Q_D(QSGAnchorChanges);
    QList<QDeclarativeAction> extra;

    QSGAnchors::Anchors combined = d->anchorSet->d_func()->usedAnchors | d->anchorSet->d_func()->resetAnchors;
    bool hChange = combined & QSGAnchors::Horizontal_Mask;
    bool vChange = combined & QSGAnchors::Vertical_Mask;

    if (d->target) {
        QDeclarativeAction a;
        if (hChange && d->fromX != d->toX) {
            a.property = QDeclarativeProperty(d->target, QLatin1String("x"));
            a.toValue = d->toX;
            extra << a;
        }
        if (vChange && d->fromY != d->toY) {
            a.property = QDeclarativeProperty(d->target, QLatin1String("y"));
            a.toValue = d->toY;
            extra << a;
        }
        if (hChange && d->fromWidth != d->toWidth) {
            a.property = QDeclarativeProperty(d->target, QLatin1String("width"));
            a.toValue = d->toWidth;
            extra << a;
        }
        if (vChange && d->fromHeight != d->toHeight) {
            a.property = QDeclarativeProperty(d->target, QLatin1String("height"));
            a.toValue = d->toHeight;
            extra << a;
        }
    }

    return extra;
}

bool QSGAnchorChanges::changesBindings()
{
    return true;
}

void QSGAnchorChanges::saveOriginals()
{
    Q_D(QSGAnchorChanges);
    if (!d->target)
        return;

    d->origLeftBinding = QDeclarativePropertyPrivate::binding(d->leftProp);
    d->origRightBinding = QDeclarativePropertyPrivate::binding(d->rightProp);
    d->origHCenterBinding = QDeclarativePropertyPrivate::binding(d->hCenterProp);
    d->origTopBinding = QDeclarativePropertyPrivate::binding(d->topProp);
    d->origBottomBinding = QDeclarativePropertyPrivate::binding(d->bottomProp);
    d->origVCenterBinding = QDeclarativePropertyPrivate::binding(d->vCenterProp);
    d->origBaselineBinding = QDeclarativePropertyPrivate::binding(d->baselineProp);

    QSGItemPrivate *targetPrivate = QSGItemPrivate::get(d->target);
    if (targetPrivate->widthValid)
        d->origWidth = d->target->width();
    if (targetPrivate->heightValid)
        d->origHeight = d->target->height();
    d->origX = d->target->x();
    d->origY = d->target->y();

    d->applyOrigLeft = d->applyOrigRight = d->applyOrigHCenter = d->applyOrigTop
      = d->applyOrigBottom = d->applyOrigVCenter = d->applyOrigBaseline = false;

    saveCurrentValues();
}

void QSGAnchorChanges::copyOriginals(QDeclarativeActionEvent *other)
{
    Q_D(QSGAnchorChanges);
    QSGAnchorChanges *ac = static_cast<QSGAnchorChanges*>(other);
    QSGAnchorChangesPrivate *acp = ac->d_func();

    QSGAnchors::Anchors combined = acp->anchorSet->d_func()->usedAnchors |
                                            acp->anchorSet->d_func()->resetAnchors;

    //probably also need to revert some things
    d->applyOrigLeft = (combined & QSGAnchors::LeftAnchor);
    d->applyOrigRight = (combined & QSGAnchors::RightAnchor);
    d->applyOrigHCenter = (combined & QSGAnchors::HCenterAnchor);
    d->applyOrigTop = (combined & QSGAnchors::TopAnchor);
    d->applyOrigBottom = (combined & QSGAnchors::BottomAnchor);
    d->applyOrigVCenter = (combined & QSGAnchors::VCenterAnchor);
    d->applyOrigBaseline = (combined & QSGAnchors::BaselineAnchor);

    d->origLeftBinding = acp->origLeftBinding;
    d->origRightBinding = acp->origRightBinding;
    d->origHCenterBinding = acp->origHCenterBinding;
    d->origTopBinding = acp->origTopBinding;
    d->origBottomBinding = acp->origBottomBinding;
    d->origVCenterBinding = acp->origVCenterBinding;
    d->origBaselineBinding = acp->origBaselineBinding;

    d->origWidth = acp->origWidth;
    d->origHeight = acp->origHeight;
    d->origX = acp->origX;
    d->origY = acp->origY;

    d->oldBindings.clear();
    d->oldBindings << acp->leftBinding << acp->rightBinding << acp->hCenterBinding
                << acp->topBinding << acp->bottomBinding << acp->baselineBinding;

    saveCurrentValues();
}

void QSGAnchorChanges::clearBindings()
{
    Q_D(QSGAnchorChanges);
    if (!d->target)
        return;

    //### should this (saving "from" values) be moved to saveCurrentValues()?
    d->fromX = d->target->x();
    d->fromY = d->target->y();
    d->fromWidth = d->target->width();
    d->fromHeight = d->target->height();

    QSGItemPrivate *targetPrivate = QSGItemPrivate::get(d->target);
    //reset any anchors with corresponding reverts
    //reset any anchors that have been specified as "undefined"
    //reset any anchors that we'll be setting in the state
    QSGAnchors::Anchors combined = d->anchorSet->d_func()->resetAnchors |
                                            d->anchorSet->d_func()->usedAnchors;
    if (d->applyOrigLeft || (combined & QSGAnchors::LeftAnchor)) {
        targetPrivate->anchors()->resetLeft();
        QDeclarativePropertyPrivate::setBinding(d->leftProp, 0);
    }
    if (d->applyOrigRight || (combined & QSGAnchors::RightAnchor)) {
        targetPrivate->anchors()->resetRight();
        QDeclarativePropertyPrivate::setBinding(d->rightProp, 0);
    }
    if (d->applyOrigHCenter || (combined & QSGAnchors::HCenterAnchor)) {
        targetPrivate->anchors()->resetHorizontalCenter();
        QDeclarativePropertyPrivate::setBinding(d->hCenterProp, 0);
    }
    if (d->applyOrigTop || (combined & QSGAnchors::TopAnchor)) {
        targetPrivate->anchors()->resetTop();
        QDeclarativePropertyPrivate::setBinding(d->topProp, 0);
    }
    if (d->applyOrigBottom || (combined & QSGAnchors::BottomAnchor)) {
        targetPrivate->anchors()->resetBottom();
        QDeclarativePropertyPrivate::setBinding(d->bottomProp, 0);
    }
    if (d->applyOrigVCenter || (combined & QSGAnchors::VCenterAnchor)) {
        targetPrivate->anchors()->resetVerticalCenter();
        QDeclarativePropertyPrivate::setBinding(d->vCenterProp, 0);
    }
    if (d->applyOrigBaseline || (combined & QSGAnchors::BaselineAnchor)) {
        targetPrivate->anchors()->resetBaseline();
        QDeclarativePropertyPrivate::setBinding(d->baselineProp, 0);
    }
}

bool QSGAnchorChanges::override(QDeclarativeActionEvent*other)
{
    if (other->typeName() != QLatin1String("AnchorChanges"))
        return false;
    if (static_cast<QDeclarativeActionEvent*>(this) == other)
        return true;
    if (static_cast<QSGAnchorChanges*>(other)->object() == object())
        return true;
    return false;
}

void QSGAnchorChanges::rewind()
{
    Q_D(QSGAnchorChanges);
    if (!d->target)
        return;

    QSGItemPrivate *targetPrivate = QSGItemPrivate::get(d->target);

    //restore previous values (but not previous bindings, i.e. anchors)
    d->target->setX(d->rewindX);
    d->target->setY(d->rewindY);
    if (targetPrivate->widthValid) {
        d->target->setWidth(d->rewindWidth);
    }
    if (targetPrivate->heightValid) {
        d->target->setHeight(d->rewindHeight);
    }
}

void QSGAnchorChanges::saveCurrentValues()
{
    Q_D(QSGAnchorChanges);
    if (!d->target)
        return;

    QSGItemPrivate *targetPrivate = QSGItemPrivate::get(d->target);
    d->rewindLeft = targetPrivate->anchors()->left();
    d->rewindRight = targetPrivate->anchors()->right();
    d->rewindHCenter = targetPrivate->anchors()->horizontalCenter();
    d->rewindTop = targetPrivate->anchors()->top();
    d->rewindBottom = targetPrivate->anchors()->bottom();
    d->rewindVCenter = targetPrivate->anchors()->verticalCenter();
    d->rewindBaseline = targetPrivate->anchors()->baseline();

    d->rewindX = d->target->x();
    d->rewindY = d->target->y();
    d->rewindWidth = d->target->width();
    d->rewindHeight = d->target->height();
}

void QSGAnchorChanges::saveTargetValues()
{
    Q_D(QSGAnchorChanges);
    if (!d->target)
        return;

    d->toX = d->target->x();
    d->toY = d->target->y();
    d->toWidth = d->target->width();
    d->toHeight = d->target->height();
}

#include <moc_qsgstateoperations_p.cpp>

QT_END_NAMESPACE

