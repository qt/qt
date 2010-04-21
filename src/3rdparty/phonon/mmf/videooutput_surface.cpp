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

#include <QResizeEvent>

#include <QtCore/private/qcore_symbian_p.h> // for qt_TRect2QRect
#include <QtGui/private/qwidget_p.h> // to access QWExtra

#include "utils.h"
#include "videooutput_surface.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

SurfaceVideoOutput::SurfaceVideoOutput(QWidget *parent)
    :   AbstractVideoOutput(parent)
{
    TRACE_CONTEXT(SurfaceVideoOutput::SurfaceVideoOutput, EVideoInternal);
    TRACE_ENTRY("parent 0x%08x", parent);

    qt_widget_private(this)->createExtra();
    qt_widget_private(this)->extraData()->nativePaintMode = QWExtra::Disable;

    TRACE_EXIT_0();
}

SurfaceVideoOutput::~SurfaceVideoOutput()
{
    TRACE_CONTEXT(SurfaceVideoOutput::~SurfaceVideoOutput, EVideoInternal);
    TRACE_ENTRY_0();

    TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MMF::SurfaceVideoOutput::resizeEvent(QResizeEvent *event)
{
    TRACE_CONTEXT(SurfaceVideoOutput::resizeEvent, EVideoInternal);
    TRACE("%d %d -> %d %d",
          event->oldSize().width(), event->oldSize().height(),
          event->size().width(), event->size().height());

    if (event->size() != event->oldSize())
        emit videoWindowSizeChanged();
}

bool MMF::SurfaceVideoOutput::event(QEvent *event)
{
    TRACE_CONTEXT(SurfaceVideoOutput::event, EVideoInternal);

    if (event->type() == QEvent::WinIdChange) {
        TRACE_0("WinIdChange");
        emit videoWindowChanged();
        return true;
    } else {
        return QWidget::event(event);
    }
}


QT_END_NAMESPACE

