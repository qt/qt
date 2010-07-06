/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef SIGNALSLOTEDITORWINDOW_H
#define SIGNALSLOTEDITORWINDOW_H

#include <QtCore/QPointer>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
class QModelIndex;
class QSortFilterProxyModel;
class QTreeView;
class QToolButton;

namespace qdesigner_internal {

class SignalSlotEditor;
class ConnectionModel;
class Connection;

class SignalSlotEditorWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SignalSlotEditorWindow(QDesignerFormEditorInterface *core, QWidget *parent = 0);

public slots:
    void setActiveFormWindow(QDesignerFormWindowInterface *form);

private slots:
    void updateDialogSelection(Connection *con);
    void updateEditorSelection(const QModelIndex &index);

    void objectNameChanged(QDesignerFormWindowInterface *formWindow, QObject *object, const QString &newName, const QString &oldName);

    void addConnection();
    void removeConnection();
    void updateUi();

private:
    QTreeView *m_view;
    QPointer<SignalSlotEditor> m_editor;
    QToolButton *m_add_button, *m_remove_button;
    QDesignerFormEditorInterface *m_core;
    ConnectionModel *m_model;
    QSortFilterProxyModel *m_proxy_model;
    bool m_handling_selection_change;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // SIGNALSLOTEDITORWINDOW_H
