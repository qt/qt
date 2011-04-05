// Commit: 2c7cab4172f1acc86fd49345a2847417e162f2c3
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsganchors_p_p.h"

#include "qsgitem.h"
#include "qsgitem_p.h"

#include <qdeclarativeinfo.h>

QT_BEGIN_NAMESPACE

//TODO: should we cache relationships, so we don't have to check each time (parent-child or sibling)?
//TODO: support non-parent, non-sibling (need to find lowest common ancestor)

static qreal hcenter(QSGItem *item)
{
    qreal width = item->width();
    int iw = width;
    if (iw % 2)
        return (width + 1) / 2;
    else
        return width / 2;
}

static qreal vcenter(QSGItem *item)
{
    qreal height = item->height();
    int ih = height;
    if (ih % 2)
        return (height + 1) / 2;
    else
        return height / 2;
}

//### const item?
//local position
static qreal position(QSGItem *item, QSGAnchorLine::AnchorLine anchorLine)
{
    qreal ret = 0.0;
    switch(anchorLine) {
    case QSGAnchorLine::Left:
        ret = item->x();
        break;
    case QSGAnchorLine::Right:
        ret = item->x() + item->width();
        break;
    case QSGAnchorLine::Top:
        ret = item->y();
        break;
    case QSGAnchorLine::Bottom:
        ret = item->y() + item->height();
        break;
    case QSGAnchorLine::HCenter:
        ret = item->x() + hcenter(item);
        break;
    case QSGAnchorLine::VCenter:
        ret = item->y() + vcenter(item);
        break;
    case QSGAnchorLine::Baseline:
        ret = item->y() + item->baselineOffset();
        break;
    default:
        break;
    }

    return ret;
}

//position when origin is 0,0
static qreal adjustedPosition(QSGItem *item, QSGAnchorLine::AnchorLine anchorLine)
{
    qreal ret = 0.0;
    switch(anchorLine) {
    case QSGAnchorLine::Left:
        ret = 0.0;
        break;
    case QSGAnchorLine::Right:
        ret = item->width();
        break;
    case QSGAnchorLine::Top:
        ret = 0.0;
        break;
    case QSGAnchorLine::Bottom:
        ret = item->height();
        break;
    case QSGAnchorLine::HCenter:
        ret = hcenter(item);
        break;
    case QSGAnchorLine::VCenter:
        ret = vcenter(item);
        break;
    case QSGAnchorLine::Baseline:
        ret = item->baselineOffset();
        break;
    default:
        break;
    }

    return ret;
}

QSGAnchors::QSGAnchors(QSGItem *item, QObject *parent)
: QObject(*new QSGAnchorsPrivate(item), parent)
{
}

QSGAnchors::~QSGAnchors()
{
    Q_D(QSGAnchors);
    d->remDepend(d->fill);
    d->remDepend(d->centerIn);
    d->remDepend(d->left.item);
    d->remDepend(d->right.item);
    d->remDepend(d->top.item);
    d->remDepend(d->bottom.item);
    d->remDepend(d->vCenter.item);
    d->remDepend(d->hCenter.item);
    d->remDepend(d->baseline.item);
}

void QSGAnchorsPrivate::fillChanged()
{
    Q_Q(QSGAnchors);
    if (!fill || !isItemComplete())
        return;

    if (updatingFill < 2) {
        ++updatingFill;

        qreal horizontalMargin = q->mirrored() ? rightMargin : leftMargin;

        if (fill == item->parentItem()) {                         //child-parent
            setItemPos(QPointF(horizontalMargin, topMargin));
        } else if (fill->parentItem() == item->parentItem()) {   //siblings
            setItemPos(QPointF(fill->x()+horizontalMargin, fill->y()+topMargin));
        }
        setItemSize(QSizeF(fill->width()-leftMargin-rightMargin, fill->height()-topMargin-bottomMargin));

        --updatingFill;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << QSGAnchors::tr("Possible anchor loop detected on fill.");
    }

}

void QSGAnchorsPrivate::centerInChanged()
{
    Q_Q(QSGAnchors);
    if (!centerIn || fill || !isItemComplete())
        return;

    if (updatingCenterIn < 2) {
        ++updatingCenterIn;

        qreal effectiveHCenterOffset = q->mirrored() ? -hCenterOffset : hCenterOffset;
        if (centerIn == item->parentItem()) {
            QPointF p(hcenter(item->parentItem()) - hcenter(item) + effectiveHCenterOffset,
                      vcenter(item->parentItem()) - vcenter(item) + vCenterOffset);
            setItemPos(p);

        } else if (centerIn->parentItem() == item->parentItem()) {
            QPointF p(centerIn->x() + hcenter(centerIn) - hcenter(item) + effectiveHCenterOffset,
                      centerIn->y() + vcenter(centerIn) - vcenter(item) + vCenterOffset);
            setItemPos(p);
        }

        --updatingCenterIn;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << QSGAnchors::tr("Possible anchor loop detected on centerIn.");
    }
}

void QSGAnchorsPrivate::clearItem(QSGItem *item)
{
    if (!item)
        return;
    if (fill == item)
        fill = 0;
    if (centerIn == item)
        centerIn = 0;
    if (left.item == item) {
        left.item = 0;
        usedAnchors &= ~QSGAnchors::LeftAnchor;
    }
    if (right.item == item) {
        right.item = 0;
        usedAnchors &= ~QSGAnchors::RightAnchor;
    }
    if (top.item == item) {
        top.item = 0;
        usedAnchors &= ~QSGAnchors::TopAnchor;
    }
    if (bottom.item == item) {
        bottom.item = 0;
        usedAnchors &= ~QSGAnchors::BottomAnchor;
    }
    if (vCenter.item == item) {
        vCenter.item = 0;
        usedAnchors &= ~QSGAnchors::VCenterAnchor;
    }
    if (hCenter.item == item) {
        hCenter.item = 0;
        usedAnchors &= ~QSGAnchors::HCenterAnchor;
    }
    if (baseline.item == item) {
        baseline.item = 0;
        usedAnchors &= ~QSGAnchors::BaselineAnchor;
    }
}

void QSGAnchorsPrivate::addDepend(QSGItem *item)
{
    if (!item)
        return;
    
    QSGItemPrivate *p = QSGItemPrivate::get(item);
    p->addItemChangeListener(this, QSGItemPrivate::Geometry);
}

void QSGAnchorsPrivate::remDepend(QSGItem *item)
{
    if (!item)
        return;

    QSGItemPrivate *p = QSGItemPrivate::get(item);
    p->removeItemChangeListener(this, QSGItemPrivate::Geometry);
}

bool QSGAnchors::mirrored()
{
    Q_D(QSGAnchors);
    return QSGItemPrivate::get(d->item)->effectiveLayoutMirror;
}

bool QSGAnchorsPrivate::isItemComplete() const
{
    return componentComplete;
}

void QSGAnchors::classBegin()
{
    Q_D(QSGAnchors);
    d->componentComplete = false;
}

void QSGAnchors::componentComplete()
{
    Q_D(QSGAnchors);
    d->componentComplete = true;
}

void QSGAnchorsPrivate::setItemHeight(qreal v)
{
    updatingMe = true;
    item->setHeight(v);
    updatingMe = false;
}

void QSGAnchorsPrivate::setItemWidth(qreal v)
{
    updatingMe = true;
    item->setWidth(v);
    updatingMe = false;
}

void QSGAnchorsPrivate::setItemX(qreal v)
{
    updatingMe = true;
    item->setX(v);
    updatingMe = false;
}

void QSGAnchorsPrivate::setItemY(qreal v)
{
    updatingMe = true;
    item->setY(v);
    updatingMe = false;
}

void QSGAnchorsPrivate::setItemPos(const QPointF &v)
{
    updatingMe = true;
    item->setPos(v);
    updatingMe = false;
}

void QSGAnchorsPrivate::setItemSize(const QSizeF &v)
{
    updatingMe = true;
    item->setSize(v);
    updatingMe = false;
}

void QSGAnchorsPrivate::updateMe()
{
    if (updatingMe) {
        updatingMe = false;
        return;
    }

    fillChanged();
    centerInChanged();
    updateHorizontalAnchors();
    updateVerticalAnchors();
}

void QSGAnchorsPrivate::updateOnComplete()
{
    fillChanged();
    centerInChanged();
    updateHorizontalAnchors();
    updateVerticalAnchors();
}

void QSGAnchorsPrivate::itemGeometryChanged(QSGItem *, const QRectF &newG, const QRectF &oldG)
{
    fillChanged();
    centerInChanged();
    if (newG.x() != oldG.x() || newG.width() != oldG.width())
        updateHorizontalAnchors();
    if (newG.y() != oldG.y() || newG.height() != oldG.height())
        updateVerticalAnchors();
}

QSGItem *QSGAnchors::fill() const
{
    Q_D(const QSGAnchors);
    return d->fill;
}

void QSGAnchors::setFill(QSGItem *f)
{
    Q_D(QSGAnchors);
    if (d->fill == f)
        return;

    if (!f) {
        d->remDepend(d->fill);
        d->fill = f;
        emit fillChanged();
        return;
    }
    if (f != d->item->parentItem() && f->parentItem() != d->item->parentItem()){
        qmlInfo(d->item) << tr("Cannot anchor to an item that isn't a parent or sibling.");
        return;
    }
    d->remDepend(d->fill);
    d->fill = f;
    d->addDepend(d->fill);
    emit fillChanged();
    d->fillChanged();
}

void QSGAnchors::resetFill()
{
    setFill(0);
}

QSGItem *QSGAnchors::centerIn() const
{
    Q_D(const QSGAnchors);
    return d->centerIn;
}

void QSGAnchors::setCenterIn(QSGItem* c)
{
    Q_D(QSGAnchors);
    if (d->centerIn == c)
        return;

    if (!c) {
        d->remDepend(d->centerIn);
        d->centerIn = c;
        emit centerInChanged();
        return;
    }
    if (c != d->item->parentItem() && c->parentItem() != d->item->parentItem()){
        qmlInfo(d->item) << tr("Cannot anchor to an item that isn't a parent or sibling.");
        return;
    }

    d->remDepend(d->centerIn);
    d->centerIn = c;
    d->addDepend(d->centerIn);
    emit centerInChanged();
    d->centerInChanged();
}

void QSGAnchors::resetCenterIn()
{
    setCenterIn(0);
}

bool QSGAnchorsPrivate::calcStretch(const QSGAnchorLine &edge1,
                                    const QSGAnchorLine &edge2,
                                    qreal offset1,
                                    qreal offset2,
                                    QSGAnchorLine::AnchorLine line,
                                    qreal &stretch)
{
    bool edge1IsParent = (edge1.item == item->parentItem());
    bool edge2IsParent = (edge2.item == item->parentItem());
    bool edge1IsSibling = (edge1.item->parentItem() == item->parentItem());
    bool edge2IsSibling = (edge2.item->parentItem() == item->parentItem());

    bool invalid = false;
    if ((edge2IsParent && edge1IsParent) || (edge2IsSibling && edge1IsSibling)) {
        stretch = (position(edge2.item, edge2.anchorLine) + offset2)
                    - (position(edge1.item, edge1.anchorLine) + offset1);
    } else if (edge2IsParent && edge1IsSibling) {
        stretch = (position(edge2.item, edge2.anchorLine) + offset2)
                    - (position(item->parentItem(), line)
                    + position(edge1.item, edge1.anchorLine) + offset1);
    } else if (edge2IsSibling && edge1IsParent) {
        stretch = (position(item->parentItem(), line) + position(edge2.item, edge2.anchorLine) + offset2)
                    - (position(edge1.item, edge1.anchorLine) + offset1);
    } else
        invalid = true;

    return invalid;
}

void QSGAnchorsPrivate::updateVerticalAnchors()
{
    if (fill || centerIn || !isItemComplete())
        return;

    if (updatingVerticalAnchor < 2) {
        ++updatingVerticalAnchor;
        if (usedAnchors & QSGAnchors::TopAnchor) {
            //Handle stretching
            bool invalid = true;
            qreal height = 0.0;
            if (usedAnchors & QSGAnchors::BottomAnchor) {
                invalid = calcStretch(top, bottom, topMargin, -bottomMargin, QSGAnchorLine::Top, height);
            } else if (usedAnchors & QSGAnchors::VCenterAnchor) {
                invalid = calcStretch(top, vCenter, topMargin, vCenterOffset, QSGAnchorLine::Top, height);
                height *= 2;
            }
            if (!invalid)
                setItemHeight(height);

            //Handle top
            if (top.item == item->parentItem()) {
                setItemY(adjustedPosition(top.item, top.anchorLine) + topMargin);
            } else if (top.item->parentItem() == item->parentItem()) {
                setItemY(position(top.item, top.anchorLine) + topMargin);
            }
        } else if (usedAnchors & QSGAnchors::BottomAnchor) {
            //Handle stretching (top + bottom case is handled above)
            if (usedAnchors & QSGAnchors::VCenterAnchor) {
                qreal height = 0.0;
                bool invalid = calcStretch(vCenter, bottom, vCenterOffset, -bottomMargin,
                                              QSGAnchorLine::Top, height);
                if (!invalid)
                    setItemHeight(height*2);
            }

            //Handle bottom
            if (bottom.item == item->parentItem()) {
                setItemY(adjustedPosition(bottom.item, bottom.anchorLine) - item->height() - bottomMargin);
            } else if (bottom.item->parentItem() == item->parentItem()) {
                setItemY(position(bottom.item, bottom.anchorLine) - item->height() - bottomMargin);
            }
        } else if (usedAnchors & QSGAnchors::VCenterAnchor) {
            //(stetching handled above)

            //Handle vCenter
            if (vCenter.item == item->parentItem()) {
                setItemY(adjustedPosition(vCenter.item, vCenter.anchorLine)
                              - vcenter(item) + vCenterOffset);
            } else if (vCenter.item->parentItem() == item->parentItem()) {
                setItemY(position(vCenter.item, vCenter.anchorLine) - vcenter(item) + vCenterOffset);
            }
        } else if (usedAnchors & QSGAnchors::BaselineAnchor) {
            //Handle baseline
            if (baseline.item == item->parentItem()) {
                setItemY(adjustedPosition(baseline.item, baseline.anchorLine) - item->baselineOffset() + baselineOffset);
            } else if (baseline.item->parentItem() == item->parentItem()) {
                setItemY(position(baseline.item, baseline.anchorLine) - item->baselineOffset() + baselineOffset);
            }
        }
        --updatingVerticalAnchor;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << QSGAnchors::tr("Possible anchor loop detected on vertical anchor.");
    }
}

inline QSGAnchorLine::AnchorLine reverseAnchorLine(QSGAnchorLine::AnchorLine anchorLine)
{
    if (anchorLine == QSGAnchorLine::Left) {
        return QSGAnchorLine::Right;
    } else if (anchorLine == QSGAnchorLine::Right) {
        return QSGAnchorLine::Left;
    } else {
        return anchorLine;
    }
}

void QSGAnchorsPrivate::updateHorizontalAnchors()
{
    Q_Q(QSGAnchors);
    if (fill || centerIn || !isItemComplete())
        return;

    if (updatingHorizontalAnchor < 3) {
        ++updatingHorizontalAnchor;
        qreal effectiveRightMargin, effectiveLeftMargin, effectiveHorizontalCenterOffset;
        QSGAnchorLine effectiveLeft, effectiveRight, effectiveHorizontalCenter;
        QSGAnchors::Anchor effectiveLeftAnchor, effectiveRightAnchor;
        if (q->mirrored()) {
            effectiveLeftAnchor = QSGAnchors::RightAnchor;
            effectiveRightAnchor = QSGAnchors::LeftAnchor;
            effectiveLeft.item = right.item;
            effectiveLeft.anchorLine = reverseAnchorLine(right.anchorLine);
            effectiveRight.item = left.item;
            effectiveRight.anchorLine = reverseAnchorLine(left.anchorLine);
            effectiveHorizontalCenter.item = hCenter.item;
            effectiveHorizontalCenter.anchorLine = reverseAnchorLine(hCenter.anchorLine);
            effectiveLeftMargin = rightMargin;
            effectiveRightMargin = leftMargin;
            effectiveHorizontalCenterOffset = -hCenterOffset;
        } else {
            effectiveLeftAnchor = QSGAnchors::LeftAnchor;
            effectiveRightAnchor = QSGAnchors::RightAnchor;
            effectiveLeft = left;
            effectiveRight = right;
            effectiveHorizontalCenter = hCenter;
            effectiveLeftMargin = leftMargin;
            effectiveRightMargin = rightMargin;
            effectiveHorizontalCenterOffset = hCenterOffset;
        }

        if (usedAnchors & effectiveLeftAnchor) {
            //Handle stretching
            bool invalid = true;
            qreal width = 0.0;
            if (usedAnchors & effectiveRightAnchor) {
                invalid = calcStretch(effectiveLeft, effectiveRight, effectiveLeftMargin, -effectiveRightMargin, QSGAnchorLine::Left, width);
            } else if (usedAnchors & QSGAnchors::HCenterAnchor) {
                invalid = calcStretch(effectiveLeft, effectiveHorizontalCenter, effectiveLeftMargin, effectiveHorizontalCenterOffset, QSGAnchorLine::Left, width);
                width *= 2;
            }
            if (!invalid)
                setItemWidth(width);

            //Handle left
            if (effectiveLeft.item == item->parentItem()) {
                setItemX(adjustedPosition(effectiveLeft.item, effectiveLeft.anchorLine) + effectiveLeftMargin);
            } else if (effectiveLeft.item->parentItem() == item->parentItem()) {
                setItemX(position(effectiveLeft.item, effectiveLeft.anchorLine) + effectiveLeftMargin);
            }
        } else if (usedAnchors & effectiveRightAnchor) {
            //Handle stretching (left + right case is handled in updateLeftAnchor)
            if (usedAnchors & QSGAnchors::HCenterAnchor) {
                qreal width = 0.0;
                bool invalid = calcStretch(effectiveHorizontalCenter, effectiveRight, effectiveHorizontalCenterOffset, -effectiveRightMargin,
                                              QSGAnchorLine::Left, width);
                if (!invalid)
                    setItemWidth(width*2);
            }

            //Handle right
            if (effectiveRight.item == item->parentItem()) {
                setItemX(adjustedPosition(effectiveRight.item, effectiveRight.anchorLine) - item->width() - effectiveRightMargin);
            } else if (effectiveRight.item->parentItem() == item->parentItem()) {
                setItemX(position(effectiveRight.item, effectiveRight.anchorLine) - item->width() - effectiveRightMargin);
            }
        } else if (usedAnchors & QSGAnchors::HCenterAnchor) {
            //Handle hCenter
            if (effectiveHorizontalCenter.item == item->parentItem()) {
                setItemX(adjustedPosition(effectiveHorizontalCenter.item, effectiveHorizontalCenter.anchorLine) - hcenter(item) + effectiveHorizontalCenterOffset);
            } else if (effectiveHorizontalCenter.item->parentItem() == item->parentItem()) {
                setItemX(position(effectiveHorizontalCenter.item, effectiveHorizontalCenter.anchorLine) - hcenter(item) + effectiveHorizontalCenterOffset);
            }
        }
        --updatingHorizontalAnchor;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << QSGAnchors::tr("Possible anchor loop detected on horizontal anchor.");
    }
}

QSGAnchorLine QSGAnchors::top() const
{
    Q_D(const QSGAnchors);
    return d->top;
}

void QSGAnchors::setTop(const QSGAnchorLine &edge)
{
    Q_D(QSGAnchors);
    if (!d->checkVAnchorValid(edge) || d->top == edge)
        return;

    d->usedAnchors |= TopAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~TopAnchor;
        return;
    }

    d->remDepend(d->top.item);
    d->top = edge;
    d->addDepend(d->top.item);
    emit topChanged();
    d->updateVerticalAnchors();
}

void QSGAnchors::resetTop()
{
    Q_D(QSGAnchors);
    d->usedAnchors &= ~TopAnchor;
    d->remDepend(d->top.item);
    d->top = QSGAnchorLine();
    emit topChanged();
    d->updateVerticalAnchors();
}

QSGAnchorLine QSGAnchors::bottom() const
{
    Q_D(const QSGAnchors);
    return d->bottom;
}

void QSGAnchors::setBottom(const QSGAnchorLine &edge)
{
    Q_D(QSGAnchors);
    if (!d->checkVAnchorValid(edge) || d->bottom == edge)
        return;

    d->usedAnchors |= BottomAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~BottomAnchor;
        return;
    }

    d->remDepend(d->bottom.item);
    d->bottom = edge;
    d->addDepend(d->bottom.item);
    emit bottomChanged();
    d->updateVerticalAnchors();
}

void QSGAnchors::resetBottom()
{
    Q_D(QSGAnchors);
    d->usedAnchors &= ~BottomAnchor;
    d->remDepend(d->bottom.item);
    d->bottom = QSGAnchorLine();
    emit bottomChanged();
    d->updateVerticalAnchors();
}

QSGAnchorLine QSGAnchors::verticalCenter() const
{
    Q_D(const QSGAnchors);
    return d->vCenter;
}

void QSGAnchors::setVerticalCenter(const QSGAnchorLine &edge)
{
    Q_D(QSGAnchors);
    if (!d->checkVAnchorValid(edge) || d->vCenter == edge)
        return;

    d->usedAnchors |= VCenterAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~VCenterAnchor;
        return;
    }

    d->remDepend(d->vCenter.item);
    d->vCenter = edge;
    d->addDepend(d->vCenter.item);
    emit verticalCenterChanged();
    d->updateVerticalAnchors();
}

void QSGAnchors::resetVerticalCenter()
{
    Q_D(QSGAnchors);
    d->usedAnchors &= ~VCenterAnchor;
    d->remDepend(d->vCenter.item);
    d->vCenter = QSGAnchorLine();
    emit verticalCenterChanged();
    d->updateVerticalAnchors();
}

QSGAnchorLine QSGAnchors::baseline() const
{
    Q_D(const QSGAnchors);
    return d->baseline;
}

void QSGAnchors::setBaseline(const QSGAnchorLine &edge)
{
    Q_D(QSGAnchors);
    if (!d->checkVAnchorValid(edge) || d->baseline == edge)
        return;

    d->usedAnchors |= BaselineAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~BaselineAnchor;
        return;
    }

    d->remDepend(d->baseline.item);
    d->baseline = edge;
    d->addDepend(d->baseline.item);
    emit baselineChanged();
    d->updateVerticalAnchors();
}

void QSGAnchors::resetBaseline()
{
    Q_D(QSGAnchors);
    d->usedAnchors &= ~BaselineAnchor;
    d->remDepend(d->baseline.item);
    d->baseline = QSGAnchorLine();
    emit baselineChanged();
    d->updateVerticalAnchors();
}

QSGAnchorLine QSGAnchors::left() const
{
    Q_D(const QSGAnchors);
    return d->left;
}

void QSGAnchors::setLeft(const QSGAnchorLine &edge)
{
    Q_D(QSGAnchors);
    if (!d->checkHAnchorValid(edge) || d->left == edge)
        return;

    d->usedAnchors |= LeftAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~LeftAnchor;
        return;
    }

    d->remDepend(d->left.item);
    d->left = edge;
    d->addDepend(d->left.item);
    emit leftChanged();
    d->updateHorizontalAnchors();
}

void QSGAnchors::resetLeft()
{
    Q_D(QSGAnchors);
    d->usedAnchors &= ~LeftAnchor;
    d->remDepend(d->left.item);
    d->left = QSGAnchorLine();
    emit leftChanged();
    d->updateHorizontalAnchors();
}

QSGAnchorLine QSGAnchors::right() const
{
    Q_D(const QSGAnchors);
    return d->right;
}

void QSGAnchors::setRight(const QSGAnchorLine &edge)
{
    Q_D(QSGAnchors);
    if (!d->checkHAnchorValid(edge) || d->right == edge)
        return;

    d->usedAnchors |= RightAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~RightAnchor;
        return;
    }

    d->remDepend(d->right.item);
    d->right = edge;
    d->addDepend(d->right.item);
    emit rightChanged();
    d->updateHorizontalAnchors();
}

void QSGAnchors::resetRight()
{
    Q_D(QSGAnchors);
    d->usedAnchors &= ~RightAnchor;
    d->remDepend(d->right.item);
    d->right = QSGAnchorLine();
    emit rightChanged();
    d->updateHorizontalAnchors();
}

QSGAnchorLine QSGAnchors::horizontalCenter() const
{
    Q_D(const QSGAnchors);
    return d->hCenter;
}

void QSGAnchors::setHorizontalCenter(const QSGAnchorLine &edge)
{
    Q_D(QSGAnchors);
    if (!d->checkHAnchorValid(edge) || d->hCenter == edge)
        return;

    d->usedAnchors |= HCenterAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~HCenterAnchor;
        return;
    }

    d->remDepend(d->hCenter.item);
    d->hCenter = edge;
    d->addDepend(d->hCenter.item);
    emit horizontalCenterChanged();
    d->updateHorizontalAnchors();
}

void QSGAnchors::resetHorizontalCenter()
{
    Q_D(QSGAnchors);
    d->usedAnchors &= ~HCenterAnchor;
    d->remDepend(d->hCenter.item);
    d->hCenter = QSGAnchorLine();
    emit horizontalCenterChanged();
    d->updateHorizontalAnchors();
}

qreal QSGAnchors::leftMargin() const
{
    Q_D(const QSGAnchors);
    return d->leftMargin;
}

void QSGAnchors::setLeftMargin(qreal offset)
{
    Q_D(QSGAnchors);
    if (d->leftMargin == offset)
        return;
    d->leftMargin = offset;
    if(d->fill)
        d->fillChanged();
    else
        d->updateHorizontalAnchors();
    emit leftMarginChanged();
}

qreal QSGAnchors::rightMargin() const
{
    Q_D(const QSGAnchors);
    return d->rightMargin;
}

void QSGAnchors::setRightMargin(qreal offset)
{
    Q_D(QSGAnchors);
    if (d->rightMargin == offset)
        return;
    d->rightMargin = offset;
    if(d->fill)
        d->fillChanged();
    else
        d->updateHorizontalAnchors();
    emit rightMarginChanged();
}

qreal QSGAnchors::margins() const
{
    Q_D(const QSGAnchors);
    return d->margins;
}

void QSGAnchors::setMargins(qreal offset)
{
    Q_D(QSGAnchors);
    if (d->margins == offset)
        return;
    //###Is it significantly faster to set them directly so we can call fillChanged only once?
    if(!d->rightMargin || d->rightMargin == d->margins)
        setRightMargin(offset);
    if(!d->leftMargin || d->leftMargin == d->margins)
        setLeftMargin(offset);
    if(!d->topMargin || d->topMargin == d->margins)
        setTopMargin(offset);
    if(!d->bottomMargin || d->bottomMargin == d->margins)
        setBottomMargin(offset);
    d->margins = offset;
    emit marginsChanged();

}

qreal QSGAnchors::horizontalCenterOffset() const
{
    Q_D(const QSGAnchors);
    return d->hCenterOffset;
}

void QSGAnchors::setHorizontalCenterOffset(qreal offset)
{
    Q_D(QSGAnchors);
    if (d->hCenterOffset == offset)
        return;
    d->hCenterOffset = offset;
    if(d->centerIn)
        d->centerInChanged();
    else
        d->updateHorizontalAnchors();
    emit horizontalCenterOffsetChanged();
}

qreal QSGAnchors::topMargin() const
{
    Q_D(const QSGAnchors);
    return d->topMargin;
}

void QSGAnchors::setTopMargin(qreal offset)
{
    Q_D(QSGAnchors);
    if (d->topMargin == offset)
        return;
    d->topMargin = offset;
    if(d->fill)
        d->fillChanged();
    else
        d->updateVerticalAnchors();
    emit topMarginChanged();
}

qreal QSGAnchors::bottomMargin() const
{
    Q_D(const QSGAnchors);
    return d->bottomMargin;
}

void QSGAnchors::setBottomMargin(qreal offset)
{
    Q_D(QSGAnchors);
    if (d->bottomMargin == offset)
        return;
    d->bottomMargin = offset;
    if(d->fill)
        d->fillChanged();
    else
        d->updateVerticalAnchors();
    emit bottomMarginChanged();
}

qreal QSGAnchors::verticalCenterOffset() const
{
    Q_D(const QSGAnchors);
    return d->vCenterOffset;
}

void QSGAnchors::setVerticalCenterOffset(qreal offset)
{
    Q_D(QSGAnchors);
    if (d->vCenterOffset == offset)
        return;
    d->vCenterOffset = offset;
    if(d->centerIn)
        d->centerInChanged();
    else
        d->updateVerticalAnchors();
    emit verticalCenterOffsetChanged();
}

qreal QSGAnchors::baselineOffset() const
{
    Q_D(const QSGAnchors);
    return d->baselineOffset;
}

void QSGAnchors::setBaselineOffset(qreal offset)
{
    Q_D(QSGAnchors);
    if (d->baselineOffset == offset)
        return;
    d->baselineOffset = offset;
    d->updateVerticalAnchors();
    emit baselineOffsetChanged();
}

QSGAnchors::Anchors QSGAnchors::usedAnchors() const
{
    Q_D(const QSGAnchors);
    return d->usedAnchors;
}

bool QSGAnchorsPrivate::checkHValid() const
{
    if (usedAnchors & QSGAnchors::LeftAnchor &&
        usedAnchors & QSGAnchors::RightAnchor &&
        usedAnchors & QSGAnchors::HCenterAnchor) {
        qmlInfo(item) << QSGAnchors::tr("Cannot specify left, right, and hcenter anchors.");
        return false;
    }

    return true;
}

bool QSGAnchorsPrivate::checkHAnchorValid(QSGAnchorLine anchor) const
{
    if (!anchor.item) {
        qmlInfo(item) << QSGAnchors::tr("Cannot anchor to a null item.");
        return false;
    } else if (anchor.anchorLine & QSGAnchorLine::Vertical_Mask) {
        qmlInfo(item) << QSGAnchors::tr("Cannot anchor a horizontal edge to a vertical edge.");
        return false;
    } else if (anchor.item != item->parentItem() && anchor.item->parentItem() != item->parentItem()){
        qmlInfo(item) << QSGAnchors::tr("Cannot anchor to an item that isn't a parent or sibling.");
        return false;
    } else if (anchor.item == item) {
        qmlInfo(item) << QSGAnchors::tr("Cannot anchor item to self.");
        return false;
    }

    return true;
}

bool QSGAnchorsPrivate::checkVValid() const
{
    if (usedAnchors & QSGAnchors::TopAnchor &&
        usedAnchors & QSGAnchors::BottomAnchor &&
        usedAnchors & QSGAnchors::VCenterAnchor) {
        qmlInfo(item) << QSGAnchors::tr("Cannot specify top, bottom, and vcenter anchors.");
        return false;
    } else if (usedAnchors & QSGAnchors::BaselineAnchor &&
               (usedAnchors & QSGAnchors::TopAnchor ||
                usedAnchors & QSGAnchors::BottomAnchor ||
                usedAnchors & QSGAnchors::VCenterAnchor)) {
        qmlInfo(item) << QSGAnchors::tr("Baseline anchor cannot be used in conjunction with top, bottom, or vcenter anchors.");
        return false;
    }

    return true;
}

bool QSGAnchorsPrivate::checkVAnchorValid(QSGAnchorLine anchor) const
{
    if (!anchor.item) {
        qmlInfo(item) << QSGAnchors::tr("Cannot anchor to a null item.");
        return false;
    } else if (anchor.anchorLine & QSGAnchorLine::Horizontal_Mask) {
        qmlInfo(item) << QSGAnchors::tr("Cannot anchor a vertical edge to a horizontal edge.");
        return false;
    } else if (anchor.item != item->parentItem() && anchor.item->parentItem() != item->parentItem()){
        qmlInfo(item) << QSGAnchors::tr("Cannot anchor to an item that isn't a parent or sibling.");
        return false;
    } else if (anchor.item == item){
        qmlInfo(item) << QSGAnchors::tr("Cannot anchor item to self.");
        return false;
    }

    return true;
}

QT_END_NAMESPACE

#include <moc_qsganchors_p.cpp>

