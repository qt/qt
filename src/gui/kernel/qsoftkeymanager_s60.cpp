/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qapplication.h"
#include "qevent.h"
#include "qbitmap.h"
#include "qstyle.h"
#include "qmenubar.h"
#include "private/qt_s60_p.h"
#include "private/qmenu_p.h"
#include "private/qsoftkeymanager_p.h"
#include "private/qsoftkeymanager_s60_p.h"
#include "private/qobject_p.h"
//#include <eiksoftkeyimage.h>
#include <eikcmbut.h>

#ifndef QT_NO_SOFTKEYMANAGER
QT_BEGIN_NAMESPACE

const int S60_COMMAND_START = 6000;
const int LSK_POSITION = 0;
const int MSK_POSITION = 3;
const int RSK_POSITION = 2;

QSoftKeyManagerPrivateS60::QSoftKeyManagerPrivateS60()
{
    cachedCbaIconSize[0] = QSize(0,0);
    cachedCbaIconSize[1] = QSize(0,0);
    skipNextUpdate = false;
}

bool QSoftKeyManagerPrivateS60::skipCbaUpdate()
{
    // lets not update softkeys if
    // 1. We don't have application panes, i.e. cba
    // 2. S60 native dialog or menu is shown
    if (QApplication::testAttribute(Qt::AA_S60DontConstructApplicationPanes) ||
        CCoeEnv::Static()->AppUi()->IsDisplayingMenuOrDialog() || skipNextUpdate) {
        skipNextUpdate = false;
        return true;
    }
    return false;
}

void QSoftKeyManagerPrivateS60::ensureCbaVisibilityAndResponsiviness(CEikButtonGroupContainer &cba)
{
    RDrawableWindow *cbaWindow = cba.DrawableWindow();
    Q_ASSERT_X(cbaWindow, Q_FUNC_INFO, "Native CBA does not have window!");
    // Make sure CBA is visible, i.e. CBA window is on top
    cbaWindow->SetOrdinalPosition(0);
    // Qt shares same CBA instance between top-level widgets,
    // make sure we are not faded by underlying window.
    cbaWindow->SetFaded(EFalse, RWindowTreeNode::EFadeIncludeChildren);
    // Modal dialogs capture pointer events, but shared cba instance
    // shall stay responsive. Raise pointer capture priority to keep
    // softkeys responsive in modal dialogs
    cbaWindow->SetPointerCapturePriority(1);
}

void QSoftKeyManagerPrivateS60::clearSoftkeys(CEikButtonGroupContainer &cba)
{
    QT_TRAP_THROWING(
        //Using -1 instead of EAknSoftkeyEmpty to avoid flickering.
        cba.SetCommandL(0, -1, KNullDesC);
        // TODO: Should we clear also middle SK?
        cba.SetCommandL(2, -1, KNullDesC);
    );
    realSoftKeyActions.clear();
}

QString QSoftKeyManagerPrivateS60::softkeyText(QAction &softkeyAction)
{
    // In S60 softkeys and menu items do not support key accelerators (i.e.
    // CTRL+X). Therefore, removing the accelerator characters from both softkey
    // and menu item texts.
    const int underlineShortCut = QApplication::style()->styleHint(QStyle::SH_UnderlineShortcut);
    QString iconText = softkeyAction.iconText();
    return underlineShortCut ? softkeyAction.text() : iconText;
}

QAction *QSoftKeyManagerPrivateS60::highestPrioritySoftkey(QAction::SoftKeyRole role)
{
    QAction *ret = NULL;
    // Priority look up is two level
    // 1. First widget with softkeys always has highest priority
    for (int level = 0; !ret; level++) {
        // 2. Highest priority action within widget
        QList<QAction*> actions = requestedSoftKeyActions.values(level);
        if (actions.isEmpty())
            break;
        qSort(actions.begin(), actions.end(), QSoftKeyManagerPrivateS60::actionPriorityMoreThan);
        foreach (QAction *action, actions) {
            if (action->softKeyRole() == role) {
                ret = action;
                break;
            }
        }
    }
    return ret;
}

bool QSoftKeyManagerPrivateS60::actionPriorityMoreThan(const QAction *firstItem, const QAction *secondItem)
{
    return firstItem->priority() > secondItem->priority();
}

void QSoftKeyManagerPrivateS60::setNativeSoftkey(CEikButtonGroupContainer &cba,
                                              TInt position, TInt command, const TDesC &text)
{
    // Calling SetCommandL causes CBA redraw
    QT_TRAP_THROWING(cba.SetCommandL(position, command, text));
}

bool QSoftKeyManagerPrivateS60::isOrientationLandscape()
{
    // Hard to believe that there is no public API in S60 to
    // get current orientation. This workaround works with currently supported resolutions
    return  S60->screenHeightInPixels <  S60->screenWidthInPixels;
}

QSize QSoftKeyManagerPrivateS60::cbaIconSize(CEikButtonGroupContainer *cba, int position)
{
    Q_UNUSED(cba);
    Q_UNUSED(position);

    // Will be implemented when EikSoftkeyImage usage license wise is OK
/*
    const int index = isOrientationLandscape() ? 0 : 1;
    if(cachedCbaIconSize[index].isNull()) {
        // Only way I figured out to get CBA icon size without RnD SDK, was
        // Only way I figured out to get CBA icon size without RnD SDK, was
        // to set some dummy icon to CBA first and then ask CBA button CCoeControl::Size()
        // The returned value is cached to avoid unnecessary icon setting every time.
        const bool left = (position == LSK_POSITION);
        if(position == LSK_POSITION || position == RSK_POSITION) {
            CEikImage* tmpImage = NULL;
            QT_TRAP_THROWING(tmpImage = new (ELeave) CEikImage);
            EikSoftkeyImage::SetImage(cba, *tmpImage, left); // Takes myimage ownership
            int command = S60_COMMAND_START + position;
            setNativeSoftkey(*cba, position, command, KNullDesC());
            cachedCbaIconSize[index] = qt_TSize2QSize(cba->ControlOrNull(command)->Size());
            EikSoftkeyImage::SetLabel(cba, left);
        }
    }

    return cachedCbaIconSize[index];
*/
    return QSize();
}

bool QSoftKeyManagerPrivateS60::setSoftkeyImage(CEikButtonGroupContainer *cba,
                                            QAction &action, int position)
{
    bool ret = false;
    Q_UNUSED(cba);
    Q_UNUSED(action);
    Q_UNUSED(position);

    // Will be implemented when EikSoftkeyImage usage license wise is OK
    /*
    const bool left = (position == LSK_POSITION);
    if(position == LSK_POSITION || position == RSK_POSITION) {
        QIcon icon = action.icon();
        if (!icon.isNull()) {
            QPixmap pm = icon.pixmap(cbaIconSize(cba, position));
            pm = pm.scaled(cbaIconSize(cba, position));
            QBitmap mask = pm.mask();
            if (mask.isNull()) {
                mask = QBitmap(pm.size());
                mask.fill(Qt::color1);
            }

            CFbsBitmap* nBitmap = pm.toSymbianCFbsBitmap();
            CFbsBitmap* nMask = mask.toSymbianCFbsBitmap();

            CEikImage* myimage = new (ELeave) CEikImage;
            myimage->SetPicture( nBitmap, nMask ); // nBitmap and nMask ownership transfered

            EikSoftkeyImage::SetImage(cba, *myimage, left); // Takes myimage ownership
            ret = true;
        } else {
            // Restore softkey to text based
            EikSoftkeyImage::SetLabel(cba, left);
        }
    }
    */
    return ret;
}

bool QSoftKeyManagerPrivateS60::setSoftkey(CEikButtonGroupContainer &cba,
                                        QAction::SoftKeyRole role, int position)
{
    QAction *action = highestPrioritySoftkey(role);
    if (action) {
        setSoftkeyImage(&cba, *action, position);
        QString text = softkeyText(*action);
        TPtrC nativeText = qt_QString2TPtrC(text);
        int command = S60_COMMAND_START + position;
        setNativeSoftkey(cba, position, command, nativeText);
        cba.DimCommand(command, !action->isEnabled());
        realSoftKeyActions.insert(command, action);
        return true;
    }
    return false;
}

bool QSoftKeyManagerPrivateS60::setLeftSoftkey(CEikButtonGroupContainer &cba)
{
    return setSoftkey(cba, QAction::PositiveSoftKey, LSK_POSITION);
}

bool QSoftKeyManagerPrivateS60::setMiddleSoftkey(CEikButtonGroupContainer &cba)
{
    // Note: In order to get MSK working, application has to have EAknEnableMSK flag set
    // Currently it is not possible very easily)
    // For more information see: http://wiki.forum.nokia.com/index.php/Middle_softkey_usage
    return setSoftkey(cba, QAction::SelectSoftKey, MSK_POSITION);
}

bool QSoftKeyManagerPrivateS60::setRightSoftkey(CEikButtonGroupContainer &cba)
{
    if (!setSoftkey(cba, QAction::NegativeSoftKey, RSK_POSITION)) {
        Qt::WindowType windowType = Qt::Window;
        QAction *action = requestedSoftKeyActions.value(0);
        if (action) {
            QWidget *actionParent = action->parentWidget();
            Q_ASSERT_X(actionParent, Q_FUNC_INFO, "No parent set for softkey action!");

            QWidget *actionWindow = actionParent->window();
            Q_ASSERT_X(actionWindow, Q_FUNC_INFO, "Softkey action does not have window!");
            windowType = actionWindow->windowType();
        }

        if (windowType != Qt::Dialog && windowType != Qt::Popup) {
            QString text(QSoftKeyManager::tr("Exit"));
            TPtrC nativeText = qt_QString2TPtrC(text);
            setNativeSoftkey(cba, RSK_POSITION, EAknSoftkeyExit, nativeText);
            return true;
        }
    }
    return false;
}

void QSoftKeyManagerPrivateS60::setSoftkeys(CEikButtonGroupContainer &cba)
{
    int requestedSoftkeyCount = requestedSoftKeyActions.count();
    const int maxSoftkeyCount = 2; // TODO: differs based on orientation ans S60 versions (some have MSK)
    if (requestedSoftkeyCount > maxSoftkeyCount) {
        // We have more softkeys than available slots
        // Put highest priority negative action to RSK and Options menu with rest of softkey actions to LSK
        // TODO: Build menu
        setLeftSoftkey(cba);
        if(AknLayoutUtils::MSKEnabled())
            setMiddleSoftkey(cba);
        setRightSoftkey(cba);
    } else {
        // We have less softkeys than available slots
        // Put softkeys to request slots based on role
        setLeftSoftkey(cba);
        if(AknLayoutUtils::MSKEnabled())
            setMiddleSoftkey(cba);
        setRightSoftkey(cba);
    }
}

void QSoftKeyManagerPrivateS60::updateSoftKeys_sys()
{
    //bool status = CCoeEnv::Static()->AppUi()->IsDisplayingMenuOrDialog();
    if (skipCbaUpdate())
        return;

    CEikButtonGroupContainer *nativeContainer = S60->buttonGroupContainer();
    Q_ASSERT_X(nativeContainer, Q_FUNC_INFO, "Native CBA does not exist!");
    ensureCbaVisibilityAndResponsiviness(*nativeContainer);
    clearSoftkeys(*nativeContainer);
    setSoftkeys(*nativeContainer);

    nativeContainer->DrawDeferred(); // 3.1 needs an extra invitation
}

bool QSoftKeyManagerPrivateS60::handleCommand(int command)
{
    QAction *action = realSoftKeyActions.value(command);
    if (action) {
        QVariant property = action->property(MENU_ACTION_PROPERTY);
        if (property.isValid() && property.toBool()) {
            QT_TRAP_THROWING(S60->menuBar()->TryDisplayMenuBarL());
        } else if (action->menu()) {
            // TODO: This is hack, in order to use exising QMenuBar implementation for Symbian
            // menubar needs to have widget to which it is associated. Since we want to associate
            // menubar to action (which is inherited from QObejct), we create and associate QWidget
            // to action and pass that for QMenuBar. This associates the menubar to action, and we
            // can have own menubar for each action.
            QWidget *actionContainer = action->property("_q_action_widget").value<QWidget*>();
            if(!actionContainer) {
                actionContainer = new QWidget(action->parentWidget());
                QMenuBar *menuBar = new QMenuBar(actionContainer);
                foreach(QAction *menuAction, action->menu()->actions()) {
                    QMenu *menu = menuAction->menu();
                    if(menu)
                        menuBar->addMenu(action->menu());
                    else
                        menuBar->addAction(menuAction);
                }
                QVariant v;
                v.setValue(actionContainer);
                action->setProperty("_q_action_widget", v);
            }
            qt_symbian_next_menu_from_action(actionContainer);
            QT_TRAP_THROWING(S60->menuBar()->TryDisplayMenuBarL());
            // TODO: hack remove, it can happen that IsDisplayingMenuOrDialog return false
            // in updateSoftKeys_sys, and we will override menu CBA with our own
            skipNextUpdate = true;
        } else {
            Q_ASSERT(action->softKeyRole() != QAction::NoSoftKey);
            QWidget *actionParent = action->parentWidget();
            Q_ASSERT_X(actionParent, Q_FUNC_INFO, "No parent set for softkey action!");
            if (actionParent->isEnabled()) {
                action->activate(QAction::Trigger);
                return true;
            }
        }
    }
    return false;
}

QT_END_NAMESPACE
#endif //QT_NO_SOFTKEYMANAGER
