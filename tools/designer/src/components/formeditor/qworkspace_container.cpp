/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qworkspace_container.h"
#include "qmdiarea_container.h"

#include <QtGui/QWorkspace>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

QWorkspaceContainer::QWorkspaceContainer(QWorkspace *widget, QObject *parent)
    : QObject(parent),
      m_workspace(widget)
{
}

int QWorkspaceContainer::count() const
{
    return m_workspace->windowList(QWorkspace::CreationOrder).count();
}

QWidget *QWorkspaceContainer::widget(int index) const
{
    if (index < 0)
        return 0;
    return m_workspace->windowList(QWorkspace::CreationOrder).at(index);
}

int QWorkspaceContainer::currentIndex() const
{
    if (QWidget *aw = m_workspace->activeWindow())
        return m_workspace->windowList(QWorkspace::CreationOrder).indexOf(aw);
    return -1;
}

void QWorkspaceContainer::setCurrentIndex(int index)
{
    m_workspace->setActiveWindow(m_workspace->windowList(QWorkspace::CreationOrder).at(index));
}

void QWorkspaceContainer::addWidget(QWidget *widget)
{
    QWidget *frame = m_workspace->addWindow(widget, Qt::Window);
    frame->show();
    m_workspace->cascade();
    QMdiAreaContainer::positionNewMdiChild(m_workspace, frame);
}

void QWorkspaceContainer::insertWidget(int, QWidget *widget)
{
    addWidget(widget);
}

void QWorkspaceContainer::remove(int /* index */)
{
    // nothing to do here, reparenting to formwindow is apparently sufficient
}
}

QT_END_NAMESPACE
