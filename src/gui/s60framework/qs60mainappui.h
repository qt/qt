/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
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

#ifndef QS60MAINAPPUI_H
#define QS60MAINAPPUI_H

#include <QtCore/qglobal.h>

#ifdef Q_WS_S60

#include <aknappui.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class Q_GUI_EXPORT QS60MainAppUi : public CAknAppUi
{
public:
    QS60MainAppUi();
    // The virtuals are for qdoc.
    virtual ~QS60MainAppUi();

    virtual void ConstructL();

    virtual void RestoreMenuL(CCoeControl* menuWindow,TInt resourceId,TMenuType menuType);
    virtual void DynInitMenuBarL(TInt resourceId, CEikMenuBar *menuBar);
    virtual void DynInitMenuPaneL(TInt resourceId, CEikMenuPane *menuPane);

    virtual void HandleCommandL( TInt command );

    virtual void HandleResourceChangeL(TInt type);

    virtual void HandleStatusPaneSizeChange();

protected:
    virtual void HandleWsEventL(const TWsEvent& event, CCoeControl* destination);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q_WS_S60

#endif // QS60MAINAPPUI_H
