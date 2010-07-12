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

#include "abstractvideooutput.h"
#include "utils.h"

#ifndef QT_NO_DEBUG
#include "objectdump.h"
#endif

#include <QtCore/private/qcore_symbian_p.h> // for qt_TSize2QSize

#include <QMoveEvent>
#include <QResizeEvent>

#include <QApplication> // for QApplication::activeWindow

#include <coecntrl.h>

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AbstractVideoOutput
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

MMF::AbstractVideoOutput::AbstractVideoOutput(QWidget *parent)
    :   QWidget(parent)
    ,   m_aspectRatio(DefaultAspectRatio)
    ,   m_scaleMode(DefaultScaleMode)
{
    // Ensure that this widget has a native window handle
    winId();
}

MMF::AbstractVideoOutput::~AbstractVideoOutput()
{

}

void MMF::AbstractVideoOutput::setVideoSize(const QSize &frameSize)
{
    TRACE_CONTEXT(AbstractVideoOutput::setVideoSize, EVideoInternal);
    TRACE("oldSize %d %d newSize %d %d",
          m_videoFrameSize.width(), m_videoFrameSize.height(),
          frameSize.width(), frameSize.height());

    if (frameSize != m_videoFrameSize) {
        m_videoFrameSize = frameSize;
        updateGeometry();
    }
}


//-----------------------------------------------------------------------------
// QWidget
//-----------------------------------------------------------------------------

QSize MMF::AbstractVideoOutput::sizeHint() const
{
    // TODO: replace this with a more sensible default
    QSize result(320, 240);

    if (!m_videoFrameSize.isNull())
        result = m_videoFrameSize;

    return result;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

RWindowBase* MMF::AbstractVideoOutput::videoWindow() const
{
    CCoeControl *control = internalWinId();
    if (!control)
        control = effectiveWinId();

    RWindowBase *window = 0;
    if (control)
        window = control->DrawableWindow();

    return window;
}

QSize MMF::AbstractVideoOutput::videoWindowSize() const
{
    QSize result;
    if (RWindowBase *const window = videoWindow())
        result = qt_TSize2QSize(window->Size());
    return result;
}

Phonon::VideoWidget::AspectRatio MMF::AbstractVideoOutput::aspectRatio() const
{
    return m_aspectRatio;
}

void MMF::AbstractVideoOutput::setAspectRatio
    (Phonon::VideoWidget::AspectRatio aspectRatio)
{
    if (m_aspectRatio != aspectRatio) {
        m_aspectRatio = aspectRatio;
        emit aspectRatioChanged();
    }
}

Phonon::VideoWidget::ScaleMode MMF::AbstractVideoOutput::scaleMode() const
{
    return m_scaleMode;
}

void MMF::AbstractVideoOutput::setScaleMode
    (Phonon::VideoWidget::ScaleMode scaleMode)
{
    if (m_scaleMode != scaleMode) {
        m_scaleMode = scaleMode;
        emit scaleModeChanged();
    }
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MMF::AbstractVideoOutput::dump() const
{
#ifndef QT_NO_DEBUG
    TRACE_CONTEXT(AbstractVideoOutput::dump, EVideoInternal);

    QScopedPointer<ObjectDump::QVisitor> visitor(new ObjectDump::QVisitor);
    visitor->setPrefix("Phonon::MMF"); // to aid searchability of logs
    ObjectDump::addDefaultAnnotators(*visitor);

    if (QWidget *window = QApplication::activeWindow()) {
        TRACE("Dumping from root window 0x%08x:", window);
        ObjectDump::dumpTreeFromLeaf(*window, *visitor);
    }

    TRACE("Dumping tree from leaf 0x%08x:", this);
    ObjectDump::dumpTreeFromLeaf(*this, *visitor);

    QScopedPointer<ObjectDump::QDumper> dumper(new ObjectDump::QDumper);
    dumper->setPrefix("Phonon::MMF"); // to aid searchability of logs
    ObjectDump::addDefaultAnnotators(*dumper);
    TRACE_0("Dumping AbstractVideoOutput:");
    dumper->dumpObject(*this);
#endif
}

QT_END_NAMESPACE

