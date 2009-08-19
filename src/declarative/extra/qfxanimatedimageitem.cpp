/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QMovie>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlengine.h>
#include "qfxanimatedimageitem.h"
#include "qfxanimatedimageitem_p.h"
#include <QNetworkRequest>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

/*!
    \class QFxAnimatedImageItem
    \internal
*/

/*!
    \qmlclass AnimatedImage
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
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,AnimatedImage,QFxAnimatedImageItem)

QFxAnimatedImageItem::QFxAnimatedImageItem(QFxItem *parent)
    : QFxImage(*(new QFxAnimatedImageItemPrivate), parent)
{
}

QFxAnimatedImageItem::QFxAnimatedImageItem(QFxAnimatedImageItemPrivate &dd, QFxItem *parent)
    : QFxImage(dd, parent)
{
}

QFxAnimatedImageItem::~QFxAnimatedImageItem()
{
    Q_D(QFxAnimatedImageItem);
    delete d->_movie;
}

/*!
  \qmlproperty bool AnimatedImage::paused
  This property holds whether the animated image is paused or not

  Defaults to false, and can be set to true when you want to pause.
*/
bool QFxAnimatedImageItem::isPaused() const
{
    Q_D(const QFxAnimatedImageItem);
    if(!d->_movie)
        return false;
    return d->_movie->state()==QMovie::Paused;
}

void QFxAnimatedImageItem::setPaused(bool pause)
{
    Q_D(QFxAnimatedImageItem);
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
bool QFxAnimatedImageItem::isPlaying() const
{
    Q_D(const QFxAnimatedImageItem);
    if (!d->_movie)
        return false;
    return d->_movie->state()!=QMovie::NotRunning;
}

void QFxAnimatedImageItem::setPlaying(bool play)
{
    Q_D(QFxAnimatedImageItem);
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
int QFxAnimatedImageItem::currentFrame() const
{
    Q_D(const QFxAnimatedImageItem);
    if (!d->_movie)
        return -1;
    return d->_movie->currentFrameNumber();
}

void QFxAnimatedImageItem::setCurrentFrame(int frame)
{
    Q_D(QFxAnimatedImageItem);
    if (!d->_movie)
        return;
    d->_movie->jumpToFrame(frame);
}

int QFxAnimatedImageItem::frameCount() const
{
    Q_D(const QFxAnimatedImageItem);
    if (!d->_movie)
        return 0;
    return d->_movie->frameCount();
}

void QFxAnimatedImageItem::setSource(const QUrl &url)
{
    Q_D(QFxAnimatedImageItem);
    if (url == d->url)
        return;

    delete d->_movie;
    d->_movie = 0;

    if (d->reply) {
        d->reply->deleteLater();
        d->reply = 0;
    }

    d->url = qmlContext(this)->resolvedUrl(url);

    if (url.isEmpty()) {
        delete d->_movie;
        d->status = Null;
    } else {
        d->status = Loading;
        QNetworkRequest req(d->url);
        req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        d->reply = qmlContext(this)->engine()->networkAccessManager()->get(req);
        QObject::connect(d->reply, SIGNAL(finished()),
                         this, SLOT(movieRequestFinished()));
    }

    emit statusChanged(d->status);
}

void QFxAnimatedImageItem::movieRequestFinished()
{
    Q_D(QFxAnimatedImageItem);
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
    else
        d->_movie->jumpToFrame(0);
    if(d->paused)
        d->_movie->setPaused(true);
    setPixmap(d->_movie->currentPixmap());
}

void QFxAnimatedImageItem::movieUpdate()
{
    Q_D(QFxAnimatedImageItem);
    setPixmap(d->_movie->currentPixmap());
    emit frameChanged();
}

void QFxAnimatedImageItem::playingStatusChanged()
{
    Q_D(QFxAnimatedImageItem);
    if((d->_movie->state() != QMovie::NotRunning) != d->playing){
        d->playing = (d->_movie->state() != QMovie::NotRunning);
        emit playingChanged();
    }
    if((d->_movie->state() == QMovie::Paused) != d->paused){
        d->playing = (d->_movie->state() == QMovie::Paused);
        emit pausedChanged();
    }
}

QT_END_NAMESPACE
