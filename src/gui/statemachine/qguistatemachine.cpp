 /****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifdef QT_STATEMACHINE_SOLUTION
#include "qstatemachine.h"
#include "qstatemachine_p.h"
#else
#include <QtCore/qstatemachine.h>
#include <private/qstatemachine_p.h>
#endif
#include <QtGui/qevent.h>

QT_BEGIN_NAMESPACE

Q_CORE_EXPORT const QStateMachinePrivate::Handler *qcoreStateMachineHandler();

static QEvent *cloneEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        return new QMouseEvent(*static_cast<QMouseEvent*>(e));
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        return new QKeyEvent(*static_cast<QKeyEvent*>(e));
    case QEvent::FocusIn:
    case QEvent::FocusOut:
        return new QFocusEvent(*static_cast<QFocusEvent*>(e));
    case QEvent::Enter:
        return new QEvent(*e);
    case QEvent::Leave:
        return new QEvent(*e);
        break;
    case QEvent::Paint:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::Move:
        return new QMoveEvent(*static_cast<QMoveEvent*>(e));
    case QEvent::Resize:
        return new QResizeEvent(*static_cast<QResizeEvent*>(e));
    case QEvent::Create:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::Destroy:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::Show:
        return new QShowEvent(*static_cast<QShowEvent*>(e));
    case QEvent::Hide:
        return new QHideEvent(*static_cast<QHideEvent*>(e));
    case QEvent::Close:
        return new QCloseEvent(*static_cast<QCloseEvent*>(e));
    case QEvent::Quit:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ParentChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ParentAboutToChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ThreadChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::WindowActivate:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::WindowDeactivate:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ShowToParent:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::HideToParent:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::Wheel:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::WindowTitleChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::WindowIconChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ApplicationWindowIconChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ApplicationFontChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ApplicationLayoutDirectionChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ApplicationPaletteChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::PaletteChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::Clipboard:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::Speech:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::MetaCall:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::SockAct:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::WinEventAct:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::DeferredDelete:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::DragEnter:
        return new QDragEnterEvent(*static_cast<QDragEnterEvent*>(e));
    case QEvent::DragMove:
        return new QDragMoveEvent(*static_cast<QDragMoveEvent*>(e));
    case QEvent::DragLeave:
        return new QDragLeaveEvent(*static_cast<QDragLeaveEvent*>(e));
    case QEvent::Drop:
        return new QDropEvent(*static_cast<QDragMoveEvent*>(e));
    case QEvent::DragResponse:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ChildAdded:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ChildPolished:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
#ifdef QT3_SUPPORT
    case QEvent::ChildInsertedRequest:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ChildInserted:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::LayoutHint:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
#endif
    case QEvent::ChildRemoved:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ShowWindowRequest:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::PolishRequest:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::Polish:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::LayoutRequest:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::UpdateRequest:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::UpdateLater:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::EmbeddingControl:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ActivateControl:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::DeactivateControl:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ContextMenu:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::InputMethod:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::AccessibilityPrepare:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::TabletMove:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::LocaleChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::LanguageChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::LayoutDirectionChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::Style:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::TabletPress:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::TabletRelease:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::OkRequest:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::HelpRequest:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::IconDrag:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::FontChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::EnabledChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ActivationChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::StyleChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::IconTextChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ModifiedChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::MouseTrackingChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::WindowBlocked:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::WindowUnblocked:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::WindowStateChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::ToolTip:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::WhatsThis:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::StatusTip:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::ActionChanged:
    case QEvent::ActionAdded:
    case QEvent::ActionRemoved:
        return new QActionEvent(*static_cast<QActionEvent*>(e));

    case QEvent::FileOpen:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::Shortcut:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ShortcutOverride:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

#ifdef QT3_SUPPORT
    case QEvent::Accel:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::AccelAvailable:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
#endif

    case QEvent::WhatsThisClicked:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::ToolBarChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::ApplicationActivate:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ApplicationDeactivate:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::QueryWhatsThis:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::EnterWhatsThisMode:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::LeaveWhatsThisMode:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::ZOrderChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::HoverEnter:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::HoverLeave:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::HoverMove:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::AccessibilityHelp:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::AccessibilityDescription:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

#ifdef QT_KEYPAD_NAVIGATION
    case QEvent::EnterEditFocus:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::LeaveEditFocus:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
#endif
    case QEvent::AcceptDropsChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::MenubarUpdated:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::ZeroTimerEvent:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::GraphicsSceneMouseMove:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneMousePress:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneMouseRelease:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneMouseDoubleClick:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneContextMenu:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneHoverEnter:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneHoverMove:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneHoverLeave:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneHelp:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneDragEnter:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneDragMove:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneDragLeave:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneDrop:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneWheel:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::KeyboardLayoutChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::DynamicPropertyChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::TabletEnterProximity:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::TabletLeaveProximity:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::NonClientAreaMouseMove:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::NonClientAreaMouseButtonPress:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::NonClientAreaMouseButtonRelease:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::NonClientAreaMouseButtonDblClick:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::MacSizeChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::ContentsRectChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::MacGLWindowChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::FutureCallOut:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::GraphicsSceneResize:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GraphicsSceneMove:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::CursorChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::ToolTipChange:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::NetworkReplyUpdated:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;

    case QEvent::GrabMouse:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::UngrabMouse:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::GrabKeyboard:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    case QEvent::UngrabKeyboard:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
#ifdef QT_MAC_USE_COCOA
    case QEvent::CocoaRequestModal:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
#endif
    case QEvent::User:
    case QEvent::MaxUser:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    default:
        ;
    }
    return qcoreStateMachineHandler()->cloneEvent(e);
}

const QStateMachinePrivate::Handler qt_gui_statemachine_handler = {
    cloneEvent
};

static const QStateMachinePrivate::Handler *qt_guistatemachine_last_handler = 0;
int qRegisterGuiStateMachine()
{
    qt_guistatemachine_last_handler = QStateMachinePrivate::handler;
    QStateMachinePrivate::handler = &qt_gui_statemachine_handler;
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(qRegisterGuiStateMachine)

int qUnregisterGuiStateMachine()
{
    QStateMachinePrivate::handler = qt_guistatemachine_last_handler;
    return 1;
}
Q_DESTRUCTOR_FUNCTION(qUnregisterGuiStateMachine)

QT_END_NAMESPACE
