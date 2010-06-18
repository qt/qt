/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "videoplayer.h"
#include "videoitem.h"

#include <QtMultimedia>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

VideoPlayer::VideoPlayer(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , videoItem(0)
    , playButton(0)
    , positionSlider(0)
{
    connect(&movie, SIGNAL(stateChanged(QMovie::MovieState)),
            this, SLOT(movieStateChanged(QMovie::MovieState)));
    connect(&movie, SIGNAL(frameChanged(int)),
            this, SLOT(frameChanged(int)));

    videoItem = new VideoItem;

    QGraphicsScene *scene = new QGraphicsScene(this);
    QGraphicsView *graphicsView = new QGraphicsView(scene);

#ifndef QT_NO_OPENGL
    graphicsView->setViewport(new QGLWidget);
#endif

    scene->addItem(videoItem);

    QSlider *rotateSlider = new QSlider(Qt::Horizontal);
    rotateSlider->setRange(-180,  180);
    rotateSlider->setValue(0);

    connect(rotateSlider, SIGNAL(valueChanged(int)),
            this, SLOT(rotateVideo(int)));

    QAbstractButton *openButton = new QPushButton(tr("Open..."));
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));

    playButton = new QPushButton;
    playButton->setEnabled(false);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(playButton, SIGNAL(clicked()),
            this, SLOT(play()));

    positionSlider = new QSlider(Qt::Horizontal);
    positionSlider->setRange(0, 0);

    connect(positionSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(setPosition(int)));

    connect(&movie, SIGNAL(frameChanged(int)),
            positionSlider, SLOT(setValue(int)));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(positionSlider);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(graphicsView);
    layout->addWidget(rotateSlider);
    layout->addLayout(controlLayout);

    setLayout(layout);
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::openFile()
{
    QStringList supportedFormats;
    foreach (QString fmt, QMovie::supportedFormats())
        supportedFormats << fmt;
    foreach (QString fmt, QImageReader::supportedImageFormats())
        supportedFormats << fmt;

    QString filter = "Images (";
    foreach ( QString fmt, supportedFormats) {
        filter.append(QString("*.%1 ").arg(fmt));
    }
    filter.append(")");

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),
            QDir::homePath(), filter);

    if (!fileName.isEmpty()) {
        videoItem->stop();

        movie.setFileName(fileName);

        playButton->setEnabled(true);
        positionSlider->setMaximum(movie.frameCount());

        movie.jumpToFrame(0);
    }
}

void VideoPlayer::play()
{
    switch(movie.state()) {
    case QMovie::NotRunning:
        movie.start();
        break;
    case QMovie::Paused:
        movie.setPaused(false);
        break;
    case QMovie::Running:
        movie.setPaused(true);
        break;
    }
}

void VideoPlayer::movieStateChanged(QMovie::MovieState state)
{
    switch(state) {
    case QMovie::NotRunning:
    case QMovie::Paused:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    case QMovie::Running:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    }
}

void VideoPlayer::frameChanged(int frame)
{
    if (!presentImage(movie.currentImage())) {
        movie.stop();
        playButton->setEnabled(false);
        positionSlider->setMaximum(0);
    } else {
        positionSlider->setValue(frame);
    }
}

void VideoPlayer::setPosition(int frame)
{
    movie.jumpToFrame(frame);
}

void VideoPlayer::rotateVideo(int angle)
{
    //rotate around the center of video element
    qreal x = videoItem->boundingRect().width() / 2.0;
    qreal y = videoItem->boundingRect().height() / 2.0;
    videoItem->setTransform(QTransform().translate(x, y).rotate(angle).translate(-x, -y));
}

bool VideoPlayer::presentImage(const QImage &image)
{
    QVideoFrame frame(image);

    if (!frame.isValid())
        return false;

    QVideoSurfaceFormat currentFormat = videoItem->surfaceFormat();

    if (frame.pixelFormat() != currentFormat.pixelFormat()
            || frame.size() != currentFormat.frameSize()) {
        QVideoSurfaceFormat format(frame.size(), frame.pixelFormat());

        if (!videoItem->start(format))
            return false;
    }

    if (!videoItem->present(frame)) {
        videoItem->stop();

        return false;
    } else {
        return true;
    }
}
