/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Assistant module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GLOBALACTION_H
#define GLOBALACTION_H

#include <QtCore/QList>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QAction;
class QMenu;

class GlobalActions : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(GlobalActions)
public:
    static GlobalActions *instance(QObject *parent = 0);

    QList<QAction *> actionList() const { return m_actionList; }
    QAction *backAction() const { return m_backAction; }
    QAction *nextAction() const { return m_nextAction; }
    QAction *homeAction() const { return m_homeAction; }
    QAction *zoomInAction() const { return m_zoomInAction; }
    QAction *zoomOutAction() const { return m_zoomOutAction; }
    QAction *copyAction() const { return m_copyAction; }
    QAction *printAction() const { return m_printAction; }
    QAction *findAction() const { return m_findAction; }

    Q_SLOT void updateActions();
    Q_SLOT void setCopyAvailable(bool available);

#if !defined(QT_NO_WEBKIT)
private slots:
    void slotAboutToShowBackMenu();
    void slotAboutToShowNextMenu();
    void slotOpenActionUrl(QAction *action);
#endif

private:
    void setupNavigationMenus(QAction *back, QAction *next, QWidget *parent);

private:
    GlobalActions(QObject *parent);

    static GlobalActions *m_instance;

    QAction *m_backAction;
    QAction *m_nextAction;
    QAction *m_homeAction;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_copyAction;
    QAction *m_printAction;
    QAction *m_findAction;

    QList<QAction *> m_actionList;

    QMenu *m_backMenu;
    QMenu *m_nextMenu;
};

QT_END_NAMESPACE

#endif // GLOBALACTION_H
