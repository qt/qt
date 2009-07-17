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

#include "qfxanchors_p.h"
#include "qfxitem.h"
#include "qfxitem_p.h"
#include <QDebug>
#include <QtDeclarative/qmlinfo.h>
#include <QtDeclarative/qmlbindablevalue.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(QFxAnchors,Anchors)

//TODO: should we cache relationships, so we don't have to check each time (parent-child or sibling)?
//TODO: support non-parent, non-sibling (need to find lowest common ancestor)

//### const item?
//local position
static qreal position(QFxItem *item, QFxAnchorLine::AnchorLine anchorLine)
{
    qreal ret = 0.0;
    switch(anchorLine) {
    case QFxAnchorLine::Left:
        ret = item->x();
        break;
    case QFxAnchorLine::Right:
        ret = item->x() + item->width();
        break;
    case QFxAnchorLine::Top:
        ret = item->y();
        break;
    case QFxAnchorLine::Bottom:
        ret = item->y() + item->height();
        break;
    case QFxAnchorLine::HCenter:
        ret = item->x() + item->width()/2;
        break;
    case QFxAnchorLine::VCenter:
        ret = item->y() + item->height()/2;
        break;
    case QFxAnchorLine::Baseline:
        ret = item->y() + item->baselineOffset();
        break;
    default:
        break;
    }

    return ret;
}

//position when origin is 0,0
static qreal adjustedPosition(QFxItem *item, QFxAnchorLine::AnchorLine anchorLine)
{
    int ret = 0;
    switch(anchorLine) {
    case QFxAnchorLine::Left:
        ret = 0;
        break;
    case QFxAnchorLine::Right:
        ret = item->width();
        break;
    case QFxAnchorLine::Top:
        ret = 0;
        break;
    case QFxAnchorLine::Bottom:
        ret = item->height();
        break;
    case QFxAnchorLine::HCenter:
        ret = item->width()/2;
        break;
    case QFxAnchorLine::VCenter:
        ret = item->height()/2;
        break;
    case QFxAnchorLine::Baseline:
        ret = item->baselineOffset();
        break;
    default:
        break;
    }

    return ret;
}

/*!
    \internal
    \class QFxAnchors
    \ingroup group_layouts
    \brief The QFxAnchors class provides a way to lay out items relative to other items.

    \warning Currently, only anchoring to siblings or parent is supported.
*/

QFxAnchors::QFxAnchors(QObject *parent)
  : QObject(*new QFxAnchorsPrivate(), parent)
{
}

QFxAnchors::~QFxAnchors()
{
    Q_D(QFxAnchors);
    d->remDepend(d->fill);
    d->remDepend(d->centeredIn);
    d->remDepend(d->left.item);
    d->remDepend(d->right.item);
    d->remDepend(d->top.item);
    d->remDepend(d->bottom.item);
    d->remDepend(d->vCenter.item);
    d->remDepend(d->hCenter.item);
    d->remDepend(d->baseline.item);
}

void QFxAnchorsPrivate::fillChanged()
{
    if (!fill || !isItemComplete())
        return;

    if (fill == item->itemParent()) {                         //child-parent
        setItemPos(QPointF(leftMargin, topMargin));
    } else if (fill->itemParent() == item->itemParent()) {   //siblings
        setItemPos(QPointF(fill->x()+leftMargin, fill->y()+topMargin));
    }
    setItemWidth(fill->width()-leftMargin-rightMargin);
    setItemHeight(fill->height()-topMargin-bottomMargin);
}

void QFxAnchorsPrivate::centeredInChanged()
{
    if (!centeredIn || fill || !isItemComplete())
        return;

    if (centeredIn == item->itemParent()) {
        QPointF p((item->itemParent()->width() - item->width()) / 2.,
                  (item->itemParent()->height() - item->height()) / 2.);
        setItemPos(p);

    } else if (centeredIn->itemParent() == item->itemParent()) {

        QPointF p(centeredIn->x() + (centeredIn->width() - item->width()) / 2.,
                  centeredIn->y() + (centeredIn->height() - item->height()) / 2.);
        setItemPos(p);
    }
}

void QFxAnchorsPrivate::clearItem(QFxItem *item)
{
    if (fill == item) 
        fill = 0;
    if (centeredIn == item)
        centeredIn = 0;
    if (left.item == item) {
        left.item = 0;
        usedAnchors &= ~QFxAnchors::HasLeftAnchor;
    }
    if (right.item == item) {
        right.item = 0;
        usedAnchors &= ~QFxAnchors::HasRightAnchor;
    }
    if (top.item == item) {
        top.item = 0;
        usedAnchors &= ~QFxAnchors::HasTopAnchor;
    }
    if (bottom.item == item) {
        bottom.item = 0;
        usedAnchors &= ~QFxAnchors::HasBottomAnchor;
    }
    if (vCenter.item == item) {
        vCenter.item = 0;
        usedAnchors &= ~QFxAnchors::HasVCenterAnchor;
    }
    if (hCenter.item == item) {
        hCenter.item = 0;
        usedAnchors &= ~QFxAnchors::HasHCenterAnchor;
    }
    if (baseline.item == item) {
        baseline.item = 0;
        usedAnchors &= ~QFxAnchors::HasBaselineAnchor;
    }
}

void QFxAnchorsPrivate::addDepend(QFxItem *item)
{
    Q_Q(QFxAnchors);
    if (!item)
        return;
    QFxItemPrivate *p = 
        static_cast<QFxItemPrivate *>(QGraphicsItemPrivate::get(item));
    p->dependantAnchors.append(q);
}

void QFxAnchorsPrivate::remDepend(QFxItem *item)
{
    Q_Q(QFxAnchors);
    if (!item)
        return;
    QFxItemPrivate *p = 
        static_cast<QFxItemPrivate *>(QGraphicsItemPrivate::get(item));
    p->dependantAnchors.removeAll(q);
}

bool QFxAnchorsPrivate::isItemComplete() const
{
    return item->isComponentComplete();
}

void QFxAnchorsPrivate::setItemHeight(qreal v)
{
    updatingMe = true;
    item->setHeight(v);
    updatingMe = false;
}

void QFxAnchorsPrivate::setItemWidth(qreal v)
{
    updatingMe = true;
    item->setWidth(v);
    updatingMe = false;
}

void QFxAnchorsPrivate::setItemX(qreal v)
{
    updatingMe = true;
    item->setX(v);
    updatingMe = false;
}

void QFxAnchorsPrivate::setItemY(qreal v)
{
    updatingMe = true;
    item->setY(v);
    updatingMe = false;
}

void QFxAnchorsPrivate::setItemPos(const QPointF &v)
{
    updatingMe = true;
    item->setPos(v);
    updatingMe = false;
}

void QFxAnchorsPrivate::updateMe()
{
    if (updatingMe) {
        updatingMe = false;
        return;
    }

    fillChanged();
    centeredInChanged();
    updateHorizontalAnchors();
    updateVerticalAnchors();
}

void QFxAnchorsPrivate::updateOnComplete()
{
    fillChanged();
    centeredInChanged();
    updateHorizontalAnchors();
    updateVerticalAnchors();
}

void QFxAnchorsPrivate::update(QFxItem *, const QRectF &newG, const QRectF &oldG)
{
    fillChanged();
    centeredInChanged();

    if (newG.x() != oldG.x() || newG.width() != oldG.width())
        updateHorizontalAnchors();
    if (newG.y() != oldG.y() || newG.height() != oldG.height())
        updateVerticalAnchors();
}

/*!
    \property QFxAnchors::fill
    \brief which item the item should fill.

    This is a convenience property. It is the same as anchoring the left, right, top, and bottom
    to another item's left, right, top, and bottom.
*/
QFxItem *QFxAnchors::fill() const
{
    Q_D(const QFxAnchors);
    return d->fill;
}

void QFxAnchors::setFill(QFxItem *f)
{
    Q_D(QFxAnchors);
    if (!f) {
        d->remDepend(d->fill);
        d->fill = f;
        return;
    }
    if (f != d->item->itemParent() && f->itemParent() != d->item->itemParent()){
        qmlInfo(d->item) << "Can't anchor to an item that isn't a parent or sibling.";
        return;
    }
    d->remDepend(d->fill);
    d->fill = f;
    d->addDepend(d->fill);

    d->fillChanged();  
}

/*!
    \property QFxAnchors::centeredIn
    \brief which item the item should stay centered in.

    This is a convenience property. It is the same as anchoring the horizontalCenter
    and verticalCenter to another item's horizontalCenter and verticalCenter.
*/
QFxItem *QFxAnchors::centeredIn() const
{
    Q_D(const QFxAnchors);
    return d->centeredIn;
}

void QFxAnchors::setCenteredIn(QFxItem* c)
{
    Q_D(QFxAnchors);
    if (!c) {
        d->remDepend(d->centeredIn);
        d->centeredIn = c;
        return;
    }
    if (c != d->item->itemParent() && c->itemParent() != d->item->itemParent()){
        qmlInfo(d->item) << "Can't anchor to an item that isn't a parent or sibling.";
        return;
    }

    d->remDepend(d->centeredIn);
    d->centeredIn = c;
    d->addDepend(d->centeredIn);

    d->centeredInChanged();
}

bool QFxAnchorsPrivate::calcStretch(const QFxAnchorLine &edge1,
                                    const QFxAnchorLine &edge2,
                                    int offset1,
                                    int offset2,
                                    QFxAnchorLine::AnchorLine line,
                                    int &stretch)
{
    bool edge1IsParent = (edge1.item == item->itemParent());
    bool edge2IsParent = (edge2.item == item->itemParent());
    bool edge1IsSibling = (edge1.item->itemParent() == item->itemParent());
    bool edge2IsSibling = (edge2.item->itemParent() == item->itemParent());

    bool invalid = false;
    if ((edge2IsParent && edge1IsParent) || (edge2IsSibling && edge1IsSibling)) {
        stretch = ((int)position(edge2.item, edge2.anchorLine) + offset2)
                    - ((int)position(edge1.item, edge1.anchorLine) + offset1);
    } else if (edge2IsParent && edge1IsSibling) {
        stretch = ((int)position(edge2.item, edge2.anchorLine) + offset2)
                    - ((int)position(item->itemParent(), line)
                    + (int)position(edge1.item, edge1.anchorLine) + offset1);
    } else if (edge2IsSibling && edge1IsParent) {
        stretch = ((int)position(item->itemParent(), line) + (int)position(edge2.item, edge2.anchorLine) + offset2)
                    - ((int)position(edge1.item, edge1.anchorLine) + offset1);
    } else
        invalid = true;

    return invalid;
}

void QFxAnchorsPrivate::updateVerticalAnchors()
{
    if (fill || centeredIn || !isItemComplete())
        return;

    if (updatingVerticalAnchor < 2) {
        ++updatingVerticalAnchor;
        if (usedAnchors & QFxAnchors::HasTopAnchor) {
            //Handle stretching
            bool invalid = true;
            int height = 0;
            if (usedAnchors & QFxAnchors::HasBottomAnchor) {
                invalid = calcStretch(top, bottom, topMargin, -bottomMargin, QFxAnchorLine::Top, height);
            } else if (usedAnchors & QFxAnchors::HasVCenterAnchor) {
                invalid = calcStretch(top, vCenter, topMargin, vCenterOffset, QFxAnchorLine::Top, height);
                height *= 2;
            }
            if (!invalid)
                setItemHeight(height);

            //Handle top
            if (top.item == item->itemParent()) {
                setItemY(adjustedPosition(top.item, top.anchorLine) + topMargin);
            } else if (top.item->itemParent() == item->itemParent()) {
                setItemY(position(top.item, top.anchorLine) + topMargin);
            }
        } else if (usedAnchors & QFxAnchors::HasBottomAnchor) {
            //Handle stretching (top + bottom case is handled above)
            if (usedAnchors & QFxAnchors::HasVCenterAnchor) {
                int height = 0;
                bool invalid = calcStretch(vCenter, bottom, vCenterOffset, -bottomMargin,
                                              QFxAnchorLine::Top, height);
                if (!invalid)
                    setItemHeight(height*2);
            }

            //Handle bottom
            if (bottom.item == item->itemParent()) {
                setItemY(adjustedPosition(bottom.item, bottom.anchorLine) - item->height() - bottomMargin);
            } else if (bottom.item->itemParent() == item->itemParent()) {
                setItemY(position(bottom.item, bottom.anchorLine) - item->height() - bottomMargin);
            }
        } else if (usedAnchors & QFxAnchors::HasVCenterAnchor) {
            //(stetching handled above)

            //Handle vCenter
            if (vCenter.item == item->itemParent()) {
                setItemY(adjustedPosition(vCenter.item, vCenter.anchorLine)
                              - item->height()/2 + vCenterOffset);
            } else if (vCenter.item->itemParent() == item->itemParent()) {
                setItemY(position(vCenter.item, vCenter.anchorLine) - item->height()/2 + vCenterOffset);
            }
        } else if (usedAnchors & QFxAnchors::HasBaselineAnchor) {
            //Handle baseline
            if (baseline.item->itemParent() == item->itemParent()) {
                setItemY(position(baseline.item, baseline.anchorLine) - item->baselineOffset());
            }
        }
        --updatingVerticalAnchor;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << "Possible anchor loop detected on vertical anchor.";
    }
}

void QFxAnchorsPrivate::updateHorizontalAnchors()
{
    if (fill || centeredIn || !isItemComplete())
        return;

    if (updatingHorizontalAnchor < 2) {
        ++updatingHorizontalAnchor;

        if (usedAnchors & QFxAnchors::HasLeftAnchor) {
            //Handle stretching
            bool invalid = true;
            int width = 0;
            if (usedAnchors & QFxAnchors::HasRightAnchor) {
                invalid = calcStretch(left, right, leftMargin, -rightMargin, QFxAnchorLine::Left, width);
            } else if (usedAnchors & QFxAnchors::HasHCenterAnchor) {
                invalid = calcStretch(left, hCenter, leftMargin, hCenterOffset, QFxAnchorLine::Left, width);
                width *= 2;
            }
            if (!invalid)
                setItemWidth(width);

            //Handle left
            if (left.item == item->itemParent()) {
                setItemX(adjustedPosition(left.item, left.anchorLine) + leftMargin);
            } else if (left.item->itemParent() == item->itemParent()) {
                setItemX(position(left.item, left.anchorLine) + leftMargin);
            }
        } else if (usedAnchors & QFxAnchors::HasRightAnchor) {
            //Handle stretching (left + right case is handled in updateLeftAnchor)
            if (usedAnchors & QFxAnchors::HasHCenterAnchor) {
                int width = 0;
                bool invalid = calcStretch(hCenter, right, hCenterOffset, -rightMargin,
                                              QFxAnchorLine::Left, width);
                if (!invalid)
                    setItemWidth(width*2);
            }

            //Handle right
            if (right.item == item->itemParent()) {
                setItemX(adjustedPosition(right.item, right.anchorLine) - item->width() - rightMargin);
            } else if (right.item->itemParent() == item->itemParent()) {
                setItemX(position(right.item, right.anchorLine) - item->width() - rightMargin);
            }
        } else if (usedAnchors & QFxAnchors::HasHCenterAnchor) {
            //Handle hCenter
            if (hCenter.item == item->itemParent()) {
                setItemX(adjustedPosition(hCenter.item, hCenter.anchorLine) - item->width()/2 + hCenterOffset);
            } else if (hCenter.item->itemParent() == item->itemParent()) {
                setItemX(position(hCenter.item, hCenter.anchorLine) - item->width()/2 + hCenterOffset);
            }
        }

        --updatingHorizontalAnchor;
    } else {
        // ### Make this certain :)
        qmlInfo(item) << "Possible anchor loop detected on horizontal anchor.";
    }
}

QFxAnchorLine QFxAnchors::top() const
{
    Q_D(const QFxAnchors);
    return d->top;
}

void QFxAnchors::setTop(const QFxAnchorLine &edge)
{
    Q_D(QFxAnchors);
    if (!d->checkVAnchorValid(edge))
        return;

    d->usedAnchors |= HasTopAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~HasTopAnchor;
        return;
    }

    d->remDepend(d->top.item);
    d->top = edge;
    d->addDepend(d->top.item);
    d->updateVerticalAnchors();
}

void QFxAnchors::resetTop()
{
    Q_D(QFxAnchors);
    d->usedAnchors &= ~HasTopAnchor;
    d->remDepend(d->top.item);
    d->top = QFxAnchorLine();
    d->updateVerticalAnchors();
}

QFxAnchorLine QFxAnchors::bottom() const
{
    Q_D(const QFxAnchors);
    return d->bottom;
}

void QFxAnchors::setBottom(const QFxAnchorLine &edge)
{
    Q_D(QFxAnchors);
    if (!d->checkVAnchorValid(edge))
        return;

    d->usedAnchors |= HasBottomAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~HasBottomAnchor;
        return;
    }

    d->remDepend(d->bottom.item);
    d->bottom = edge;
    d->addDepend(d->bottom.item);
    d->updateVerticalAnchors();
}

void QFxAnchors::resetBottom()
{
    Q_D(QFxAnchors);
    d->usedAnchors &= ~HasBottomAnchor;
    d->remDepend(d->bottom.item);
    d->bottom = QFxAnchorLine();
    d->updateVerticalAnchors();
}

QFxAnchorLine QFxAnchors::verticalCenter() const
{
    Q_D(const QFxAnchors);
    return d->vCenter;
}

void QFxAnchors::setVerticalCenter(const QFxAnchorLine &edge)
{
    Q_D(QFxAnchors);
    if (!d->checkVAnchorValid(edge))
        return;

    d->usedAnchors |= HasVCenterAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~HasVCenterAnchor;
        return;
    }

    d->remDepend(d->vCenter.item);
    d->vCenter = edge;
    d->addDepend(d->vCenter.item);
    d->updateVerticalAnchors();
}

void QFxAnchors::resetVerticalCenter()
{
    Q_D(QFxAnchors);
    d->usedAnchors &= ~HasVCenterAnchor;
    d->remDepend(d->vCenter.item);
    d->vCenter = QFxAnchorLine();
    d->updateVerticalAnchors();
}

QFxAnchorLine QFxAnchors::baseline() const
{
    Q_D(const QFxAnchors);
    return d->baseline;
}

void QFxAnchors::setBaseline(const QFxAnchorLine &edge)
{
    Q_D(QFxAnchors);
    if (!d->checkVAnchorValid(edge))
        return;

    d->usedAnchors |= HasBaselineAnchor;

    if (!d->checkVValid()) {
        d->usedAnchors &= ~HasBaselineAnchor;
        return;
    }

    d->remDepend(d->baseline.item);
    d->baseline = edge;
    d->addDepend(d->baseline.item);
    d->updateVerticalAnchors();
}

void QFxAnchors::resetBaseline()
{
    Q_D(QFxAnchors);
    d->usedAnchors &= ~HasBaselineAnchor;
    d->remDepend(d->baseline.item);
    d->baseline = QFxAnchorLine();
    d->updateVerticalAnchors();
}

QFxAnchorLine QFxAnchors::left() const
{
    Q_D(const QFxAnchors);
    return d->left;
}

void QFxAnchors::setLeft(const QFxAnchorLine &edge)
{
    Q_D(QFxAnchors);
    if (!d->checkHAnchorValid(edge))
        return;

    d->usedAnchors |= HasLeftAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~HasLeftAnchor;
        return;
    }

    d->remDepend(d->left.item);
    d->left = edge;
    d->addDepend(d->left.item);
    d->updateHorizontalAnchors();
}

void QFxAnchors::resetLeft()
{
    Q_D(QFxAnchors);
    d->usedAnchors &= ~HasLeftAnchor;
    d->remDepend(d->left.item);
    d->left = QFxAnchorLine();
    d->updateHorizontalAnchors();
}

QFxAnchorLine QFxAnchors::right() const
{
    Q_D(const QFxAnchors);
    return d->right;
}

void QFxAnchors::setRight(const QFxAnchorLine &edge)
{
    Q_D(QFxAnchors);
    if (!d->checkHAnchorValid(edge))
        return;

    d->usedAnchors |= HasRightAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~HasRightAnchor;
        return;
    }

    d->remDepend(d->right.item);
    d->right = edge;
    d->addDepend(d->right.item);

    d->updateHorizontalAnchors();
}

void QFxAnchors::resetRight()
{
    Q_D(QFxAnchors);
    d->usedAnchors &= ~HasRightAnchor;
    d->remDepend(d->right.item);
    d->right = QFxAnchorLine();
    d->updateHorizontalAnchors();
}

QFxAnchorLine QFxAnchors::horizontalCenter() const
{
    Q_D(const QFxAnchors);
    return d->hCenter;
}

void QFxAnchors::setHorizontalCenter(const QFxAnchorLine &edge)
{
    Q_D(QFxAnchors);
    if (!d->checkHAnchorValid(edge))
        return;

    d->usedAnchors |= HasHCenterAnchor;

    if (!d->checkHValid()) {
        d->usedAnchors &= ~HasHCenterAnchor;
        return;
    }

    d->remDepend(d->hCenter.item);
    d->hCenter = edge;
    d->addDepend(d->hCenter.item);
    d->updateHorizontalAnchors();
}

void QFxAnchors::resetHorizontalCenter()
{
    Q_D(QFxAnchors);
    d->usedAnchors &= ~HasHCenterAnchor;
    d->remDepend(d->hCenter.item);
    d->hCenter = QFxAnchorLine();
    d->updateHorizontalAnchors();
}

qreal QFxAnchors::leftMargin() const
{
    Q_D(const QFxAnchors);
    return d->leftMargin;
}

void QFxAnchors::setLeftMargin(qreal offset)
{
    Q_D(QFxAnchors);
    if (d->leftMargin == offset)
        return;
    d->leftMargin = offset;
    d->updateHorizontalAnchors();
    emit leftMarginChanged();
}

qreal QFxAnchors::rightMargin() const
{
    Q_D(const QFxAnchors);
    return d->rightMargin;
}

void QFxAnchors::setRightMargin(qreal offset)
{
    Q_D(QFxAnchors);
    if (d->rightMargin == offset)
        return;
    d->rightMargin = offset;
    d->updateHorizontalAnchors();
    emit rightMarginChanged();
}

qreal QFxAnchors::horizontalCenterOffset() const
{
    Q_D(const QFxAnchors);
    return d->hCenterOffset;
}

void QFxAnchors::setHorizontalCenterOffset(qreal offset)
{
    Q_D(QFxAnchors);
    if (d->hCenterOffset == offset)
        return;
    d->hCenterOffset = offset;
    d->updateHorizontalAnchors();
    emit horizontalCenterOffsetChanged();
}

qreal QFxAnchors::topMargin() const
{
    Q_D(const QFxAnchors);
    return d->topMargin;
}

void QFxAnchors::setTopMargin(qreal offset)
{
    Q_D(QFxAnchors);
    if (d->topMargin == offset)
        return;
    d->topMargin = offset;
    d->updateVerticalAnchors();
    emit topMarginChanged();
}

qreal QFxAnchors::bottomMargin() const
{
    Q_D(const QFxAnchors);
    return d->bottomMargin;
}

void QFxAnchors::setBottomMargin(qreal offset)
{
    Q_D(QFxAnchors);
    if (d->bottomMargin == offset)
        return;
    d->bottomMargin = offset;
    d->updateVerticalAnchors();
    emit bottomMarginChanged();
}

qreal QFxAnchors::verticalCenterOffset() const
{
    Q_D(const QFxAnchors);
    return d->vCenterOffset;
}

void QFxAnchors::setVerticalCenterOffset(qreal offset)
{
    Q_D(QFxAnchors);
    if (d->vCenterOffset == offset)
        return;
    d->vCenterOffset = offset;
    d->updateVerticalAnchors();
    emit verticalCenterOffsetChanged();
}

QFxAnchors::UsedAnchors QFxAnchors::usedAnchors() const
{
    Q_D(const QFxAnchors);
    return d->usedAnchors;
}

void QFxAnchors::setItem(QFxItem *item)
{
    Q_D(QFxAnchors);
    d->item = item;
}

bool QFxAnchorsPrivate::checkHValid() const
{
    if (usedAnchors & QFxAnchors::HasLeftAnchor &&
        usedAnchors & QFxAnchors::HasRightAnchor &&
        usedAnchors & QFxAnchors::HasHCenterAnchor) {
        qmlInfo(item) << "Can't specify left, right, and hcenter anchors.";
        return false;
    }

    return true;
}

bool QFxAnchorsPrivate::checkHAnchorValid(QFxAnchorLine anchor) const
{
    if (!anchor.item) {
        qmlInfo(item) << "Can't anchor to a null item.";
        return false;
    } else if (anchor.anchorLine & QFxAnchorLine::Vertical_Mask) {
        qmlInfo(item) << "Can't anchor a horizontal edge to a vertical edge.";
        return false;
    } else if (anchor.item != item->itemParent() && anchor.item->itemParent() != item->itemParent()){
        qmlInfo(item) << "Can't anchor to an item that isn't a parent or sibling.";
        return false;
    } else if (anchor.item == item) {
        qmlInfo(item) << "Can't anchor item to self.";
        return false;
    }

    return true;
}

bool QFxAnchorsPrivate::checkVValid() const
{
    if (usedAnchors & QFxAnchors::HasTopAnchor &&
        usedAnchors & QFxAnchors::HasBottomAnchor &&
        usedAnchors & QFxAnchors::HasVCenterAnchor) {
        qmlInfo(item) << "Can't specify top, bottom, and vcenter anchors.";
        return false;
    } else if (usedAnchors & QFxAnchors::HasBaselineAnchor &&
               (usedAnchors & QFxAnchors::HasTopAnchor ||
                usedAnchors & QFxAnchors::HasBottomAnchor ||
                usedAnchors & QFxAnchors::HasVCenterAnchor)) {
        qmlInfo(item) << "Baseline anchor can't be used in conjunction with top, bottom, or vcenter anchors.";
        return false;
    }

    return true;
}

bool QFxAnchorsPrivate::checkVAnchorValid(QFxAnchorLine anchor) const
{
    if (!anchor.item) {
        qmlInfo(item) << "Can't anchor to a null item.";
        return false;
    } else if (anchor.anchorLine & QFxAnchorLine::Horizontal_Mask) {
        qmlInfo(item) << "Can't anchor a vertical edge to a horizontal edge.";
        return false;
    } else if (anchor.item != item->itemParent() && anchor.item->itemParent() != item->itemParent()){
        qmlInfo(item) << "Can't anchor to an item that isn't a parent or sibling.";
        return false;
    } else if (anchor.item == item){
        qmlInfo(item) << "Can't anchor item to self.";
        return false;
    }

    return true;
}

QT_END_NAMESPACE
