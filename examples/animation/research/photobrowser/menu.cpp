/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "menu.h"

Menu::Menu(QGraphicsItem *parent) : QGraphicsWidget(parent), m_selected(0)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    m_selection = new QGraphicsRectItem(this);
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0,50));
    linearGrad.setColorAt(0, QColor(255,255,255,128));
    linearGrad.setColorAt(1, QColor(255,255,255,16));
    m_selection->setBrush(linearGrad);   
    m_selection->setPen(QPen(Qt::transparent));
}

Menu::~Menu()
{
}


MenuAction *Menu::addAction(const QString &text, QObject *receiver, const char* slot)
{
    MenuAction *action = new MenuAction(text, this);
    if (!m_actions.isEmpty()) {
        MenuAction *last = m_actions.last();
        action->setPos(last->pos() + last->boundingRect().bottomLeft());
    }
    m_actions.append(action);
    if (m_selection->boundingRect().width() < action->boundingRect().width())
        m_selection->setRect(action->boundingRect());

    QObject::connect(action, SIGNAL(triggered()), receiver, slot);
    return action;
}

QRectF Menu::boundingRect() const
{
    QRectF res;
    foreach (MenuAction *a, m_actions)
        res |= a->boundingRect();
    return res;
}

void Menu::keyPressEvent (QKeyEvent * event) 
{
    switch (event->key()) {
        case Qt::Key_Escape:
            hide();
            break;
        case Qt::Key_Up:
            m_selected -= 2;
        case Qt::Key_Down:
            if (!m_actions.isEmpty()) {
                m_selected = (m_selected + 1 + m_actions.count()) % m_actions.count();
                QItemAnimation *anim = new QItemAnimation(m_selection, QItemAnimation::Position);
                anim->setEndValue(m_actions.at(m_selected)->pos());
                anim->start(QAbstractAnimation::DeleteWhenStopped);
            }
            break;  
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (!m_actions.isEmpty()) {
                QItemAnimation *anim = new QItemAnimation(m_selection, QItemAnimation::RotationX);
                anim->setEndValue(m_selection->xRotation() < 180 ? qreal(360) : qreal(0));
                anim->start(QAbstractAnimation::DeleteWhenStopped);
                m_actions.at(m_selected)->trigger();
                hide();
            }            
            break;
        default:
            QGraphicsItem::keyPressEvent(event);
    }
}

void Menu::show()
{
    QItemAnimation *anim = new QItemAnimation(this, QItemAnimation::Opacity);
    anim->setEndValue(qreal(1.));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    anim = new QItemAnimation(m_selection, QItemAnimation::ScaleFactorX);
    anim->setEndValue(qreal(1));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    anim = new QItemAnimation(m_selection, QItemAnimation::ScaleFactorY);
    anim->setEndValue(qreal(1));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    setFocus();
}

void Menu::hide()
{
    QItemAnimation *anim = new QItemAnimation(m_selection, QItemAnimation::ScaleFactorX);
    anim->setEndValue(qreal(.1));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    anim = new QItemAnimation(m_selection, QItemAnimation::ScaleFactorY);
    anim->setEndValue(qreal(.1));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    anim = new QItemAnimation(this, QItemAnimation::Opacity);
    anim->setEndValue(qreal(0));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    parentItem()->setFocus();
}


MenuAction::MenuAction(const QString &text, Menu * parent) 
    : QGraphicsTextItem(text,parent)
{
    QFont f = font();
    f.setPointSize(18);
    setFont(f);
    setDefaultTextColor(Qt::white);
}

void MenuAction::trigger()
{
    emit triggered();
}
