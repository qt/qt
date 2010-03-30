/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QtCore/private/qcore_symbian_p.h> // for qt_TRect2QRect
#include <QtGui/private/qwidget_p.h> // to access QWExtra
#include <QResizeEvent>
#include <QMoveEvent>

#include <coemain.h> // for CCoeEnv

#include "ancestormovemonitor.h"
#include "utils.h"
#include "videooutput_dsa.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

DsaVideoOutput::DsaVideoOutput(QWidget *parent)
    :   AbstractVideoOutput(parent)
    ,   m_ancestorMoveMonitor(0)
{
    TRACE_CONTEXT(DsaVideoOutput::DsaVideoOutput, EVideoInternal);
    TRACE_ENTRY("parent 0x%08x", parent);

    setPalette(QPalette(Qt::black));
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);

    qt_widget_private(this)->extraData()->nativePaintMode = QWExtra::ZeroFill;
    qt_widget_private(this)->extraData()->receiveNativePaintEvents = true;

    getVideoWindowScreenRect();

    dump();

    TRACE_EXIT_0();
}

DsaVideoOutput::~DsaVideoOutput()
{
    TRACE_CONTEXT(DsaVideoOutput::~DsaVideoOutput, EVideoInternal);
    TRACE_ENTRY_0();

    m_ancestorMoveMonitor->unRegisterTarget(this);

    TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

void MMF::DsaVideoOutput::setAncestorMoveMonitor(AncestorMoveMonitor *monitor)
{
    m_ancestorMoveMonitor = monitor;
    registerForAncestorMoved();
}

const QRect& MMF::DsaVideoOutput::videoWindowScreenRect() const
{
    return m_videoWindowScreenRect;
}

void MMF::DsaVideoOutput::ancestorMoved()
{
    TRACE_CONTEXT(DsaVideoOutput::ancestorMoved, EVideoInternal);
    TRACE_ENTRY_0();

    RWindowBase *const window = videoWindow();

    if (window) {
        const TPoint newWindowPosSymbian = window->AbsPosition();
        const QPoint newWindowPos(newWindowPosSymbian.iX, newWindowPosSymbian.iY);

        if (newWindowPos != m_videoWindowScreenRect.topLeft()) {
            m_videoWindowScreenRect.moveTo(newWindowPos);
            emit videoWindowScreenRectChanged();
        }
    }

    TRACE_EXIT_0();
}

void MMF::DsaVideoOutput::beginNativePaintEvent(const QRect & /*controlRect*/)
{
    TRACE_CONTEXT(DsaVideoOutput::beginNativePaintEvent, EVideoInternal);
    TRACE_ENTRY_0();

    emit beginVideoWindowNativePaint();
}

void MMF::DsaVideoOutput::endNativePaintEvent(const QRect & /*controlRect*/)
{
    TRACE_CONTEXT(DsaVideoOutput::endNativePaintEvent, EVideoInternal);
    TRACE_ENTRY_0();

    // Ensure that draw ops are executed into the WSERV output framebuffer
    CCoeEnv::Static()->WsSession().Flush();

    emit endVideoWindowNativePaint();
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MMF::DsaVideoOutput::getVideoWindowScreenRect()
{
    RWindowBase *const window = videoWindow();
    if (window)
        m_videoWindowScreenRect =
            qt_TRect2QRect(TRect(window->AbsPosition(), window->Size()));
}

void MMF::DsaVideoOutput::registerForAncestorMoved()
{
    m_ancestorMoveMonitor->registerTarget(this);
}

void MMF::DsaVideoOutput::resizeEvent(QResizeEvent *event)
{
    TRACE_CONTEXT(DsaVideoOutput::resizeEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldSize().width(), event->oldSize().height(),
          event->size().width(), event->size().height());

    if (event->size() != event->oldSize()) {
        m_videoWindowScreenRect.setSize(event->size());
        emit videoWindowScreenRectChanged();
    }
}

void MMF::DsaVideoOutput::moveEvent(QMoveEvent *event)
{
    TRACE_CONTEXT(DsaVideoOutput::moveEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldPos().x(), event->oldPos().y(),
          event->pos().x(), event->pos().y());

    if (event->pos() != event->oldPos()) {
        m_videoWindowScreenRect.moveTo(event->pos());
        emit videoWindowScreenRectChanged();
    }
}

bool MMF::DsaVideoOutput::event(QEvent *event)
{
    TRACE_CONTEXT(DsaVideoOutput::event, EVideoInternal);

    if (event->type() == QEvent::WinIdChange) {
        TRACE_0("WinIdChange");
        getVideoWindowScreenRect();
        emit videoWindowChanged();
        return true;
    } else if (event->type() == QEvent::ParentChange) {
        // Tell ancestor move monitor to update ancestor list for this widget
        registerForAncestorMoved();
        return true;
    } else {
        return QWidget::event(event);
    }
}

QT_END_NAMESPACE

