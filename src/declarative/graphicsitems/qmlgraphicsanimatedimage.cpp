/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlgraphicsanimatedimage_p.h"
#include "qmlgraphicsanimatedimage_p_p.h"

#include <qmlengine.h>

#include <QMovie>
#include <QNetworkRequest>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

/*!
    \class QmlGraphicsAnimatedImage
    \internal
*/

/*!
    \qmlclass AnimatedImage QmlGraphicsAnimatedImage
    \inherits Image

    This item provides for playing animations stored as images containing a series of frames,
    such as GIF files. The full list of supported formats can be determined with
    QMovie::supportedFormats().

    \table
    \row
    \o \image animatedimageitem.gif
    \o
    \qml
Item {
    width: anim.width; height: anim.height+8
    AnimatedImage { id: anim; source: "pics/games-anim.gif" }
    Rectangle { color: "red"; width: 4; height: 8; y: anim.height
        x: (anim.width-width)*anim.currentFrame/(anim.frameCount-1)
    }
}
    \endqml
    \endtable
*/

QmlGraphicsAnimatedImage::QmlGraphicsAnimatedImage(QmlGraphicsItem *parent)
    : QmlGraphicsImage(*(new QmlGraphicsAnimatedImagePrivate), parent)
{
}

QmlGraphicsAnimatedImage::~QmlGraphicsAnimatedImage()
{
    Q_D(QmlGraphicsAnimatedImage);
    delete d->_movie;
}

/*!
  \qmlproperty bool AnimatedImage::paused
  This property holds whether the animated image is paused or not

  Defaults to false, and can be set to true when you want to pause.
*/
bool QmlGraphicsAnimatedImage::isPaused() const
{
    Q_D(const QmlGraphicsAnimatedImage);
    if(!d->_movie)
        return false;
    return d->_movie->state()==QMovie::Paused;
}

void QmlGraphicsAnimatedImage::setPaused(bool pause)
{
    Q_D(QmlGraphicsAnimatedImage);
    if(pause == d->paused)
        return;
    d->paused = pause;
    if(!d->_movie)
        return;
    d->_movie->setPaused(pause);
}
/*!
  \qmlproperty bool AnimatedImage::playing
  This property holds whether the animated image is playing or not

  Defaults to true, so as to start playing immediately.
*/
bool QmlGraphicsAnimatedImage::isPlaying() const
{
    Q_D(const QmlGraphicsAnimatedImage);
    if (!d->_movie)
        return false;
    return d->_movie->state()!=QMovie::NotRunning;
}

void QmlGraphicsAnimatedImage::setPlaying(bool play)
{
    Q_D(QmlGraphicsAnimatedImage);
    if(play == d->playing)
        return;
    d->playing = play;
    if (!d->_movie)
        return;
    if (play)
        d->_movie->start();
    else
        d->_movie->stop();
}

/*!
  \qmlproperty int AnimatedImage::currentFrame
  \qmlproperty int AnimatedImage::frameCount

  currentFrame is the frame that is currently visible. Watching when this changes can
  allow other things to animate at the same time as the image. frameCount is the number
  of frames in the animation. For some animation formats, frameCount is unknown and set to zero.
*/
int QmlGraphicsAnimatedImage::currentFrame() const
{
    Q_D(const QmlGraphicsAnimatedImage);
    if (!d->_movie)
        return d->preset_currentframe;
    return d->_movie->currentFrameNumber();
}

void QmlGraphicsAnimatedImage::setCurrentFrame(int frame)
{
    Q_D(QmlGraphicsAnimatedImage);
    if (!d->_movie) {
        d->preset_currentframe = frame;
        return;
    }
    d->_movie->jumpToFrame(frame);
}

int QmlGraphicsAnimatedImage::frameCount() const
{
    Q_D(const QmlGraphicsAnimatedImage);
    if (!d->_movie)
        return 0;
    return d->_movie->frameCount();
}

static QString toLocalFileOrQrc(const QUrl& url)
{
    QString r = url.toLocalFile();
    if (r.isEmpty() && url.scheme() == QLatin1String("qrc"))
        r = QLatin1Char(':') + url.path();
    return r;
}

void QmlGraphicsAnimatedImage::setSource(const QUrl &url)
{
    Q_D(QmlGraphicsAnimatedImage);
    if (url == d->url)
        return;

    delete d->_movie;
    d->_movie = 0;

    if (d->reply) {
        d->reply->deleteLater();
        d->reply = 0;
    }

    d->url = url;

    if (url.isEmpty()) {
        delete d->_movie;
        d->status = Null;
    } else {
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
        QString lf = toLocalFileOrQrc(url);
        if (!lf.isEmpty()) {
            //### should be unified with movieRequestFinished
            d->_movie = new QMovie(lf);
            if (!d->_movie->isValid()){
                qWarning() << "Error Reading Animated Image File" << d->url;
                delete d->_movie;
                d->_movie = 0;
                return;
            }
            connect(d->_movie, SIGNAL(stateChanged(QMovie::MovieState)),
                    this, SLOT(playingStatusChanged()));
            connect(d->_movie, SIGNAL(frameChanged(int)),
                    this, SLOT(movieUpdate()));
            d->_movie->setCacheMode(QMovie::CacheAll);
            if(d->playing)
                d->_movie->start();
            else
                d->_movie->jumpToFrame(0);
            if(d->paused)
                d->_movie->setPaused(true);
            d->setPixmap(d->_movie->currentPixmap());
            d->status = Ready;
            d->progress = 1.0;
            emit statusChanged(d->status);
            emit sourceChanged(d->url);
            emit progressChanged(d->progress);
            return;
        }
#endif
        d->status = Loading;
        QNetworkRequest req(d->url);
        req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        d->reply = qmlEngine(this)->networkAccessManager()->get(req);
        QObject::connect(d->reply, SIGNAL(finished()),
                         this, SLOT(movieRequestFinished()));
    }

    emit statusChanged(d->status);
}

void QmlGraphicsAnimatedImage::movieRequestFinished()
{
    Q_D(QmlGraphicsAnimatedImage);
    d->_movie = new QMovie(d->reply);
    if (!d->_movie->isValid()){
        qWarning() << "Error Reading Animated Image File " << d->url;
        delete d->_movie;
        d->_movie = 0;
        return;
    }
    connect(d->_movie, SIGNAL(stateChanged(QMovie::MovieState)),
            this, SLOT(playingStatusChanged()));
    connect(d->_movie, SIGNAL(frameChanged(int)),
            this, SLOT(movieUpdate()));
    d->_movie->setCacheMode(QMovie::CacheAll);
    if(d->playing)
        d->_movie->start();
    if (d->paused || !d->playing) {
        d->_movie->jumpToFrame(d->preset_currentframe);
        d->preset_currentframe = 0;
    }
    if(d->paused)
        d->_movie->setPaused(true);
    d->setPixmap(d->_movie->currentPixmap());
}

void QmlGraphicsAnimatedImage::movieUpdate()
{
    Q_D(QmlGraphicsAnimatedImage);
    d->setPixmap(d->_movie->currentPixmap());
    emit frameChanged();
}

void QmlGraphicsAnimatedImage::playingStatusChanged()
{
    Q_D(QmlGraphicsAnimatedImage);
    if((d->_movie->state() != QMovie::NotRunning) != d->playing){
        d->playing = (d->_movie->state() != QMovie::NotRunning);
        emit playingChanged();
    }
    if((d->_movie->state() == QMovie::Paused) != d->paused){
        d->playing = (d->_movie->state() == QMovie::Paused);
        emit pausedChanged();
    }
}

void QmlGraphicsAnimatedImage::componentComplete()
{
    Q_D(QmlGraphicsAnimatedImage);
    if (!d->reply) {
        setCurrentFrame(d->preset_currentframe);
        d->preset_currentframe = 0;
    }
}

QT_END_NAMESPACE
