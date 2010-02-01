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

#ifndef PHONON_MMF_VIDEOOUTPUT_H
#define PHONON_MMF_VIDEOOUTPUT_H

#include <QtGui/QWidget>
#include <QVector>
#include <QRect>
#include "defs.h"

#include <phonon/videowidget.h>

#include <e32std.h>
class RWindowBase;

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace MMF
{
class AncestorMoveMonitor;

class VideoOutput       :   public QWidget
{
    Q_OBJECT

public:
    VideoOutput(AncestorMoveMonitor* ancestorMoveMonitor, QWidget* parent);
    ~VideoOutput();

    // Set size of video frame.  Called by VideoPlayer.
    void setVideoSize(const QSize& size);

    RWindowBase* videoWindow();
    const QRect& videoWindowRect() const;

    Phonon::VideoWidget::AspectRatio aspectRatio() const;
    void setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio);

    Phonon::VideoWidget::ScaleMode scaleMode() const;
    void setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode);

    // Called by AncestorMoveMonitor
    void ancestorMoved();

    // Debugging output
    void dump() const;

public Q_SLOTS:
    void beginNativePaintEvent(const QRect& /*controlRect*/);
    void endNativePaintEvent(const QRect& /*controlRect*/);

Q_SIGNALS:
    void videoWindowChanged();
    void aspectRatioChanged();
    void scaleModeChanged();
    void beginVideoWindowNativePaint();
    void endVideoWindowNativePaint();

protected:
    // Override QWidget functions
    QSize sizeHint() const;
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void moveEvent(QMoveEvent* event);
    bool event(QEvent* event);

private:
    void getVideoWindowRect();
    void registerForAncestorMoved();

private:
    // Not owned
    AncestorMoveMonitor*    m_ancestorMoveMonitor;

    QSize                   m_videoFrameSize;
    QRect                   m_videoWindowRect;

    Phonon::VideoWidget::AspectRatio        m_aspectRatio;
    Phonon::VideoWidget::ScaleMode          m_scaleMode;

};
}
}

QT_END_NAMESPACE

#endif
