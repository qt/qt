/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
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

#include "qmenu.h"
#include "qapplication.h"
#include "qevent.h"
#include "qstyle.h"
#include "qdebug.h"
#include "qwidgetaction.h"
#include <private/qapplication_p.h>
#include <private/qmenu_p.h>
#include <private/qmenubar_p.h>
#include <qt_s60_p.h>
#include <QtCore/qlibrary.h>

#ifdef Q_WS_S60
#include <eikmenub.h>
#include <eikmenup.h>
#include <eikaufty.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#endif

#if !defined(QT_NO_MENUBAR) && defined(Q_WS_S60)

QT_BEGIN_NAMESPACE

typedef QMultiHash<QWidget *, QMenuBarPrivate *> MenuBarHash;
Q_GLOBAL_STATIC(MenuBarHash, menubars)

#define QT_FIRST_MENU_ITEM 32000

struct SymbianMenuItem
{
    int id;
    CEikMenuPaneItem::SData menuItemData;
    QList<SymbianMenuItem*> children;
    QAction* action;
};

static QList<SymbianMenuItem*> symbianMenus;
static QList<QMenuBar*> nativeMenuBars;
static uint qt_symbian_menu_static_cmd_id = QT_FIRST_MENU_ITEM;
static QPointer<QWidget> widgetWithContextMenu;
static QList<QAction*> contextMenuActionList;
static QAction contextAction(0);
static int contexMenuCommand=0;

bool menuExists()
{
    QWidget *w = qApp->activeWindow();
    QMenuBarPrivate *mb = menubars()->value(w);
    if ((!mb) && !menubars()->count()) 
        return false;
    return true;
}

static bool hasContextMenu(QWidget* widget)
{
    if (!widget)
        return false;
    const Qt::ContextMenuPolicy policy = widget->contextMenuPolicy();
    if (policy != Qt::NoContextMenu && policy != Qt::PreventContextMenu ) {
        return true;
    }
    return false;
}
// ### FIX THIS, copy/paste of original (faulty) stripped text implementation.
// Implementation should be removed from QAction implementation to some generic place
static QString qt_strippedText_copy_from_qaction(QString s)
{
    s.remove(QString::fromLatin1("..."));
    int i = 0;
    while (i < s.size()) {
        ++i;
        if (s.at(i-1) != QLatin1Char('&'))
            continue;
        if (i < s.size() && s.at(i) == QLatin1Char('&'))
            ++i;
        s.remove(i-1,1);
    }
    return s.trimmed();
};

static SymbianMenuItem* qt_symbian_find_menu(int id, const QList<SymbianMenuItem*> &parent)
{
    int index=0;
    while (index < parent.count()) {
        SymbianMenuItem* temp = parent[index];
        if (temp->menuItemData.iCascadeId == id)
           return temp;
        else if (temp->menuItemData.iCascadeId != 0) {
            SymbianMenuItem* result = qt_symbian_find_menu( id, temp->children);
            if (result)
                return result;
        }
        index++;
    }
    return 0;
}

static SymbianMenuItem* qt_symbian_find_menu_item(int id, const QList<SymbianMenuItem*> &parent)
{
    int index=0;
    while (index < parent.count()) {
        SymbianMenuItem* temp = parent[index];
        if (temp->menuItemData.iCascadeId != 0) {
            SymbianMenuItem* result = qt_symbian_find_menu_item( id, temp->children);
            if (result)
                return result;
        }
        else if (temp->menuItemData.iCommandId == id)
            return temp;
        index++;

    }
    return 0;
}

static void qt_symbian_insert_action(QSymbianMenuAction* action, QList<SymbianMenuItem*>* parent)
{
    if (action->action->isVisible()) {
        if (action->action->isSeparator())
            return;

// ### FIX THIS, the qt_strippedText2 doesn't work perfectly for stripping & marks. Same bug is in QAction
//     New really working method is needed in a place where the implementation isn't copy/pasted
        QString text = qt_strippedText_copy_from_qaction(action->action->text());
        TPtrC menuItemText(qt_QString2TPtrC(text));

        if (action->action->menu()) {
            SymbianMenuItem* menuItem = new SymbianMenuItem();
            menuItem->menuItemData.iCascadeId = action->command;
            menuItem->menuItemData.iCommandId = action->command;
            menuItem->menuItemData.iFlags = 0;
            menuItem->menuItemData.iText = menuItemText;
            menuItem->action = action->action;
            if (action->action->menu()->actions().size() == 0 || !action->action->isEnabled() )
                menuItem->menuItemData.iFlags |= EEikMenuItemDimmed;
            parent->append(menuItem);

            if (action->action->menu()->actions().size() > 0) {
                for (int c2= 0; c2 < action->action->menu()->actions().size(); ++c2) {
                    QSymbianMenuAction *symbianAction2 = new QSymbianMenuAction;
                    symbianAction2->action = action->action->menu()->actions().at(c2);
                    QMenu * menu = symbianAction2->action->menu();
                    symbianAction2->command = qt_symbian_menu_static_cmd_id++;
                    qt_symbian_insert_action(symbianAction2, &(menuItem->children));
                }
            }

        } else {
            SymbianMenuItem* menuItem = new SymbianMenuItem();
            menuItem->menuItemData.iCascadeId = 0;
            menuItem->menuItemData.iCommandId = action->command;
            menuItem->menuItemData.iFlags = 0;
            menuItem->menuItemData.iText = menuItemText;
            menuItem->action = action->action;
            if (!action->action->isEnabled()){
                menuItem->menuItemData.iFlags += EEikMenuItemDimmed;
            }
            
            if (action->action->isCheckable()) {
                if (action->action->isChecked())
                    menuItem->menuItemData.iFlags += EEikMenuItemCheckBox | EEikMenuItemSymbolOn;
                else
                    menuItem->menuItemData.iFlags += EEikMenuItemCheckBox;
            }
            parent->append(menuItem);
        }
    }
}

void deleteAll(QList<SymbianMenuItem*> *items)
{
    while (!items->isEmpty()) {
        SymbianMenuItem* temp = items->takeFirst();
        deleteAll(&temp->children);
        delete temp;
    }
}

static void rebuildMenu()
{
    widgetWithContextMenu = 0;
    QMenuBarPrivate *mb = 0;
    QWidget *w = qApp->activeWindow();
    QWidget* focusWidget = QApplication::focusWidget();
    if (focusWidget) {
        if (hasContextMenu(focusWidget))
            widgetWithContextMenu = focusWidget;
    }

    if (w) {
        mb = menubars()->value(w);
        qt_symbian_menu_static_cmd_id = QT_FIRST_MENU_ITEM;
        deleteAll( &symbianMenus );
        if (!mb)
            return;
        mb->symbian_menubar->rebuild();
    }
}

Q_GUI_EXPORT void qt_symbian_show_toplevel( CEikMenuPane* menuPane)
{
    if (!menuExists())
        return;
    rebuildMenu();
    for (int i = 0; i < symbianMenus.count(); ++i)
        menuPane->AddMenuItemL(symbianMenus.at(i)->menuItemData);
}

Q_GUI_EXPORT void qt_symbian_show_submenu( CEikMenuPane* menuPane, int id)
{
    SymbianMenuItem* menu = qt_symbian_find_menu(id, symbianMenus);
    if (menu) {
        for (int i = 0; i < menu->children.count(); ++i)
            menuPane->AddMenuItemL(menu->children.at(i)->menuItemData);
    }
}

void QMenuBarPrivate::symbianCommands(int command)
{
    if (command == contexMenuCommand && !widgetWithContextMenu.isNull()) {
        QContextMenuEvent* event = new QContextMenuEvent(QContextMenuEvent::Keyboard, QPoint(0,0));
        QCoreApplication::postEvent(widgetWithContextMenu, event);
    }
    
    int size = nativeMenuBars.size();
    for (int i = 0; i < nativeMenuBars.size(); ++i) {
    SymbianMenuItem* menu = qt_symbian_find_menu_item(command, symbianMenus);
    if (!menu)
            continue;

        emit nativeMenuBars.at(i)->triggered(menu->action);
    menu->action->activate(QAction::Trigger);
        break;
    }
}

void QMenuBarPrivate::symbianCreateMenuBar(QWidget *parent)
{
    Q_Q(QMenuBar);
    if (parent && parent->isWindow()){
        menubars()->insert(q->window(), this);
        symbian_menubar = new QSymbianMenuBarPrivate(this);
        nativeMenuBars.append(q);
    }
}

void QMenuBarPrivate::symbianDestroyMenuBar()
{
    Q_Q(QMenuBar);
    int index = nativeMenuBars.indexOf(q);
    nativeMenuBars.removeAt(index);
    menubars()->remove(q->window(), this);
    rebuildMenu();
    if (symbian_menubar)
        delete symbian_menubar;
    symbian_menubar = 0;
}

QMenuBarPrivate::QSymbianMenuBarPrivate::QSymbianMenuBarPrivate(QMenuBarPrivate *menubar)
{
    d = menubar;
}

QMenuBarPrivate::QSymbianMenuBarPrivate::~QSymbianMenuBarPrivate()
{
    deleteAll( &symbianMenus );
    symbianMenus.clear();
    d = 0;
    rebuild();
}

QMenuPrivate::QSymbianMenuPrivate::QSymbianMenuPrivate()
{
}

QMenuPrivate::QSymbianMenuPrivate::~QSymbianMenuPrivate()
{

}

void QMenuPrivate::QSymbianMenuPrivate::addAction(QAction *a, QSymbianMenuAction *before)
{
    QSymbianMenuAction *action = new QSymbianMenuAction;
    action->action = a;
    action->command = qt_symbian_menu_static_cmd_id++;
    addAction(action, before);
}

void QMenuPrivate::QSymbianMenuPrivate::addAction(QSymbianMenuAction *action, QSymbianMenuAction *before)
{
    if (!action)
        return;
    int before_index = actionItems.indexOf(before);
    if (before_index < 0) {
        before = 0;
        before_index = actionItems.size();
    }
    actionItems.insert(before_index, action);
}


void QMenuPrivate::QSymbianMenuPrivate::syncAction(QSymbianMenuAction *)
{
    rebuild();
}

void QMenuPrivate::QSymbianMenuPrivate::removeAction(QSymbianMenuAction *action)
{
    actionItems.removeAll(action);
    delete action;
    action = 0;
    rebuild();
}

void QMenuPrivate::QSymbianMenuPrivate::rebuild(bool)
{
}

void QMenuBarPrivate::QSymbianMenuBarPrivate::addAction(QAction *a, QSymbianMenuAction *before)
{
    QSymbianMenuAction *action = new QSymbianMenuAction;
    action->action = a;
    action->command = qt_symbian_menu_static_cmd_id++;
    addAction(action, before);
}

void QMenuBarPrivate::QSymbianMenuBarPrivate::addAction(QSymbianMenuAction *action, QSymbianMenuAction *before)
{
    if (!action)
        return;
    int before_index = actionItems.indexOf(before);
    if (before_index < 0) {
        before = 0;
        before_index = actionItems.size();
    }
    actionItems.insert(before_index, action);
}

void QMenuBarPrivate::QSymbianMenuBarPrivate::syncAction(QSymbianMenuAction*)
{
    rebuild();
}

void QMenuBarPrivate::QSymbianMenuBarPrivate::removeAction(QSymbianMenuAction *action)
{
    actionItems.removeAll(action);
    delete action;
    rebuild();
}

void QMenuBarPrivate::QSymbianMenuBarPrivate::insertNativeMenuItems(const QList<QAction*> &actions)
{
    for (int i = 0; i <actions.size(); ++i) {
        QSymbianMenuAction *symbianActionTopLevel = new QSymbianMenuAction;
        symbianActionTopLevel->action = actions.at(i);
        symbianActionTopLevel->parent = 0;
        symbianActionTopLevel->command = qt_symbian_menu_static_cmd_id++;
        qt_symbian_insert_action(symbianActionTopLevel, &symbianMenus);
    }
}



void QMenuBarPrivate::QSymbianMenuBarPrivate::rebuild()
{
    contexMenuCommand = 0;
    qt_symbian_menu_static_cmd_id = QT_FIRST_MENU_ITEM;
    deleteAll( &symbianMenus );
    if (d)
        insertNativeMenuItems(d->actions);

    contextMenuActionList.clear();
    if (widgetWithContextMenu) {
        contexMenuCommand = qt_symbian_menu_static_cmd_id;
        contextAction.setText(QMenuBar::tr("Actions"));
        contextMenuActionList.append(&contextAction);
        insertNativeMenuItems(contextMenuActionList);
    }
}
QT_END_NAMESPACE

#endif //QT_NO_MENUBAR
