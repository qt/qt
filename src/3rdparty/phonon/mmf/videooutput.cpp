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
#include "objectdump.h"
#endif

#include <QPaintEvent>
#include <QPainter>
#include <QMoveEvent>
#include <QResizeEvent>

// Required for implementation of transparentFill
#include <QtGui/private/qwidget_p.h>
#include <QtGui/private/qdrawhelper_p.h>
#include <QtGui/private/qwindowsurface_p.h>
#include <QImage>


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
	setAttribute(Qt::WA_OpaquePaintEvent, true);
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
    
#ifdef PHONON_MMF_DIRECT_WRITE_ALPHA
    transparentFill(event->region().rects());
#else
    // Note: composition mode code was a failed attempt to get transparent
    // alpha values to be propagated to the (EColor16MU) window surface.

    QPainter painter;
    //const QPainter::CompositionMode compositionMode = painter.compositionMode();
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.begin(this);
    painter.setBrush(QColor(0, 0, 0, 0));
    painter.drawRects(event->region().rects());
    painter.end();
    //painter.setCompositionMode(compositionMode);
#endif
}

void MMF::VideoOutput::transparentFill(const QVector<QRect>& rects)
{
	TRACE_CONTEXT(VideoOutput::transparentFill, EVideoInternal);
	TRACE_ENTRY_0();
	
	QImage *image = window()->windowSurface()->buffer(window());
	QRgb *data = reinterpret_cast<QRgb *>(image->bits());
	const int row_stride = image->bytesPerLine() / 4;

	for (QVector<QRect>::const_iterator it = rects.begin(); it != rects.end(); ++it) {
	
		const QRect& rect = *it;
		
		TRACE("%d %d size %d x %d", rect.x(), rect.y(), rect.width(), rect.height());
	
		const int x_start = rect.x();
		const int width = rect.width();

		const int y_start = rect.y();
		const int height = rect.height();

		QRgb *row = data + row_stride * y_start;
		for (int y = 0; y < height; ++y) {
		
			// Note: not using the optimised qt_memfill function implemented in
			// gui/painting/qdrawhelper.cpp - can we somehow link against this?
		
			//qt_memfill(row + x_start, 0U, width);
			memset(row + x_start, 0, width*4);
			row += row_stride;
		}
	}
	
	TRACE_EXIT_0();
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

