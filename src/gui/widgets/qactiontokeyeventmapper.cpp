/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qapplication.h"
#include "qevent.h"
#include "qactiontokeyeventmapper_p.h"

QT_BEGIN_NAMESPACE

QActionToKeyEventMapper::QActionToKeyEventMapper(QAction *softKeyAction, Qt::Key key, QObject *parent)
    : QObject(parent)
    , m_softKeyAction(softKeyAction)
    , m_key(key)
{

}

QString QActionToKeyEventMapper::roleText(QAction::SoftKeyRole role)
{
    switch (role) {
    case QAction::OptionsSoftKey:
        return QAction::tr("Options");
    case QAction::SelectSoftKey:
        return QAction::tr("Select");
    case QAction::BackSoftKey:
        return QAction::tr("Back");
    case QAction::NextSoftKey:
        return QAction::tr("Next");
    case QAction::PreviousSoftKey:
        return QAction::tr("Previous");
    case QAction::OkSoftKey:
        return QAction::tr("Ok");
    case QAction::CancelSoftKey:
        return QAction::tr("Cancel");
    case QAction::EditSoftKey:
        return QAction::tr("Edit");
    case QAction::ViewSoftKey:
        return QAction::tr("View");
    default:
        return QString();
    };
}
void QActionToKeyEventMapper::addSoftKey(QAction::SoftKeyRole standardRole, Qt::Key key, QWidget *actionWidget)
{
    QAction *action = new QAction(actionWidget);
    action->setSoftKeyRole(standardRole);
    action->setText(roleText(standardRole));
    QActionToKeyEventMapper *softKey = new QActionToKeyEventMapper(action, key, actionWidget);
    connect(action, SIGNAL(triggered()), softKey, SLOT(sendKeyEvent()));
    connect(action, SIGNAL(destroyed()), softKey, SLOT(deleteLater()));
    actionWidget->setSoftKey(action);
}

void QActionToKeyEventMapper::removeSoftkey(QWidget *focussedWidget)
{
    focussedWidget->setSoftKey(0);
}

void QActionToKeyEventMapper::sendKeyEvent()
{
    QApplication::postEvent(parent(), new QKeyEvent(QEvent::KeyPress, m_key, Qt::NoModifier));
}

QT_END_NAMESPACE

