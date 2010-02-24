/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QWidget>

#include <qmediaplayer.h>
#include <qmediaplaylist.h>
#include <qvideowidget.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QAbstractItemView;
class QLabel;
class QModelIndex;
class QSlider;
class QMediaPlayer;
class QVideoWidget;
class PlaylistModel;

class Player : public QWidget
{
    Q_OBJECT
public:
    Player(QWidget *parent = 0);
    ~Player();

Q_SIGNALS:
    void fullScreenChanged(bool fullScreen);

private slots:
    void open();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();

    void previousClicked();

    void seek(int seconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);

    void showColorDialog();

private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videoWidget;
    QLabel *coverLabel;
    QSlider *slider;
    PlaylistModel *playlistModel;
    QAbstractItemView *playlistView;
    QDialog *colorDialog;
    QString trackInfo;
    QString statusInfo;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
