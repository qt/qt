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

#include <QFile>
#include <QmlComponent>
#include <private/qmltimeline_p.h>
#include "qfxtestengine.h"
#include "qfxtestobjects.h"
#include <QCryptographicHash>
#include <QApplication>
#include <QKeyEvent>
#include <QSimpleCanvas>
#include <QMouseEvent>
#include <qmlengine.h>
#include <private/qabstractanimation_p.h>
#include <QAbstractAnimation>

QT_BEGIN_NAMESPACE

#define MAX_MISMATCHED_FRAMES 5
#define MAX_MISMATCHED_PIXELS 5

class QFxTestEnginePrivate : public QAbstractAnimation
{
public:
    QFxTestEnginePrivate(QFxTestEngine *p)
    : q(p), canvas(0), testMode(QFxTestEngine::NoTest), fullFrame(true),
      status(Working), exitOnFail(true), mismatchedFrames(0), 
      lastFrameMismatch(false) {}

    QFxTestEngine *q;

    QmlEngine engine;
    QSimpleCanvas *canvas;
    QFxTestEngine::TestMode testMode;
    QString testDirectory;

    TestLog testData;
    TestLog playbackTestData;
    bool fullFrame;
    QList<QImage> fullFrames;

    virtual void updateCurrentTime(int);

    void recordFrameEvent(const QImage &img);
    void recordFullFrameEvent(const QImage &img);
    void recordKeyEvent(QKeyEvent *e);
    void recordMouseEvent(QMouseEvent *e);
    void testPass();
    void save(const QString &filename, bool = true);

    enum MessageType { Success, Fail };
    void message(MessageType t, const char *);

    enum Status { Working, Failed, Passed };
    Status status;
    bool exitOnFail;

    QList<TestObject *> toPost;
    QSet<QEvent *> postedEvents;

    // OpenGL seems to give inconsistent rendering results.  We allow a small
    // tolerance to compensate - a maximum number of mismatched frames and only
    // one mismatch in a row
    int mismatchedFrames;
    bool lastFrameMismatch;

    bool compare(const QImage &img1, const QImage &img2);

    virtual int duration() const { return -1; }

    int elapsed() { return currentTime(); }
};

bool QFxTestEnginePrivate::compare(const QImage &img1, const QImage &img2)
{
    if (img1.size() != img2.size()) {
        qWarning() << "Image size mismatch" << img1.size() << img2.size();
        return false;
    }

    int errorCount = 0;
    for (int yy = 0; yy < img1.height(); ++yy) {
        for (int xx = 0; xx < img1.width(); ++xx) {
            if (img1.pixel(xx, yy) != img2.pixel(xx, yy)) {
                errorCount++;
                if (errorCount > MAX_MISMATCHED_PIXELS) {
                    return false;
                }
            }
        }
    }

    return true;
}

QFxTestEngine::QFxTestEngine(TestMode mode, const QString &dir, 
                             QSimpleCanvas *canvas, QObject *parent)
: QObject(parent), d(new QFxTestEnginePrivate(this))
{
    Q_ASSERT(canvas);

    d->canvas = canvas;
    d->start();

    d->testDirectory = dir;
    d->testMode = mode;
    if (d->testMode == RecordTest) {
        qWarning("QFxTestEngine: Record ON");
    } else if (d->testMode == PlaybackTest) {

        QString fileName(d->testDirectory + QLatin1String("/manifest.qml"));
        QFile f(fileName);
        if (!f.open(QIODevice::ReadOnly)) {
            qWarning() << "QFxTestEngine: Unable to open file" << fileName;
            return;
        }

        QByteArray data = f.readAll();
        QmlComponent c(&d->engine, data, QUrl(d->testDirectory + QLatin1String("/manifest.qml")));
        QObject *o = c.create();
        TestLog *log = qobject_cast<TestLog *>(o);
        if (log) {
            log->setParent(this);
            d->playbackTestData.actions() = log->actions();
            qWarning() << "QFxTestEngine: Playback ON," << d->playbackTestData.actions().count() << "actions";
        } else {
            delete o;
            qWarning() << "QFxTestEngine: File" << fileName << "is corrupt.";
            return;
        }
    }

    if (d->testMode != NoTest) {
        QUnifiedTimer::instance()->setConsistentTiming(true);
        QObject::connect(canvas, SIGNAL(framePainted()), 
                         this, SLOT(framePainted()));

        canvas->installEventFilter(this);
        for (int ii = 0; ii < d->playbackTestData.actions().count(); ++ii) {
            TestObject *o = d->playbackTestData.actions().at(ii);
            if (TestMouse *m = qobject_cast<TestMouse *>(o))
                d->toPost << m;
            else if (TestKey *k = qobject_cast<TestKey *>(o))
                d->toPost << k;
        }
    }
}

QFxTestEngine::~QFxTestEngine()
{
    delete d; d = 0;
}

void QFxTestEngine::framePainted()
{
    QImage img = d->canvas->asImage();

    if (d->fullFrame) {
        d->fullFrame = false;
        d->recordFullFrameEvent(img);
    } else {
        d->recordFrameEvent(img);
    }
}

void QFxTestEnginePrivate::recordFullFrameEvent(const QImage &img)
{
    TestFullFrame *ff = new TestFullFrame(q);
    ff->setTime(elapsed());
    ff->setFrameId(fullFrames.count());

    fullFrames << img;
    testData.actions() << ff;

    if (testMode == QFxTestEngine::PlaybackTest) {
        TestFullFrame *pf = qobject_cast<TestFullFrame *>(playbackTestData.next());
        QString filename = testDirectory + QLatin1String("/image") + QString::number(pf->frameId()) + QLatin1String(".png");
        QImage recImg(filename);
        if (!pf || !compare(recImg, img) || pf->time() != elapsed()) {
            qDebug() << pf << pf->time() << elapsed();
            message(Fail, "FFrame mismatch");
        } else {
            message(Success, "FFrame OK");
        }

        testPass();
    }
}

static QByteArray toHex(uchar c)
{
    QByteArray rv;
    uint h = c / 16;
    uint l = c % 16;
    if (h >= 10)
        rv.append(h - 10 + 'A');
    else
        rv.append(h + '0');
    if (l >= 10)
        rv.append(l - 10 + 'A');
    else
        rv.append(l + '0');
    return rv;
}

void QFxTestEnginePrivate::recordFrameEvent(const QImage &img)
{
    QCryptographicHash hash(QCryptographicHash::Md5);

    hash.addData((const char *)img.bits(), img.bytesPerLine() * img.height());

    QByteArray result = hash.result();
    QByteArray hexResult;
    for (int ii = 0; ii < result.count(); ++ii) 
        hexResult.append(toHex(result.at(ii)));

    TestFrame *f = new TestFrame(q);
    f->setTime(elapsed());

    f->setHash(QLatin1String(hexResult));
    testData.actions() << f;
    if (testMode == QFxTestEngine::PlaybackTest) {
        TestObject *o = playbackTestData.next();
        TestFrame *f = qobject_cast<TestFrame *>(o);
        if (!f || f->time() != elapsed() ||
                f->hash() != QLatin1String(hexResult)) {
            mismatchedFrames++;
            if (mismatchedFrames > MAX_MISMATCHED_FRAMES ||
               lastFrameMismatch)
                message(Fail, "Frame mismatch");
            else
                message(Success, "Frame mismatch - within tolerance");
            lastFrameMismatch = true;
        } else {
            message(Success, "Frame OK");
            lastFrameMismatch = false;
        }

        testPass();
    }
}

void QFxTestEnginePrivate::updateCurrentTime(int)
{
    if (status != Working)
        return;

    while(!toPost.isEmpty()) {
        int t = elapsed();
        TestObject *o = toPost.first();
        if (testMode == QFxTestEngine::RecordTest)
            o->setTime(t);
        else if (o->time() != t)
            return;
        toPost.takeFirst();
        if (TestMouse *m = qobject_cast<TestMouse *>(o)) {
            QMouseEvent e((QEvent::Type)m->type(), m->pos(), m->globalPos(), (Qt::MouseButton)m->button(), (Qt::MouseButtons)m->buttons(), (Qt::KeyboardModifiers)0);
            postedEvents.insert(&e);
            QApplication::sendEvent(canvas, &e);
        } else if (TestKey *k = qobject_cast<TestKey *>(o)) {
            QKeyEvent e((QEvent::Type)k->type(), k->key(), (Qt::KeyboardModifiers)k->modifiers(), k->text());
            postedEvents.insert(&e);
            QApplication::sendEvent(canvas, &e);
        }
    }
}

bool QFxTestEngine::eventFilter(QObject *, QEvent *event)
{
    if (d->status != QFxTestEnginePrivate::Working)
        return false;
    
    if (event->type() == QEvent::MouseButtonPress ||
       event->type() == QEvent::MouseButtonDblClick ||
       event->type() == QEvent::MouseButtonRelease ||
       event->type() == QEvent::MouseMove) {
        if (d->testMode == RecordTest && d->postedEvents.contains(event)) {
            d->postedEvents.remove(event);
        } else {
            d->recordMouseEvent(static_cast<QMouseEvent *>(event));
            return d->testMode == RecordTest;
        }
    } else if (event->type() == QEvent::KeyPress ||
              event->type() == QEvent::KeyRelease) {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if (key->key() < Qt::Key_F1 || key->key() > Qt::Key_F9)  {

            if (d->testMode == RecordTest && d->postedEvents.contains(event)) {
                d->postedEvents.remove(event);
            } else {
                d->recordKeyEvent(key);
                return d->testMode == RecordTest;
            }

        } 
    }

    return false;
}

void QFxTestEnginePrivate::recordMouseEvent(QMouseEvent *e)
{
    TestMouse *m = new TestMouse(q);
    m->setTime(elapsed());
    m->setType(e->type());
    m->setButton(e->button());
    m->setButtons(e->buttons());
    m->setGlobalPos(e->globalPos());
    m->setPos(e->pos());
    testData.actions() << m;

    if (testMode == QFxTestEngine::PlaybackTest) {
        TestMouse *m = qobject_cast<TestMouse *>(playbackTestData.next());
        if (!m || m->time() != elapsed() ||
                m->type() != e->type() ||
                m->button() != e->button() ||
                m->buttons() != e->buttons() ||
//                m->globalPos() != e->globalPos() ||
                m->pos() != e->pos()) {
            if (m)
            qWarning() << m->time() << elapsed();
            message(Fail, "Mouse mismatch");
        } else
            message(Success, "Mouse OK");

        testPass();
    } else {
        toPost << m;
    }

}

void QFxTestEnginePrivate::recordKeyEvent(QKeyEvent *e)
{
    TestKey *k = new TestKey(q);
    k->setTime(elapsed());
    k->setType(e->type());
    k->setModifiers(e->QInputEvent::modifiers());
    k->setText(e->text());
    k->setKey(e->key());
    testData.actions() << k;
    if (testMode == QFxTestEngine::PlaybackTest) {
        TestKey *f = qobject_cast<TestKey *>(playbackTestData.next());
        if (!f || f->time() != elapsed() ||
                f->type() != e->type() ||
                f->modifiers() != e->QInputEvent::modifiers() ||
                f->text() != e->text() ||
                f->key() != e->key())
            message(Fail, "Key mismatch");
        else
            message(Success, "Key OK");

        testPass();
    } else {
        toPost << k;
    }
}

void QFxTestEngine::captureFullFrame()
{
    d->fullFrame = true;
}

void QFxTestEnginePrivate::message(MessageType t, const char *message)
{
    if (exitOnFail)
        qWarning("%s", message);
    if (t == Fail) {
        if (exitOnFail) {
            save(QLatin1String("manifest-fail.qml"), false);
            qFatal("Failed");
        } else {
            status = Failed;
        }
    }
}

void QFxTestEnginePrivate::save(const QString &filename, bool images)
{
    qWarning() << "QFxTestEngine: Writing test data";

    QFile manifest(testDirectory + QLatin1String("/") + filename);
    manifest.open(QIODevice::WriteOnly);
    testData.save(&manifest);
    manifest.close();

    if (images) {
        for (int ii = 0; ii < fullFrames.count(); ++ii) 
            fullFrames.at(ii).save(testDirectory + QLatin1String("/image") + QString::number(ii) + QLatin1String(".png"));
    }
}

void QFxTestEngine::save()
{
    if (d->testMode != RecordTest)
        return;

    d->save(QLatin1String("manifest.qml"));
}

void QFxTestEnginePrivate::testPass()
{
    if (playbackTestData.atEnd()) {
        qWarning("Test PASSED");
        if (exitOnFail) {
            save(QLatin1String("manifest-play.qml"));
            exit(0);
        } else {
            status = Passed;
        }
    }
}

bool QFxTestEngine::runTest()
{
    d->exitOnFail = false;
    while(d->status == QFxTestEnginePrivate::Working)
        QApplication::processEvents();
    d->exitOnFail = true;
    qWarning() << d->status;
    return d->status == QFxTestEnginePrivate::Passed;
}

QT_END_NAMESPACE
