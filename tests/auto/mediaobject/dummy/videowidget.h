/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test sutie of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PHONON_DUMMY_VIDEOWIDGET_H
#define PHONON_DUMMY_VIDEOWIDGET_H

#include <phonon/videowidget.h>
#include <phonon/videowidgetinterface.h>

#include "backend.h"

QT_BEGIN_NAMESPACE

class QString;

namespace Phonon
{
namespace Dummy
{

class VideoWidget : public QWidget, public Phonon::VideoWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(Phonon::VideoWidgetInterface)
public:
    VideoWidget(Backend *backend, QWidget *parent = 0);
    ~VideoWidget();

    void paintEvent(QPaintEvent *event);
    void setVisible(bool);

    Phonon::VideoWidget::AspectRatio aspectRatio() const;
    void setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio);
    Phonon::VideoWidget::ScaleMode scaleMode() const;
    void setScaleMode(Phonon::VideoWidget::ScaleMode);
    qreal brightness() const;
    void setBrightness(qreal);
    qreal contrast() const;
    void setContrast(qreal);
    qreal hue() const;
    void setHue(qreal);
    qreal saturation() const;
    void setSaturation(qreal);
    void setMovieSize(const QSize &size);
    QSize sizeHint() const;
    QRect scaleToAspect(QRect srcRect, int w, int h) const;
    QRect calculateDrawFrameRect() const;

    QSize movieSize() const {
        return m_movieSize;
    }

    bool event(QEvent *);

    QWidget *widget() {
        return this;
    }

protected:
    QSize m_movieSize;

private:
    Phonon::VideoWidget::AspectRatio m_aspectRatio;
    qreal m_brightness, m_hue, m_contrast, m_saturation;
    Phonon::VideoWidget::ScaleMode m_scaleMode;
};

}
} //namespace Phonon::Dummy

QT_END_NAMESPACE

#endif // PHONON_DUMMY_VIDEOWIDGET_H
