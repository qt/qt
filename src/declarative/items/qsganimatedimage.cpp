// Commit: af33f9f2e7ec433b81f5c18e3e7395db4a56c5fe
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsganimatedimage_p.h"
#include "qsganimatedimage_p_p.h"

#ifndef QT_NO_MOVIE

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtGui/qmovie.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>

#include <private/qdeclarativeengine_p.h>

QT_BEGIN_NAMESPACE

QSGAnimatedImage::QSGAnimatedImage(QSGItem *parent)
    : QSGImage(*(new QSGAnimatedImagePrivate), parent)
{
}

QSGAnimatedImage::~QSGAnimatedImage()
{
    Q_D(QSGAnimatedImage);
    delete d->_movie;
}

bool QSGAnimatedImage::isPaused() const
{
    Q_D(const QSGAnimatedImage);
    if(!d->_movie)
        return false;
    return d->_movie->state()==QMovie::Paused;
}

void QSGAnimatedImage::setPaused(bool pause)
{
    Q_D(QSGAnimatedImage);
    if(pause == d->paused)
        return;
    d->paused = pause;
    if(!d->_movie)
        return;
    d->_movie->setPaused(pause);
}

bool QSGAnimatedImage::isPlaying() const
{
    Q_D(const QSGAnimatedImage);
    if (!d->_movie)
        return false;
    return d->_movie->state()!=QMovie::NotRunning;
}

void QSGAnimatedImage::setPlaying(bool play)
{
    Q_D(QSGAnimatedImage);
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

int QSGAnimatedImage::currentFrame() const
{
    Q_D(const QSGAnimatedImage);
    if (!d->_movie)
        return d->preset_currentframe;
    return d->_movie->currentFrameNumber();
}

void QSGAnimatedImage::setCurrentFrame(int frame)
{
    Q_D(QSGAnimatedImage);
    if (!d->_movie) {
        d->preset_currentframe = frame;
        return;
    }
    d->_movie->jumpToFrame(frame);
}

int QSGAnimatedImage::frameCount() const
{
    Q_D(const QSGAnimatedImage);
    if (!d->_movie)
        return 0;
    return d->_movie->frameCount();
}

void QSGAnimatedImage::setSource(const QUrl &url)
{
    Q_D(QSGAnimatedImage);
    if (url == d->url)
        return;

    delete d->_movie;
    d->_movie = 0;

    if (d->reply) {
        d->reply->deleteLater();
        d->reply = 0;
    }

    d->url = url;
    emit sourceChanged(d->url);

    if (isComponentComplete())
        load();
}

void QSGAnimatedImage::load()
{
    Q_D(QSGAnimatedImage);

    QSGImageBase::Status oldStatus = d->status;
    qreal oldProgress = d->progress;

    if (d->url.isEmpty()) {
        delete d->_movie;
        d->setPixmap(QPixmap());
        d->progress = 0;
        d->status = Null;
        if (d->status != oldStatus)
            emit statusChanged(d->status);
        if (d->progress != oldProgress)
            emit progressChanged(d->progress);
    } else {
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
        QString lf = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(d->url);
        if (!lf.isEmpty()) {
            //### should be unified with movieRequestFinished
            d->_movie = new QMovie(lf);
            if (!d->_movie->isValid()){
                qmlInfo(this) << "Error Reading Animated Image File " << d->url.toString();
                delete d->_movie;
                d->_movie = 0;
                d->status = Error;
                if (d->status != oldStatus)
                    emit statusChanged(d->status);
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
            if (d->status != oldStatus)
                emit statusChanged(d->status);
            if (d->progress != oldProgress)
                emit progressChanged(d->progress);
            return;
        }
#endif
        d->status = Loading;
        d->progress = 0;
        emit statusChanged(d->status);
        emit progressChanged(d->progress);
        QNetworkRequest req(d->url);
        req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
        d->reply = qmlEngine(this)->networkAccessManager()->get(req);
        QObject::connect(d->reply, SIGNAL(finished()),
                         this, SLOT(movieRequestFinished()));
        QObject::connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                         this, SLOT(requestProgress(qint64,qint64)));
    }
}

#define ANIMATEDIMAGE_MAXIMUM_REDIRECT_RECURSION 16

void QSGAnimatedImage::movieRequestFinished()
{
    Q_D(QSGAnimatedImage);

    d->redirectCount++;
    if (d->redirectCount < ANIMATEDIMAGE_MAXIMUM_REDIRECT_RECURSION) {
        QVariant redirect = d->reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            QUrl url = d->reply->url().resolved(redirect.toUrl());
            d->reply->deleteLater();
            d->reply = 0;
            setSource(url);
            return;
        }
    }
    d->redirectCount=0;

    d->_movie = new QMovie(d->reply);
    if (!d->_movie->isValid()){
#ifndef QT_NO_DEBUG_STREAM
        qmlInfo(this) << "Error Reading Animated Image File " << d->url;
#endif
        delete d->_movie;
        d->_movie = 0;
        d->status = Error;
        emit statusChanged(d->status);
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
    d->status = Ready;
    emit statusChanged(d->status);
}

void QSGAnimatedImage::movieUpdate()
{
    Q_D(QSGAnimatedImage);
    d->setPixmap(d->_movie->currentPixmap());
    emit frameChanged();
}

void QSGAnimatedImage::playingStatusChanged()
{
    Q_D(QSGAnimatedImage);
    if((d->_movie->state() != QMovie::NotRunning) != d->playing){
        d->playing = (d->_movie->state() != QMovie::NotRunning);
        emit playingChanged();
    }
    if((d->_movie->state() == QMovie::Paused) != d->paused){
        d->playing = (d->_movie->state() == QMovie::Paused);
        emit pausedChanged();
    }
}

void QSGAnimatedImage::componentComplete()
{
    Q_D(QSGAnimatedImage);
    QSGItem::componentComplete(); // NOT QSGImage
    if (d->url.isValid())
        load();
    if (!d->reply) {
        setCurrentFrame(d->preset_currentframe);
        d->preset_currentframe = 0;
    }
}

QT_END_NAMESPACE

#endif // QT_NO_MOVIE
