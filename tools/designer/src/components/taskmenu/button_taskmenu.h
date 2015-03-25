/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BUTTON_TASKMENU_H
#define BUTTON_TASKMENU_H

#include <QtGui/QAbstractButton>
#include <QtGui/QCommandLinkButton>
#include <QtGui/QButtonGroup>

#include <qdesigner_taskmenu_p.h>
#include <extensionfactory_p.h>

QT_BEGIN_NAMESPACE

class QMenu;
class QActionGroup;
class QDesignerFormWindowCursorInterface;

namespace qdesigner_internal {

// ButtonGroupMenu: Mixin menu for the 'select members'/'break group' options of
// the task menu of buttons and button group
class ButtonGroupMenu : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ButtonGroupMenu)
public:
    ButtonGroupMenu(QObject *parent = 0);

    void initialize(QDesignerFormWindowInterface *formWindow,
                    QButtonGroup *buttonGroup = 0,
                    /* Current button for selection in ButtonMode */
                    QAbstractButton *currentButton = 0);

    QAction *selectGroupAction() const { return m_selectGroupAction; }
    QAction *breakGroupAction() const  { return m_breakGroupAction; }

private slots:
    void selectGroup();
    void breakGroup();

private:
    QAction *m_selectGroupAction;
    QAction *m_breakGroupAction;

    QDesignerFormWindowInterface *m_formWindow;
    QButtonGroup *m_buttonGroup;
    QAbstractButton *m_currentButton;
};

// Task menu extension of a QButtonGroup
class ButtonGroupTaskMenu : public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_DISABLE_COPY(ButtonGroupTaskMenu)
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    explicit ButtonGroupTaskMenu(QButtonGroup *buttonGroup, QObject *parent = 0);

    virtual QAction *preferredEditAction() const;
    virtual QList<QAction*> taskActions() const;

private:
    QButtonGroup *m_buttonGroup;
    QList<QAction*> m_taskActions;
    mutable ButtonGroupMenu m_menu;
};

// Task menu extension of a QAbstractButton
class ButtonTaskMenu: public QDesignerTaskMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(ButtonTaskMenu)
public:
    explicit ButtonTaskMenu(QAbstractButton *button, QObject *parent = 0);
    virtual ~ButtonTaskMenu();

    virtual QAction *preferredEditAction() const;
    virtual QList<QAction*> taskActions() const;

    QAbstractButton *button() const;

protected:
    void insertAction(int index, QAction *a);

private slots:
    void createGroup();
    void addToGroup(QAction *a);
    void removeFromGroup();

private:
    enum SelectionType {
        OtherSelection,
        UngroupedButtonSelection,
        GroupedButtonSelection
    };

    SelectionType selectionType(const QDesignerFormWindowCursorInterface *cursor, QButtonGroup ** ptrToGroup = 0) const;
    bool refreshAssignMenu(const QDesignerFormWindowInterface *fw, int buttonCount, SelectionType st, QButtonGroup *currentGroup);
    QMenu *createGroupSelectionMenu(const QDesignerFormWindowInterface *fw);

    QList<QAction*> m_taskActions;
    mutable ButtonGroupMenu m_groupMenu;
    QMenu *m_assignGroupSubMenu;
    QActionGroup *m_assignActionGroup;
    QAction *m_assignToGroupSubMenuAction;
    QMenu *m_currentGroupSubMenu;
    QAction *m_currentGroupSubMenuAction;

    QAction *m_createGroupAction;
    QAction *m_preferredEditAction;
    QAction *m_removeFromGroupAction;
};

// Task menu extension of a QCommandLinkButton
class CommandLinkButtonTaskMenu: public ButtonTaskMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(CommandLinkButtonTaskMenu)
public:
    explicit CommandLinkButtonTaskMenu(QCommandLinkButton *button, QObject *parent = 0);
};

typedef ExtensionFactory<QDesignerTaskMenuExtension, QButtonGroup, ButtonGroupTaskMenu> ButtonGroupTaskMenuFactory;
typedef ExtensionFactory<QDesignerTaskMenuExtension, QCommandLinkButton, CommandLinkButtonTaskMenu>  CommandLinkButtonTaskMenuFactory;
typedef ExtensionFactory<QDesignerTaskMenuExtension, QAbstractButton, ButtonTaskMenu>  ButtonTaskMenuFactory;
}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // BUTTON_TASKMENU_H
