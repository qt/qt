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
	
#ifdef PHONON_MMF_DEBUG_VIDEO_OUTPUT
	dump();
#endif
	
	TRACE_EXIT_0();
}

#ifdef PHONON_MMF_DEBUG_VIDEO_OUTPUT
void VideoOutput::dump()
{
    TRACE_CONTEXT(VideoOutput::dump, EVideoInternal);
    TRACE_ENTRY_0();
    
    TRACE("dumpObjectInfo this 0x%08x", this);
    this->dumpObjectInfo();

    TRACE_0("Traversing up object tree ...");
    QObject* node = this;
    QObject* root = this;
    while(node)
    {
        QWidget* widget = qobject_cast<QWidget*>(node);
        const bool visible = widget ? widget->isVisible() : false;
        const QHBufC name(node->objectName());
        TRACE("node 0x%08x name %S widget 0x%08x visible %d", node, name.data(), widget, visible);
        
        root = node;
        node = node->parent();
    }
    
    TRACE("dumpObjectInfo root 0x%08x", root);
    root->dumpObjectInfo();
    TRACE_0("+ dumpObjectTree");
    root->dumpObjectTree();
    TRACE_0("- dumpObjectTree");
    
    TRACE("isVisible %d", isVisible());
    TRACE("pos %d %d", x(), y());
    TRACE("size %d %d", size().width(), size().height());
    TRACE("maxSize %d %d", maximumWidth(), maximumHeight());
    TRACE("sizeHint %d %d", sizeHint().width(), sizeHint().height());
    
    QWidget& parentWidget = *qobject_cast<QWidget*>(parent());
    TRACE("parent.isVisible %d", parentWidget.isVisible());
    TRACE("parent.pos %d %d", parentWidget.x(), parentWidget.y());
    TRACE("parent.size %d %d", parentWidget.size().width(), parentWidget.size().height());
    TRACE("parent.maxSize %d %d", parentWidget.maximumWidth(), parentWidget.maximumHeight());
    TRACE("parent.sizeHint %d %d", parentWidget.sizeHint().width(), parentWidget.sizeHint().height());
    
    TRACE_EXIT_0();
}
#endif // PHONON_MMF_DEBUG_VIDEO_OUTPUT

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
    
    if(frameSize != m_frameSize)
    {
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
    TRACE_CONTEXT(VideoOutput::sizeHint, EVideoApi);
    
    // TODO: replace this with a more sensible default
    QSize result(320, 240);
    
    if(!m_frameSize.isNull())
    {
        result = m_frameSize;
    }
    
    TRACE("  result %d %d", result.width(), result.height());
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
	
	QWidget::paintEvent(event);
}

QPaintEngine* MMF::VideoOutput::paintEngine() const
{
    TRACE_CONTEXT(VideoOutput::sizeHint, EVideoApi);
    
    QPaintEngine* const engine = QWidget::paintEngine();

    TRACE_RETURN("0x%08x", engine);
}

void MMF::VideoOutput::resizeEvent(QResizeEvent* event)
{
	TRACE_CONTEXT(VideoOutput::resizeEvent, EVideoInternal);
	TRACE("%d %d -> %d %d", 
		event->oldSize().width(), event->oldSize().height(),
		event->size().width(), event->size().height());
	
	QWidget::resizeEvent(event);

	if(m_observer)
	{
	    m_observer->videoOutputRegionChanged();
	}
}

void MMF::VideoOutput::moveEvent(QMoveEvent* event)
{
	TRACE_CONTEXT(VideoOutput::moveEvent, EVideoInternal);
	TRACE("%d %d -> %d %d", 
		event->oldPos().x(), event->oldPos().y(),
		event->pos().x(), event->pos().y());
	
	QWidget::moveEvent(event);
	
	if(m_observer)
    {
        m_observer->videoOutputRegionChanged();
    }
}

#endif // PHONON_MMF_VIDEOOUTPUT_IS_QWIDGET


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------



