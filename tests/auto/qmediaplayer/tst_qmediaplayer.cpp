/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>
#include <QtCore/qdebug.h>
#include <QtCore/qbuffer.h>

#include <QtMediaServices/qmediaplayer.h>
#include <QtMediaServices/qmediaplayercontrol.h>
#include <QtMediaServices/qmediaplaylist.h>
#include <QtMediaServices/qmediaservice.h>



class AutoConnection
{
public:
    AutoConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
            : sender(sender), signal(signal), receiver(receiver), method(method)
    {
        QObject::connect(sender, signal, receiver, method);
    }

    ~AutoConnection()
    {
        QObject::disconnect(sender, signal, receiver, method);
    }

private:
    QObject *sender;
    const char *signal;
    QObject *receiver;
    const char *method;
};


class MockPlayerControl : public QMediaPlayerControl
{
    friend class MockPlayerService;

public:
    MockPlayerControl():QMediaPlayerControl(0) {}

    QMediaPlayer::State state() const { return _state; }
    QMediaPlayer::MediaStatus mediaStatus() const { return _mediaStatus; }

    qint64 duration() const { return _duration; }

    qint64 position() const { return _position; }

    void setPosition(qint64 position) { if (position != _position) emit positionChanged(_position = position); }

    int volume() const { return _volume; }
    void setVolume(int volume) { emit volumeChanged(_volume = volume); }

    bool isMuted() const { return _muted; }
    void setMuted(bool muted) { if (muted != _muted) emit mutedChanged(_muted = muted); }

    int bufferStatus() const { return _bufferStatus; }

    bool isAudioAvailable() const { return _audioAvailable; }
    bool isVideoAvailable() const { return _videoAvailable; }

    bool isSeekable() const { return _isSeekable; }
    QMediaTimeRange availablePlaybackRanges() const { return QMediaTimeRange(_seekRange.first, _seekRange.second); }
    void setSeekRange(qint64 minimum, qint64 maximum) { _seekRange = qMakePair(minimum, maximum); }

    qreal playbackRate() const { return _playbackRate; }
    void setPlaybackRate(qreal rate) { if (rate != _playbackRate) emit playbackRateChanged(_playbackRate = rate); }

    QMediaContent media() const { return _media; }
    void setMedia(const QMediaContent &content, QIODevice *stream)
    {
        _stream = stream;
        _media = content;
        if (_state != QMediaPlayer::StoppedState) {
            _mediaStatus = _media.isNull() ? QMediaPlayer::NoMedia : QMediaPlayer::LoadingMedia;
            emit stateChanged(_state = QMediaPlayer::StoppedState);
            emit mediaStatusChanged(_mediaStatus);
        }
        emit mediaChanged(_media = content);
    }
    QIODevice *mediaStream() const { return _stream; }

    void play() { if (_isValid && !_media.isNull() && _state != QMediaPlayer::PlayingState) emit stateChanged(_state = QMediaPlayer::PlayingState); }
    void pause() { if (_isValid && !_media.isNull() && _state != QMediaPlayer::PausedState) emit stateChanged(_state = QMediaPlayer::PausedState); }
    void stop() { if (_state != QMediaPlayer::StoppedState) emit stateChanged(_state = QMediaPlayer::StoppedState); }

    QMediaPlayer::State _state;
    QMediaPlayer::MediaStatus _mediaStatus;
    QMediaPlayer::Error _error;
    qint64 _duration;
    qint64 _position;
    int _volume;
    bool _muted;
    int _bufferStatus;
    bool _audioAvailable;
    bool _videoAvailable;
    bool _isSeekable;
    QPair<qint64, qint64> _seekRange;
    qreal _playbackRate;
    QMediaContent _media;
    QIODevice *_stream;
    bool _isValid;
    QString _errorString;
};


class MockPlayerService : public QMediaService
{
    Q_OBJECT

public:
    MockPlayerService():QMediaService(0)
    {
        mockControl = new MockPlayerControl;
    }

    ~MockPlayerService()
    {
        delete mockControl;
    }

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QMediaPlayerControl_iid) == 0)
            return mockControl;

        return 0;
    }

    void setState(QMediaPlayer::State state) { emit mockControl->stateChanged(mockControl->_state = state); }
    void setState(QMediaPlayer::State state, QMediaPlayer::MediaStatus status) {
        mockControl->_state = state;
        mockControl->_mediaStatus = status;
        emit mockControl->mediaStatusChanged(status);
        emit mockControl->stateChanged(state);
    }
    void setMediaStatus(QMediaPlayer::MediaStatus status) { emit mockControl->mediaStatusChanged(mockControl->_mediaStatus = status); }
    void setIsValid(bool isValid) { mockControl->_isValid = isValid; }
    void setMedia(QMediaContent media) { mockControl->_media = media; }
    void setDuration(qint64 duration) { mockControl->_duration = duration; }
    void setPosition(qint64 position) { mockControl->_position = position; }
    void setSeekable(bool seekable) { mockControl->_isSeekable = seekable; }
    void setVolume(int volume) { mockControl->_volume = volume; }
    void setMuted(bool muted) { mockControl->_muted = muted; }
    void setVideoAvailable(bool videoAvailable) { mockControl->_videoAvailable = videoAvailable; }
    void setBufferStatus(int bufferStatus) { mockControl->_bufferStatus = bufferStatus; }
    void setPlaybackRate(qreal playbackRate) { mockControl->_playbackRate = playbackRate; }
    void setError(QMediaPlayer::Error error) { mockControl->_error = error; emit mockControl->error(mockControl->_error, mockControl->_errorString); }
    void setErrorString(QString errorString) { mockControl->_errorString = errorString; emit mockControl->error(mockControl->_error, mockControl->_errorString); }

    void reset()
    {
        mockControl->_state = QMediaPlayer::StoppedState;
        mockControl->_mediaStatus = QMediaPlayer::UnknownMediaStatus;
        mockControl->_error = QMediaPlayer::NoError;
        mockControl->_duration = 0;
        mockControl->_position = 0;
        mockControl->_volume = 0;
        mockControl->_muted = false;
        mockControl->_bufferStatus = 0;
        mockControl->_videoAvailable = false;
        mockControl->_isSeekable = false;
        mockControl->_playbackRate = 0.0;
        mockControl->_media = QMediaContent();
        mockControl->_stream = 0;
        mockControl->_isValid = false;
        mockControl->_errorString = QString();
    }

    MockPlayerControl *mockControl;
};

class MockProvider : public QMediaServiceProvider
{
public:
    MockProvider(MockPlayerService *service):mockService(service) {}
    QMediaService *requestService(const QByteArray &, const QMediaServiceProviderHint &)
    {
        return mockService;
    }

    void releaseService(QMediaService *service) { delete service; }

    MockPlayerService *mockService;
};

class tst_QMediaPlayer: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase_data();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testNullService();
    void testValid();
    void testMedia();
    void testDuration();
    void testPosition();
    void testVolume();
    void testMuted();
    void testVideoAvailable();
    void testBufferStatus();
    void testSeekable();
    void testPlaybackRate();
    void testError();
    void testErrorString();
    void testService();
    void testPlay();
    void testPause();
    void testStop();
    void testMediaStatus();
    void testPlaylist();

private:
    MockProvider *mockProvider;
    MockPlayerService  *mockService;
    QMediaPlayer *player;
};

void tst_QMediaPlayer::initTestCase_data()
{
    QTest::addColumn<bool>("valid");
    QTest::addColumn<QMediaPlayer::State>("state");
    QTest::addColumn<QMediaPlayer::MediaStatus>("status");
    QTest::addColumn<QMediaContent>("mediaContent");
    QTest::addColumn<qint64>("duration");
    QTest::addColumn<qint64>("position");
    QTest::addColumn<bool>("seekable");
    QTest::addColumn<int>("volume");
    QTest::addColumn<bool>("muted");
    QTest::addColumn<bool>("videoAvailable");
    QTest::addColumn<int>("bufferStatus");
    QTest::addColumn<qreal>("playbackRate");
    QTest::addColumn<QMediaPlayer::Error>("error");
    QTest::addColumn<QString>("errorString");

    QTest::newRow("invalid") << false << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent() << qint64(0) << qint64(0) << false << 0 << false << false << 0 <<
                                qreal(0) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+null") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent() << qint64(0) << qint64(0) << false << 0 << false << false << 50 <<
                                qreal(0) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+stopped") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(0) << qint64(0) << false << 50 << false << false << 0 <<
                                qreal(1) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+playing") << true << QMediaPlayer::PlayingState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(10000) << qint64(10) << true << 50 << true << false << 0 <<
                                qreal(1) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+paused") << true << QMediaPlayer::PausedState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(10000) << qint64(10) << true << 50 << true << false << 0 <<
                                qreal(1)  << QMediaPlayer::NoError << QString();
    QTest::newRow("valud+streaming") << true << QMediaPlayer::PlayingState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("http://example.com/stream")) << qint64(10000) << qint64(10000) << false << 50 << false << true << 0 <<
                                qreal(1)  << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+error") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent(QUrl("http://example.com/stream")) << qint64(0) << qint64(0) << false << 50 << false << false << 0 <<
                                qreal(0) << QMediaPlayer::ResourceError << QString("Resource unavailable");
}

void tst_QMediaPlayer::initTestCase()
{
    qRegisterMetaType<QMediaContent>();

    mockService = new MockPlayerService;
    mockProvider = new MockProvider(mockService);
    player = new QMediaPlayer(0, 0, mockProvider);
}

void tst_QMediaPlayer::cleanupTestCase()
{
    delete player;
}

void tst_QMediaPlayer::init()
{
    mockService->reset();
}

void tst_QMediaPlayer::cleanup()
{
}

void tst_QMediaPlayer::testNullService()
{
    MockProvider provider(0);
    QMediaPlayer player(0, 0, &provider);

    const QIODevice *nullDevice = 0;

    QCOMPARE(player.media(), QMediaContent());
    QCOMPARE(player.mediaStream(), nullDevice);
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(player.mediaStatus(), QMediaPlayer::UnknownMediaStatus);
    QCOMPARE(player.duration(), qint64(-1));
    QCOMPARE(player.position(), qint64(0));
    QCOMPARE(player.volume(), 0);
    QCOMPARE(player.isMuted(), false);
    QCOMPARE(player.isVideoAvailable(), false);
    QCOMPARE(player.bufferStatus(), 0);
    QCOMPARE(player.isSeekable(), false);
    QCOMPARE(player.playbackRate(), qreal(0));
    QCOMPARE(player.error(), QMediaPlayer::ServiceMissingError);

    {
        QFETCH_GLOBAL(QMediaContent, mediaContent);

        QSignalSpy spy(&player, SIGNAL(mediaChanged(QMediaContent)));
        QFile file;

        player.setMedia(mediaContent, &file);
        QCOMPARE(player.media(), QMediaContent());
        QCOMPARE(player.mediaStream(), nullDevice);
        QCOMPARE(spy.count(), 0);
    } {
        QSignalSpy stateSpy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));
        QSignalSpy statusSpy(&player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));

        player.play();
        QCOMPARE(player.state(), QMediaPlayer::StoppedState);
        QCOMPARE(player.mediaStatus(), QMediaPlayer::UnknownMediaStatus);
        QCOMPARE(stateSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);

        player.pause();
        QCOMPARE(player.state(), QMediaPlayer::StoppedState);
        QCOMPARE(player.mediaStatus(), QMediaPlayer::UnknownMediaStatus);
        QCOMPARE(stateSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);

        player.stop();
        QCOMPARE(player.state(), QMediaPlayer::StoppedState);
        QCOMPARE(player.mediaStatus(), QMediaPlayer::UnknownMediaStatus);
        QCOMPARE(stateSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);
    } {
        QFETCH_GLOBAL(int, volume);
        QFETCH_GLOBAL(bool, muted);

        QSignalSpy volumeSpy(&player, SIGNAL(volumeChanged(int)));
        QSignalSpy mutingSpy(&player, SIGNAL(mutedChanged(bool)));

        player.setVolume(volume);
        QCOMPARE(player.volume(), 0);
        QCOMPARE(volumeSpy.count(), 0);

        player.setMuted(muted);
        QCOMPARE(player.isMuted(), false);
        QCOMPARE(mutingSpy.count(), 0);
    } {
        QFETCH_GLOBAL(qint64, position);

        QSignalSpy spy(&player, SIGNAL(positionChanged(qint64)));

        player.setPosition(position);
        QCOMPARE(player.position(), qint64(0));
        QCOMPARE(spy.count(), 0);
    } {
        QFETCH_GLOBAL(qreal, playbackRate);

        QSignalSpy spy(&player, SIGNAL(playbackRateChanged(qreal)));

        player.setPlaybackRate(playbackRate);
        QCOMPARE(player.playbackRate(), qreal(0));
        QCOMPARE(spy.count(), 0);
    } {
        QMediaPlaylist playlist;
        playlist.setMediaObject(&player);

        QSignalSpy mediaSpy(&player, SIGNAL(mediaChanged(QMediaContent)));
        QSignalSpy statusSpy(&player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));

        playlist.addMedia(QUrl("http://example.com/stream"));
        playlist.addMedia(QUrl("file:///some.mp3"));

        playlist.setCurrentIndex(0);
        QCOMPARE(playlist.currentIndex(), 0);
        QCOMPARE(player.media(), QMediaContent());
        QCOMPARE(mediaSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);

        playlist.next();
        QCOMPARE(playlist.currentIndex(), 1);
        QCOMPARE(player.media(), QMediaContent());
        QCOMPARE(mediaSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);
    }
}

void tst_QMediaPlayer::testValid()
{
    /*
    QFETCH_GLOBAL(bool, valid);

    mockService->setIsValid(valid);
    QCOMPARE(player->isValid(), valid);
    */
}

void tst_QMediaPlayer::testMedia()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);

    mockService->setMedia(mediaContent);
    QCOMPARE(player->media(), mediaContent);

    QBuffer stream;
    player->setMedia(mediaContent, &stream);
    QCOMPARE(player->media(), mediaContent);
    QCOMPARE((QBuffer*)player->mediaStream(), &stream);
}

void tst_QMediaPlayer::testDuration()
{
    QFETCH_GLOBAL(qint64, duration);

    mockService->setDuration(duration);
    QVERIFY(player->duration() == duration);
}

void tst_QMediaPlayer::testPosition()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(bool, seekable);
    QFETCH_GLOBAL(qint64, position);
    QFETCH_GLOBAL(qint64, duration);

    mockService->setIsValid(valid);
    mockService->setSeekable(seekable);
    mockService->setPosition(position);
    mockService->setDuration(duration);
    QVERIFY(player->isSeekable() == seekable);
    QVERIFY(player->position() == position);
    QVERIFY(player->duration() == duration);

    if (seekable) {
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(position);
        QCOMPARE(player->position(), position);
        QCOMPARE(spy.count(), 0); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(0);
        QCOMPARE(player->position(), qint64(0));
        QCOMPARE(spy.count(), position == 0 ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(duration);
        QCOMPARE(player->position(), duration);
        QCOMPARE(spy.count(), position == duration ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(-1);
        QCOMPARE(player->position(), qint64(0));
        QCOMPARE(spy.count(), position == 0 ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(duration + 1);
        QCOMPARE(player->position(), duration);
        QCOMPARE(spy.count(), position == duration ? 0 : 1); }
    }
    else {
        QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(position);

        QCOMPARE(player->position(), position);
        QCOMPARE(spy.count(), 0);
    }
}

void tst_QMediaPlayer::testVolume()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(int, volume);

    mockService->setVolume(volume);
    QVERIFY(player->volume() == volume);

    if (valid) {
        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(10);
        QCOMPARE(player->volume(), 10);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(-1000);
        QCOMPARE(player->volume(), 0);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(100);
        QCOMPARE(player->volume(), 100);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(1000);
        QCOMPARE(player->volume(), 100);
        QCOMPARE(spy.count(), 0); }
    }
}

void tst_QMediaPlayer::testMuted()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(bool, muted);
    QFETCH_GLOBAL(int, volume);

    if (valid) {
        mockService->setMuted(muted);
        mockService->setVolume(volume);
        QVERIFY(player->isMuted() == muted);

        QSignalSpy spy(player, SIGNAL(mutedChanged(bool)));
        player->setMuted(!muted);
        QCOMPARE(player->isMuted(), !muted);
        QCOMPARE(player->volume(), volume);
        QCOMPARE(spy.count(), 1);
    }
}

void tst_QMediaPlayer::testVideoAvailable()
{
    QFETCH_GLOBAL(bool, videoAvailable);

    mockService->setVideoAvailable(videoAvailable);
    QVERIFY(player->isVideoAvailable() == videoAvailable);
}

void tst_QMediaPlayer::testBufferStatus()
{
    QFETCH_GLOBAL(int, bufferStatus);

    mockService->setBufferStatus(bufferStatus);
    QVERIFY(player->bufferStatus() == bufferStatus);
}

void tst_QMediaPlayer::testSeekable()
{
    QFETCH_GLOBAL(bool, seekable);

    mockService->setSeekable(seekable);
    QVERIFY(player->isSeekable() == seekable);
}

void tst_QMediaPlayer::testPlaybackRate()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(qreal, playbackRate);

    if (valid) {
        mockService->setPlaybackRate(playbackRate);
        QVERIFY(player->playbackRate() == playbackRate);

        QSignalSpy spy(player, SIGNAL(playbackRateChanged(qreal)));
        player->setPlaybackRate(playbackRate + 0.5f);
        QCOMPARE(player->playbackRate(), playbackRate + 0.5f);
        QCOMPARE(spy.count(), 1);
    }
}

void tst_QMediaPlayer::testError()
{
    QFETCH_GLOBAL(QMediaPlayer::Error, error);

    mockService->setError(error);
    QVERIFY(player->error() == error);
}

void tst_QMediaPlayer::testErrorString()
{
    QFETCH_GLOBAL(QString, errorString);

    mockService->setErrorString(errorString);
    QVERIFY(player->errorString() == errorString);
}

void tst_QMediaPlayer::testService()
{
    /*
    QFETCH_GLOBAL(bool, valid);

    mockService->setIsValid(valid);

    if (valid)
        QVERIFY(player->service() != 0);
    else
        QVERIFY(player->service() == 0);
        */
}

void tst_QMediaPlayer::testPlay()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setIsValid(valid);
    mockService->setState(state);
    mockService->setMedia(mediaContent);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaContent);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->play();

    if (!valid || mediaContent.isNull())  {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        QCOMPARE(player->state(), QMediaPlayer::PlayingState);
        QCOMPARE(spy.count(), state == QMediaPlayer::PlayingState ? 0 : 1);
    }
}

void tst_QMediaPlayer::testPause()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setIsValid(valid);
    mockService->setState(state);
    mockService->setMedia(mediaContent);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaContent);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->pause();

    if (!valid || mediaContent.isNull()) {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        QCOMPARE(player->state(), QMediaPlayer::PausedState);
        QCOMPARE(spy.count(), state == QMediaPlayer::PausedState ? 0 : 1);
    }
}

void tst_QMediaPlayer::testStop()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setState(state);
    mockService->setMedia(mediaContent);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaContent);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->stop();

    if (mediaContent.isNull() || state == QMediaPlayer::StoppedState) {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 1);
    }
}

void tst_QMediaPlayer::testMediaStatus()
{
    QFETCH_GLOBAL(int, bufferStatus);
    int bufferSignals = 0;

    player->setNotifyInterval(10);

    mockService->setMediaStatus(QMediaPlayer::NoMedia);
    mockService->setBufferStatus(bufferStatus);

    AutoConnection connection(
            player, SIGNAL(bufferStatusChanged(int)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    QSignalSpy statusSpy(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    QSignalSpy bufferSpy(player, SIGNAL(bufferStatusChanged(int)));

    QCOMPARE(player->mediaStatus(), QMediaPlayer::NoMedia);

    mockService->setMediaStatus(QMediaPlayer::LoadingMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::LoadingMedia);
    QCOMPARE(statusSpy.count(), 1);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::LoadingMedia);

    mockService->setMediaStatus(QMediaPlayer::LoadedMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::LoadedMedia);
    QCOMPARE(statusSpy.count(), 2);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::LoadedMedia);

    // Verify the bufferStatusChanged() signal isn't being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(bufferSpy.count(), 0);

    mockService->setMediaStatus(QMediaPlayer::StalledMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::StalledMedia);
    QCOMPARE(statusSpy.count(), 3);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::StalledMedia);

    // Verify the bufferStatusChanged() signal is being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(bufferSpy.count() > bufferSignals);
    QCOMPARE(bufferSpy.last().value(0).toInt(), bufferStatus);
    bufferSignals = bufferSpy.count();

    mockService->setMediaStatus(QMediaPlayer::BufferingMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::BufferingMedia);
    QCOMPARE(statusSpy.count(), 4);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::BufferingMedia);

    // Verify the bufferStatusChanged() signal is being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(bufferSpy.count() > bufferSignals);
    QCOMPARE(bufferSpy.last().value(0).toInt(), bufferStatus);
    bufferSignals = bufferSpy.count();

    mockService->setMediaStatus(QMediaPlayer::BufferedMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::BufferedMedia);
    QCOMPARE(statusSpy.count(), 5);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::BufferedMedia);

    // Verify the bufferStatusChanged() signal isn't being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(bufferSpy.count(), bufferSignals);

    mockService->setMediaStatus(QMediaPlayer::EndOfMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::EndOfMedia);
    QCOMPARE(statusSpy.count(), 6);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::EndOfMedia);
}

void tst_QMediaPlayer::testPlaylist()
{
    QMediaContent content0(QUrl(QLatin1String("test://audio/song1.mp3")));
    QMediaContent content1(QUrl(QLatin1String("test://audio/song2.mp3")));
    QMediaContent content2(QUrl(QLatin1String("test://video/movie1.mp4")));
    QMediaContent content3(QUrl(QLatin1String("test://video/movie2.mp4")));
    QMediaContent content4(QUrl(QLatin1String("test://image/photo.jpg")));

    mockService->setIsValid(true);
    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::NoMedia);

    QMediaPlaylist *playlist = new QMediaPlaylist;
    playlist->setMediaObject(player);

    QSignalSpy stateSpy(player, SIGNAL(stateChanged(QMediaPlayer::State)));
    QSignalSpy mediaSpy(player, SIGNAL(mediaChanged(QMediaContent)));

    // Test the player does nothing with an empty playlist attached.
    player->play();
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(mediaSpy.count(), 0);

    playlist->addMedia(content0);
    playlist->addMedia(content1);
    playlist->addMedia(content2);
    playlist->addMedia(content3);

    // Test changing the playlist position, changes the current media, but not the playing state.
    playlist->setCurrentIndex(1);
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(mediaSpy.count(), 1);

    // Test playing starts with the current media.
    player->play();
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(mediaSpy.count(), 1);

    // Test pausing doesn't change the current media.
    player->pause();
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(mediaSpy.count(), 1);

    // Test stopping doesn't change the current media.
    player->stop();
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 3);
    QCOMPARE(mediaSpy.count(), 1);

    // Test when the player service reaches the end of the current media, the player moves onto
    // the next item without stopping.
    player->play();
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 1);

    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    QCOMPARE(player->media(), content2);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 2);

    // Test skipping the current media doesn't change the state.
    playlist->next();
    QCOMPARE(player->media(), content3);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 3);

    // Test changing the current media while paused doesn't change the state.
    player->pause();
    mockService->setMediaStatus(QMediaPlayer::BufferedMedia);
    QCOMPARE(player->media(), content3);
    QCOMPARE(player->state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 5);
    QCOMPARE(mediaSpy.count(), 3);

    playlist->previous();
    QCOMPARE(player->media(), content2);
    QCOMPARE(player->state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 5);
    QCOMPARE(mediaSpy.count(), 4);

    // Test changing the current media while stopped doesn't change the state.
    player->stop();
    mockService->setMediaStatus(QMediaPlayer::LoadedMedia);
    QCOMPARE(player->media(), content2);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 6);
    QCOMPARE(mediaSpy.count(), 4);

    playlist->next();
    QCOMPARE(player->media(), content3);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 6);
    QCOMPARE(mediaSpy.count(), 5);

    // Test the player is stopped and the current media cleared when it reaches the end of the last
    // item in the playlist.
    player->play();
    QCOMPARE(player->media(), content3);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 7);
    QCOMPARE(mediaSpy.count(), 5);

    // Double up the signals to ensure some noise doesn't destabalize things.
    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 8);
    QCOMPARE(mediaSpy.count(), 6);

    // Test starts playing from the start of the playlist if there is no current media selected.
    player->play();
    QCOMPARE(player->media(), content0);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 9);
    QCOMPARE(mediaSpy.count(), 7);

    // Test deleting the playlist stops the player and clears the media it set.
    delete playlist;
    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 10);
    QCOMPARE(mediaSpy.count(), 8);

    // Test the player works as normal with the playlist removed.
    player->play();
    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 10);
    QCOMPARE(mediaSpy.count(), 8);

    player->setMedia(content1);
    player->play();

    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 11);
    QCOMPARE(mediaSpy.count(), 9);

    // Test the player can bind to playlist again
    playlist = new QMediaPlaylist;
    playlist->setMediaObject(player);
    QCOMPARE(playlist->mediaObject(), qobject_cast<QMediaObject*>(player));

    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);

    playlist->addMedia(content0);
    playlist->addMedia(content1);
    playlist->addMedia(content2);
    playlist->addMedia(content3);

    playlist->setCurrentIndex(1);
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);

    // Test attaching the new playlist,
    // player should detach the current one
    QMediaPlaylist *playlist2 = new QMediaPlaylist;
    playlist2->addMedia(content1);
    playlist2->addMedia(content2);
    playlist2->addMedia(content3);
    playlist2->setCurrentIndex(2);

    player->play();
    playlist2->setMediaObject(player);
    QCOMPARE(playlist2->mediaObject(), qobject_cast<QMediaObject*>(player));
    QVERIFY(playlist->mediaObject() == 0);
    QCOMPARE(player->media(), playlist2->currentMedia());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);

    playlist2->setCurrentIndex(1);
    QCOMPARE(player->media(), playlist2->currentMedia());
}

QTEST_MAIN(tst_QMediaPlayer)

#include "tst_qmediaplayer.moc"
