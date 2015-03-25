/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
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

#ifndef QS60MAINAPPUI_H
#define QS60MAINAPPUI_H

#include <QtCore/qglobal.h>

#ifdef Q_OS_SYMBIAN

#ifdef Q_WS_S60
#include <aknappui.h>
typedef CAknAppUi QS60MainAppUiBase;
#else
#include <eikappui.h>
// these stub classes simulate the structure of CAknAppUi, to help binary compatibility between Qt configured with and without S60/Avkon
class QS60StubAknAppUiBase : public CEikAppUi
{
private:
    int qS60StubAknAppUiBaseSpace[4];
};

class QS60StubMEikStatusPaneObserver
{
public:
    virtual void HandleStatusPaneSizeChange() = 0;
};

class QS60StubMAknTouchPaneObserver
{
public:
    virtual void HandleTouchPaneSizeChange() = 0;
};

class QS60StubAknAppUi : public QS60StubAknAppUiBase, QS60StubMEikStatusPaneObserver,
            public MCoeViewDeactivationObserver,
            public QS60StubMAknTouchPaneObserver
{
public: // MCoeViewDeactivationObserver
    virtual void HandleViewDeactivation(const TVwsViewId&, const TVwsViewId &);

public: // from MAknTouchPaneObserver
    virtual void HandleTouchPaneSizeChange();

protected: // from MEikStatusPaneObserver
    virtual void HandleStatusPaneSizeChange();

protected: // from CAknAppUi
    virtual void Reserved_MtsmPosition();
    virtual void Reserved_MtsmObject();

private:
    int qS60StubAknAppUiSpace[4];
};

typedef QS60StubAknAppUi QS60MainAppUiBase;
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class Q_GUI_EXPORT QS60MainAppUi : public QS60MainAppUiBase
{
public:
    QS60MainAppUi();
    // The virtuals are for qdoc.
    virtual ~QS60MainAppUi();

    virtual void ConstructL();

    virtual void RestoreMenuL(CCoeControl *menuWindow,TInt resourceId,TMenuType menuType);
    virtual void DynInitMenuBarL(TInt resourceId, CEikMenuBar *menuBar);
    virtual void DynInitMenuPaneL(TInt resourceId, CEikMenuPane *menuPane);

    virtual void HandleCommandL( TInt command );

    virtual void HandleResourceChangeL(TInt type);

    virtual void HandleStatusPaneSizeChange();

protected:
    virtual void HandleWsEventL(const TWsEvent &event, CCoeControl *destination);

public:
    virtual void Exit();
    virtual void SetFadedL(TBool aFaded);
    virtual TRect ApplicationRect() const;
    virtual void ProcessCommandL(TInt aCommand);
    virtual TErrorHandlerResponse HandleError (TInt aError, const SExtendedError &aExtErr, TDes &aErrorText, TDes &aContextText);
    virtual void HandleViewDeactivation(const TVwsViewId &aViewIdToBeDeactivated, const TVwsViewId &aNewlyActivatedViewId);
    virtual void PrepareToExit();
    virtual void HandleTouchPaneSizeChange();
    virtual TBool ProcessCommandParametersL(TApaCommand aCommand, TFileName &aDocumentName, const TDesC8 &aTail);

protected:
    virtual void HandleScreenDeviceChangedL();
    virtual void HandleApplicationSpecificEventL(TInt aType, const TWsEvent &aEvent);
    virtual TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    virtual void HandleSystemEventL(const TWsEvent &aEvent);
    virtual void Reserved_MtsmPosition();
    virtual void Reserved_MtsmObject();
    virtual void HandleForegroundEventL(TBool aForeground);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q_OS_SYMBIAN

#endif // QS60MAINAPPUI_H
