/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>
#include "tst_qglthreads.h"

#ifdef Q_WS_X11
#include <private/qt_x11_p.h>
#endif

#define RUNNING_TIME 5000

tst_QGLThreads::tst_QGLThreads(QObject *parent)
    : QObject(parent)
{
}



/*

   swapInThread

   The purpose of this testcase is to verify that it is possible to do rendering into
   a GL context from the GUI thread, then swap the contents in from a background thread.

   The usecase for this is to have the background thread do the waiting for vertical
   sync while the GUI thread is idle.

   Currently the locking is handled directly in the paintEvent(). For the actual usecase
   in Qt, the locking is done in the windowsurface before starting any drawing while
   unlocking is done after all drawing has been done.
 */


class SwapThread : public QThread
{
    Q_OBJECT
public:
    SwapThread(QGLWidget *widget)
        : m_widget(widget)
    {
        moveToThread(this);
    }

    void run() {
        QTime time;
        time.start();
        while (time.elapsed() < RUNNING_TIME) {
            lock();
            wait();

            m_widget->makeCurrent();
            m_widget->swapBuffers();
            m_widget->doneCurrent();
            unlock();
        }
    }

    void lock() { m_mutex.lock(); }
    void unlock() { m_mutex.unlock(); }

    void wait() { m_wait_condition.wait(&m_mutex); }
    void notify() { m_wait_condition.wakeAll(); }

private:
    QGLWidget *m_widget;
    QMutex m_mutex;
    QWaitCondition m_wait_condition;
};

class ForegroundWidget : public QGLWidget
{
public:
    ForegroundWidget(const QGLFormat &format)
        : QGLWidget(format), m_thread(0)
    {
        setAutoBufferSwap(false);
    }

    void paintEvent(QPaintEvent *)
    {
        m_thread->lock();
        makeCurrent();
        QPainter p(this);
        p.fillRect(rect(), QColor(rand() % 256, rand() % 256, rand() % 256));
        p.setPen(Qt::red);
        p.setFont(QFont("SansSerif", 24));
        p.drawText(rect(), Qt::AlignCenter, "This is an autotest");
        p.end();
        doneCurrent();
        m_thread->notify();
        m_thread->unlock();

        update();
    }

    void setThread(SwapThread *thread) {
        m_thread = thread;
    }

    SwapThread *m_thread;
};

void tst_QGLThreads::swapInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on mac as they were causing reboots", SkipAll);
#endif

    QGLFormat format;
    format.setSwapInterval(1);
    ForegroundWidget widget(format);
    SwapThread thread(&widget);
    widget.setThread(&thread);
    widget.show();

    QTest::qWaitForWindowShown(&widget);
    thread.start();

    while (thread.isRunning()) {
        qApp->processEvents();
    }

    widget.hide();

    QVERIFY(true);
}







/*
   textureUploadInThread

   The purpose of this testcase is to verify that doing texture uploads in a background
   thread is possible and that it works.
 */

class CreateAndUploadThread : public QThread
{
    Q_OBJECT
public:
    CreateAndUploadThread(QGLWidget *shareWidget)
    {
        m_gl = new QGLWidget(0, shareWidget);
        moveToThread(this);
    }

    ~CreateAndUploadThread()
    {
        delete m_gl;
    }

    void run() {
        m_gl->makeCurrent();
        QTime time;
        time.start();
        while (time.elapsed() < RUNNING_TIME) {
            QImage image(400, 300, QImage::Format_RGB32);
            QPainter p(&image);
            p.fillRect(image.rect(), QColor(rand() % 256, rand() % 256, rand() % 256));
            p.setPen(Qt::red);
            p.setFont(QFont("SansSerif", 24));
            p.drawText(image.rect(), Qt::AlignCenter, "This is an autotest");
            p.end();
            m_gl->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, QGLContext::InternalBindOption);
            createdAndUploaded(image);
        }
    }

signals:
    void createdAndUploaded(const QImage &image);

private:
    QGLWidget *m_gl;
};

class TextureDisplay : public QGLWidget
{
    Q_OBJECT
public:
    void paintEvent(QPaintEvent *) {
        QPainter p(this);
        for (int i=0; i<m_images.size(); ++i) {
            p.drawImage(m_positions.at(i), m_images.at(i));
            m_positions[i] += QPoint(1, 1);
        }
        update();
    }

public slots:
    void receiveImage(const QImage &image) {
        m_images << image;
        m_positions << QPoint(-rand() % width() / 2, -rand() % height() / 2);

        if (m_images.size() > 100) {
            m_images.takeFirst();
            m_positions.takeFirst();
        }
    }

private:
    QList <QImage> m_images;
    QList <QPoint> m_positions;
};

void tst_QGLThreads::textureUploadInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on mac as they were causing reboots", SkipAll);
#endif

    TextureDisplay display;
    CreateAndUploadThread thread(&display);

    connect(&thread, SIGNAL(createdAndUploaded(QImage)), &display, SLOT(receiveImage(QImage)));

    display.show();
    QTest::qWaitForWindowShown(&display);

    thread.start();

    while (thread.isRunning()) {
        qApp->processEvents();
    }

    QVERIFY(true);
}






/*
   renderInThread

   This test sets up a scene and renders it in a different thread.
   For simplicity, the scene is simply a bunch of rectangles, but
   if that works, we're in good shape..
 */

static inline float qrandom() { return (rand() % 100) / 100.f; }

void renderAScene(int w, int h)
{
#ifdef QT_OPENGL_ES_2
            QGLShaderProgram program;
            program.addShaderFromSourceCode(QGLShader::Vertex, "attribute highp vec2 pos; void main() { gl_Position = vec4(pos.xy, 1.0, 1.0); }");
            program.addShaderFromSourceCode(QGLShader::Fragment, "uniform lowp vec4 color; void main() { gl_FragColor = color; }");
            program.bindAttributeLocation("pos", 0);
            program.bind();
            int colorId = program.uniformLocation("color");

            glEnableVertexAttribArray(0);

            for (int i=0; i<1000; ++i) {
                GLfloat pos[] = {
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.,
                    (rand() % 100) / 100.
                };

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, pos);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
            }
#else
            glViewport(0, 0, w, h);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glFrustum(0, w, h, 0, 1, 100);
            glTranslated(0, 0, -1);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            for (int i=0;i<1000; ++i) {
                glBegin(GL_TRIANGLES);
                glColor3f(qrandom(), qrandom(), qrandom());
                glVertex2f(qrandom() * w, qrandom() * h);
                glColor3f(qrandom(), qrandom(), qrandom());
                glVertex2f(qrandom() * w, qrandom() * h);
                glColor3f(qrandom(), qrandom(), qrandom());
                glVertex2f(qrandom() * w, qrandom() * h);
                glEnd();
            }
#endif
}

class ThreadSafeGLWidget : public QGLWidget
{
public:
    void paintEvent(QPaintEvent *)
    {
        // ignored as we're anyway swapping as fast as we can
    };

    void resizeEvent(QResizeEvent *e)
    {
        mutex.lock();
        newSize = e->size();
        mutex.unlock();
    };

    QMutex mutex;
    QSize newSize;
};

class SceneRenderingThread : public QThread
{
    Q_OBJECT
public:
    SceneRenderingThread(ThreadSafeGLWidget *widget)
        : m_widget(widget)
    {
        moveToThread(this);
        m_size = widget->size();
    }

    void run() {
        QTime time;
        time.start();
        failure = false;

        m_widget->makeCurrent();

        while (time.elapsed() < RUNNING_TIME && !failure) {


            m_widget->mutex.lock();
            QSize s = m_widget->newSize;
            m_widget->mutex.unlock();

            if (s != m_size) {
                glViewport(0, 0, s.width(), s.height());
            }

            if (QGLContext::currentContext() != m_widget->context()) {
                failure = true;
                break;
            }

            glClear(GL_COLOR_BUFFER_BIT);

            int w = m_widget->width();
            int h = m_widget->height();

            renderAScene(w, h);

            int color;
            glReadPixels(w / 2, h / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);

            m_widget->swapBuffers();
        }

        m_widget->doneCurrent();
    }

    bool failure;

private:
    ThreadSafeGLWidget *m_widget;
    QSize m_size;
};

void tst_QGLThreads::renderInThread_data()
{
    QTest::addColumn<bool>("resize");
    QTest::addColumn<bool>("update");

    QTest::newRow("basic") << false << false;
    QTest::newRow("with-resize") << true << false;
    QTest::newRow("with-update") << false << true;
    QTest::newRow("with-resize-and-update") << true << true;
}

void tst_QGLThreads::renderInThread()
{
#ifdef Q_OS_MAC
    QSKIP("OpenGL threading tests are currently disabled on mac as they were causing reboots", SkipAll);
#endif

    QFETCH(bool, resize);
    QFETCH(bool, update);

    ThreadSafeGLWidget widget;
    widget.resize(200, 200);
    SceneRenderingThread thread(&widget);

    widget.show();
    QTest::qWaitForWindowShown(&widget);
    widget.doneCurrent();

    thread.start();

    int value = 10;
    while (thread.isRunning()) {
        if (resize)
            widget.resize(200 + value, 200 + value);
        if (update)
            widget.update(100 + value, 100 + value, 20, 20);
        qApp->processEvents();
        value = -value;

#ifdef Q_WS_WIN
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
    }

    QVERIFY(!thread.failure);
}




int main(int argc, char **argv)
{
#ifdef Q_WS_X11
    XInitThreads();
#endif

    QApplication app(argc, argv);
    QTEST_DISABLE_KEYPAD_NAVIGATION \

    tst_QGLThreads tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_qglthreads.moc"
