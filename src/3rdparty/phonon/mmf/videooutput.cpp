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

#include "utils.h"
#include "videooutput.h"
#include "videooutputobserver.h"

#ifdef _DEBUG
#include "mmfphonondebug/objectdump.h"
#endif

#include <QPaintEvent>
#include <QPainter>
#include <QMoveEvent>
#include <QResizeEvent>

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoOutput::VideoOutput(QWidget* parent)
        :   QWidget(parent)
        ,   m_observer(NULL)
{
    TRACE_CONTEXT(VideoOutput::VideoOutput, EVideoInternal);
    TRACE_ENTRY("parent 0x%08x", parent);

#ifndef PHONON_MMF_VIDEOOUTPUT_IS_QWIDGET
    setPalette(QPalette(Qt::black));
    //setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);
#endif // PHONON_MMF_VIDEOOUTPUT_IS_QWIDGET

    dump();
    
    TRACE_EXIT_0();
}

MMF::VideoOutput::~VideoOutput()
{
    TRACE_CONTEXT(VideoOutput::~VideoOutput, EVideoInternal);
    TRACE_ENTRY_0();

    TRACE_EXIT_0();
}

void MMF::VideoOutput::setFrameSize(const QSize& frameSize)
{
#ifdef PHONON_MMF_VIDEOOUTPUT_IS_QWIDGET
    Q_UNUSED(frameSize);
#else
    TRACE_CONTEXT(VideoOutput::setFrameSize, EVideoInternal);
    TRACE("oldSize %d %d newSize %d %d",
          m_frameSize.width(), m_frameSize.height(),
          frameSize.width(), frameSize.height());

    if (frameSize != m_frameSize) {
        m_frameSize = frameSize;
        updateGeometry();
    }
#endif // PHONON_MMF_VIDEOOUTPUT_IS_QWIDGET
}

void MMF::VideoOutput::setObserver(VideoOutputObserver* observer)
{
    TRACE_CONTEXT(VideoOutput::setObserver, EVideoInternal);
    TRACE("observer 0x%08x", observer);

    m_observer = observer;
}


//-----------------------------------------------------------------------------
// QWidget
//-----------------------------------------------------------------------------

#ifndef PHONON_MMF_VIDEOOUTPUT_IS_QWIDGET

QSize MMF::VideoOutput::sizeHint() const
{
    // TODO: replace this with a more sensible default
    QSize result(320, 240);

    if (!m_frameSize.isNull()) {
        result = m_frameSize;
    }

    return result;
}

void MMF::VideoOutput::paintEvent(QPaintEvent* event)
{
    TRACE_CONTEXT(VideoOutput::paintEvent, EVideoInternal);
    TRACE("rect %d %d - %d %d",
          event->rect().left(), event->rect().top(),
          event->rect().right(), event->rect().bottom());
    TRACE("regions %d", event->region().numRects());
    TRACE("type %d", event->type());

    dump();
    
/*
    QPainter painter;
    painter.begin(this);
    painter.setBrush(QColor(0, 0, 0, 255));   // opaque black
    painter.drawRects(event->region().rects());
    painter.end();
*/
}

void MMF::VideoOutput::resizeEvent(QResizeEvent* event)
{
    TRACE_CONTEXT(VideoOutput::resizeEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldSize().width(), event->oldSize().height(),
          event->size().width(), event->size().height());

    QWidget::resizeEvent(event);

    if (m_observer)
        m_observer->videoOutputRegionChanged();
}

void MMF::VideoOutput::moveEvent(QMoveEvent* event)
{
    TRACE_CONTEXT(VideoOutput::moveEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldPos().x(), event->oldPos().y(),
          event->pos().x(), event->pos().y());

    QWidget::moveEvent(event);

    if (m_observer)
        m_observer->videoOutputRegionChanged();
}

#endif // PHONON_MMF_VIDEOOUTPUT_IS_QWIDGET


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void VideoOutput::dump() const
{
#ifdef _DEBUG
    TRACE_CONTEXT(VideoOutput::dump, EVideoInternal);
    QScopedPointer<ObjectDump::QVisitor> visitor(new ObjectDump::QVisitor);
    visitor->setPrefix("Phonon::MMF"); // to aid searchability of logs
    ObjectDump::addDefaultAnnotators(*visitor);
    TRACE("Dumping tree from leaf 0x%08x:", this);
    //ObjectDump::dumpAncestors(*this, *visitor);
    ObjectDump::dumpTreeFromLeaf(*this, *visitor);
#endif
}


QT_END_NAMESPACE

