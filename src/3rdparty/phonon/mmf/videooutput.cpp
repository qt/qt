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

#include <QPaintEvent>
#include <QMoveEvent>
#include <QResizeEvent>

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoOutput::VideoOutput(QWidget* parent)
					:	QWidget(parent)
{
	TRACE_CONTEXT(VideoOutput::VideoOutput, EVideoInternal);
	TRACE_ENTRY("parent 0x%08x", parent);
	
	setPalette(QPalette(Qt::black));
	setAttribute(Qt::WA_OpaquePaintEvent, true);
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAutoFillBackground(false);
	
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
    TRACE_CONTEXT(VideoOutput::setFrameSize, EVideoInternal);
    TRACE("oldSize %d %d newSize %d %d",
        m_frameSize.width(), m_frameSize.height(),
        frameSize.width(), frameSize.height());
    
    if(frameSize != m_frameSize)
    {
        m_frameSize = frameSize;
        updateGeometry();
    }
}


//-----------------------------------------------------------------------------
// QWidget
//-----------------------------------------------------------------------------

QSize MMF::VideoOutput::sizeHint() const
{
    if(m_frameSize.isNull())
    {
        // TODO: replace this with a more sensible default
        return QSize(320, 240);
    }
    else
    {
        return m_frameSize;
    }
}

void MMF::VideoOutput::paintEvent(QPaintEvent* event)
{
	TRACE_CONTEXT(VideoOutput::paintEvent, EVideoInternal);
	TRACE("rect %d %d - %d %d",
		event->rect().left(), event->rect().top(),
		event->rect().right(), event->rect().bottom());
	TRACE("regions %d", event->region().numRects());
	TRACE("type %d", event->type());
}

QPaintEngine* MMF::VideoOutput::paintEngine() const
{
	return NULL;
}

void MMF::VideoOutput::resizeEvent(QResizeEvent* event)
{
	TRACE_CONTEXT(VideoOutput::resizeEvent, EVideoInternal);
	TRACE("%d %d -> %d %d", 
		event->oldSize().width(), event->oldSize().height(),
		event->size().width(), event->size().height());
}

void MMF::VideoOutput::moveEvent(QMoveEvent* event)
{
	TRACE_CONTEXT(VideoOutput::moveEvent, EVideoInternal);
	TRACE("%d %d -> %d %d", 
		event->oldPos().x(), event->oldPos().y(),
		event->pos().x(), event->pos().y());
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------



