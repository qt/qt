/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#include "qmenu.h"
#include "qapplication.h"
#include "qmainwindow.h"
#include "qtoolbar.h"
#include "qevent.h"
#include "qstyle.h"
#include "qdebug.h"
#include "qwidgetaction.h"
#include <eikmenub.h>
#include <eikmenup.h>
#include <private/qapplication_p.h>
#include <private/qmenu_p.h>
#include <private/qmenubar_p.h>
#include <qt_s60_p.h>
#include <eikaufty.h>
#include <eikbtgpc.h>
#include <QtCore/qlibrary.h>
#include <avkon.rsg>

#ifndef QT_NO_MENUBAR

QT_BEGIN_NAMESPACE

// ### FIX/Document this, we need some safe range of menu id's for Qt that don't clash with AIW ones
#define QT_FIRST_MENU_ITEM 32000
static QList<QMenuBarPrivate *> s60_menubars;

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
        HBufC* menuItemText = qt_QString2HBufCNewL(text);

        if (action->action->menu()) {
            SymbianMenuItem* menuItem = new SymbianMenuItem();
            menuItem->menuItemData.iCascadeId = action->command;
            menuItem->menuItemData.iCommandId = action->command;
            menuItem->menuItemData.iFlags = 0;
            menuItem->menuItemData.iText = *menuItemText;
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
            menuItem->menuItemData.iText = *menuItemText;
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
        delete menuItemText;
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

static void setSoftkeys()
{
    CEikButtonGroupContainer* cba = CEikonEnv::Static()->AppUiFactory()->Cba();
    if (cba){
    if (s60_menubars.count()>0)
        cba->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
    else
        cba->SetCommandSetL(R_AVKON_SOFTKEYS_EXIT);
    }
}

static void rebuildMenu()
{
    qt_symbian_menu_static_cmd_id = QT_FIRST_MENU_ITEM;
    deleteAll( &symbianMenus );
    if (s60_menubars.count()==0)
        return;
    for (int i = 0; i < s60_menubars.last()->actions.size(); ++i) {
        QSymbianMenuAction *symbianActionTopLevel = new QSymbianMenuAction;
        symbianActionTopLevel->action = s60_menubars.last()->actions.at(i);
        symbianActionTopLevel->parent = 0;
        symbianActionTopLevel->command = qt_symbian_menu_static_cmd_id++;
        qt_symbian_insert_action(symbianActionTopLevel, &symbianMenus);
    }

    return;
 }

Q_GUI_EXPORT void qt_symbian_show_toplevel( CEikMenuPane* menuPane)
{
    if (s60_menubars.count()==0)
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

void QMenu::symbianCommands(int command)
{
    Q_D(QMenu);
    d->symbianCommands(command);
}

void QMenuBar::symbianCommands(int command)
{
    int size = nativeMenuBars.size();
    for (int i = 0; i < nativeMenuBars.size(); ++i) {
        bool result = nativeMenuBars.at(i)->d_func()->symbianCommands(command);
        if (result)
            return;
    }
}

bool QMenuBarPrivate::symbianCommands(int command)
{
    SymbianMenuItem* menu = qt_symbian_find_menu_item(command, symbianMenus);
    if (!menu)
        return false;

    emit q_func()->triggered(menu->action);
    menu->action->activate(QAction::Trigger);
    return true;
}

bool QMenuPrivate::symbianCommands(int command)
{
    SymbianMenuItem* menu = qt_symbian_find_menu_item(command, symbianMenus);
    if (!menu)
        return false;

    emit q_func()->triggered(menu->action);
    menu->action->activate(QAction::Trigger);
    return true;
}

void QMenuBarPrivate::symbianCreateMenuBar(QWidget *parent)
{
    Q_Q(QMenuBar);
    if (parent && parent->isWindow()){
        symbian_menubar = new QSymbianMenuBarPrivate(this);
        nativeMenuBars.append(q);
    }
}

void QMenuBarPrivate::symbianDestroyMenuBar()
{
    Q_Q(QMenuBar);
    int index = nativeMenuBars.indexOf(q);
    nativeMenuBars.removeAt(index);
    s60_menubars.removeLast();
    rebuildMenu();
    if (symbian_menubar)
        delete symbian_menubar;
    symbian_menubar = 0;
}

QMenuBarPrivate::QSymbianMenuBarPrivate::QSymbianMenuBarPrivate(QMenuBarPrivate *menubar)
{
    d = menubar;
    s60_menubars.append(menubar);
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

void QMenuBarPrivate::QSymbianMenuBarPrivate::rebuild()
{
    setSoftkeys();
    if (s60_menubars.count()==0)
        return;

    rebuildMenu();
 }

#endif //QT_NO_MENUBAR
