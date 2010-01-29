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

#include "ancestormovemonitor.h"
#include "utils.h"
#include "videooutput.h"

#ifndef QT_NO_DEBUG
#include "objectdump.h"
#endif

#include <QPaintEvent>
#include <QPainter>
#include <QMoveEvent>
#include <QResizeEvent>

#include <QtCore/private/qcore_symbian_p.h> // for qt_TRect2QRect
#include <QtGui/private/qwidget_p.h> // to access QWExtra

#include <coecntrl.h>

#include <coemain.h>    // for CCoeEnv

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::VideoOutput
  \internal
*/

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

static const Phonon::VideoWidget::AspectRatio DefaultAspectRatio =
    Phonon::VideoWidget::AspectRatioAuto;
static const Phonon::VideoWidget::ScaleMode DefaultScaleMode =
    Phonon::VideoWidget::FitInView;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoOutput::VideoOutput
    (AncestorMoveMonitor* ancestorMoveMonitor, QWidget* parent)
        :   QWidget(parent)
        ,   m_ancestorMoveMonitor(ancestorMoveMonitor)
        ,   m_aspectRatio(DefaultAspectRatio)
        ,   m_scaleMode(DefaultScaleMode)
{
    TRACE_CONTEXT(VideoOutput::VideoOutput, EVideoInternal);
    TRACE_ENTRY("parent 0x%08x", parent);

    setPalette(QPalette(Qt::black));
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);

    qt_widget_private(this)->extraData()->nativePaintMode = QWExtra::ZeroFill;
    qt_widget_private(this)->extraData()->receiveNativePaintEvents = true;

    getVideoWindowRect();
    registerForAncestorMoved();

    dump();

    TRACE_EXIT_0();
}

MMF::VideoOutput::~VideoOutput()
{
    TRACE_CONTEXT(VideoOutput::~VideoOutput, EVideoInternal);
    TRACE_ENTRY_0();

    m_ancestorMoveMonitor->unRegisterTarget(this);

    TRACE_EXIT_0();
}

void MMF::VideoOutput::setVideoSize(const QSize& frameSize)
{
    TRACE_CONTEXT(VideoOutput::setVideoSize, EVideoInternal);
    TRACE("oldSize %d %d newSize %d %d",
          m_videoFrameSize.width(), m_videoFrameSize.height(),
          frameSize.width(), frameSize.height());

    if (frameSize != m_videoFrameSize) {
        m_videoFrameSize = frameSize;
        updateGeometry();
    }
}

void MMF::VideoOutput::ancestorMoved()
{
    TRACE_CONTEXT(VideoOutput::ancestorMoved, EVideoInternal);
    TRACE_ENTRY_0();

    RWindowBase *const window = videoWindow();

    if(window) {
        const TPoint newWindowPosSymbian = window->AbsPosition();
        const QPoint newWindowPos(newWindowPosSymbian.iX, newWindowPosSymbian.iY);

        if(newWindowPos != m_videoWindowRect.topLeft()) {
            m_videoWindowRect.moveTo(newWindowPos);
            emit videoWindowChanged();
        }
    }

    TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// QWidget
//-----------------------------------------------------------------------------

QSize MMF::VideoOutput::sizeHint() const
{
    // TODO: replace this with a more sensible default
    QSize result(320, 240);

    if (!m_videoFrameSize.isNull())
        result = m_videoFrameSize;

    return result;
}

void MMF::VideoOutput::paintEvent(QPaintEvent* event)
{
    TRACE_CONTEXT(VideoOutput::paintEvent, EVideoInternal);
    TRACE("rect %d %d - %d %d",
          event->rect().left(), event->rect().top(),
          event->rect().right(), event->rect().bottom());
    TRACE("regions %d", event->region().rectCount());
    TRACE("type %d", event->type());

    // Do nothing
}

void MMF::VideoOutput::resizeEvent(QResizeEvent* event)
{
    TRACE_CONTEXT(VideoOutput::resizeEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldSize().width(), event->oldSize().height(),
          event->size().width(), event->size().height());

    if(event->size() != event->oldSize()) {
        m_videoWindowRect.setSize(event->size());
        emit videoWindowChanged();
    }
}

void MMF::VideoOutput::moveEvent(QMoveEvent* event)
{
    TRACE_CONTEXT(VideoOutput::moveEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldPos().x(), event->oldPos().y(),
          event->pos().x(), event->pos().y());

    if(event->pos() != event->oldPos()) {
        m_videoWindowRect.moveTo(event->pos());
        emit videoWindowChanged();
    }
}

bool MMF::VideoOutput::event(QEvent* event)
{
    TRACE_CONTEXT(VideoOutput::event, EVideoInternal);

    if (event->type() == QEvent::WinIdChange) {
        TRACE_0("WinIdChange");
        getVideoWindowRect();
        emit videoWindowChanged();
        return true;
    } else if (event->type() == QEvent::ParentChange) {
        // Tell ancestor move monitor to update ancestor list for this widget
        registerForAncestorMoved();
        return true;
    } else
        return QWidget::event(event);
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

RWindowBase* MMF::VideoOutput::videoWindow()
{
    CCoeControl *control = internalWinId();
    if(!control)
        control = effectiveWinId();

    RWindowBase *window = 0;
    if(control)
        window = control->DrawableWindow();

    return window;
}

const QRect& MMF::VideoOutput::videoWindowRect() const
{
    return m_videoWindowRect;
}

Phonon::VideoWidget::AspectRatio MMF::VideoOutput::aspectRatio() const
{
    return m_aspectRatio;
}

void MMF::VideoOutput::setAspectRatio
    (Phonon::VideoWidget::AspectRatio aspectRatio)
{
    if(m_aspectRatio != aspectRatio) {
        m_aspectRatio = aspectRatio;
        emit aspectRatioChanged();
    }
}

Phonon::VideoWidget::ScaleMode MMF::VideoOutput::scaleMode() const
{
    return m_scaleMode;
}

void MMF::VideoOutput::setScaleMode
    (Phonon::VideoWidget::ScaleMode scaleMode)
{
    if(m_scaleMode != scaleMode) {
        m_scaleMode = scaleMode;
        emit scaleModeChanged();
    }
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MMF::VideoOutput::getVideoWindowRect()
{
    RWindowBase *const window = videoWindow();
    if(window)
        m_videoWindowRect =
            qt_TRect2QRect(TRect(window->AbsPosition(), window->Size()));
}

void MMF::VideoOutput::registerForAncestorMoved()
{
    m_ancestorMoveMonitor->registerTarget(this);
}

void MMF::VideoOutput::dump() const
{
#ifndef QT_NO_DEBUG
    TRACE_CONTEXT(VideoOutput::dump, EVideoInternal);

    QScopedPointer<ObjectDump::QVisitor> visitor(new ObjectDump::QVisitor);
    visitor->setPrefix("Phonon::MMF"); // to aid searchability of logs
    ObjectDump::addDefaultAnnotators(*visitor);
    TRACE("Dumping tree from leaf 0x%08x:", this);
    ObjectDump::dumpTreeFromLeaf(*this, *visitor);

    QScopedPointer<ObjectDump::QDumper> dumper(new ObjectDump::QDumper);
    dumper->setPrefix("Phonon::MMF"); // to aid searchability of logs
    ObjectDump::addDefaultAnnotators(*dumper);
    TRACE_0("Dumping VideoOutput:");
    dumper->dumpObject(*this);
#endif
}

void MMF::VideoOutput::beginNativePaintEvent(const QRect& /*controlRect*/)
{
    emit beginVideoWindowNativePaint();
}

void MMF::VideoOutput::endNativePaintEvent(const QRect& /*controlRect*/)
{
    // Ensure that draw ops are executed into the WSERV output framebuffer
    CCoeEnv::Static()->WsSession().Flush();

    emit endVideoWindowNativePaint();
}

QT_END_NAMESPACE

