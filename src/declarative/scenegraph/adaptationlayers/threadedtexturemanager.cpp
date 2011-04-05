#include "threadedtexturemanager.h"

#include <QWaitCondition>
#include <QMutex>
#include <QThread>

#include <QQueue>
#include <QPaintDevice>
#include <QGLWidget>

#include <qdatetime.h>

QT_BEGIN_NAMESPACE

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

struct TextureToUpload
{
    QImage image;
    TextureReference *texture;
    TextureManager::UploadHints hints;
};

class QSGTTMUploadThread : public QThread
{
public:
    QSGTTMUploadThread(QSGThreadedTextureManager *m)
        : manager(m)
    {
    }

    void run() {

        manager->makeThreadContextCurrent();

        while (true) {

            mutex.lock();
            if (requests.isEmpty()) {
                condition.wait(&mutex);
            }

            if (requests.isEmpty()) {
                mutex.unlock();
                continue;
            }

            TextureToUpload work = requests.dequeue();
            mutex.unlock();

            if (work.image.isNull())
                continue;

            manager->uploadInThread(work.texture, work.image, work.hints);
        }
    }

    QSGThreadedTextureManager *manager;

    QWaitCondition condition;
    QMutex mutex;
    QQueue<TextureToUpload> requests;


};

class QSGThreadedTextureManagerPrivate
{
public:
    QSGTTMUploadThread *thread;
    QGLContext *context;
    QGLWidget *widget;
    QGLFunctions functions;
};

QSGThreadedTextureManager::QSGThreadedTextureManager()
{
    d = new QSGThreadedTextureManagerPrivate;
    d->thread = 0;
    d->context = 0;
    d->widget = 0;
}

const QSGTextureRef &QSGThreadedTextureManager::requestUploadedTexture(const QImage &image, UploadHints hints, QObject *listener, const char *slot)
{
    if (!d->thread) {
        d->thread = new QSGTTMUploadThread(this);
        initializeThreadContext();
        d->thread->start();
    }

    Q_ASSERT(!image.isNull());

    if (hints & SynchronousUploadHint) {
        return TextureManager::requestUploadedTexture(image, hints, listener, slot);
    }

    TextureToUpload work;
    work.texture = new TextureReference;
    work.image = image;
    work.hints = hints;

    QObject::connect(work.texture, SIGNAL(statusChanged(int)), listener, slot);

    d->thread->mutex.lock();

    d->thread->requests << work;
    d->thread->condition.wakeOne();
    d->thread->mutex.unlock();

    return work.texture;
}



/*!
    Reimplement this function to initialize a non-standard threaded context.

    This function is called from the same thread as the scene graph is running
    in. The default implementation will construct a QGLWidget, extract its
    context and use this context in the background thread.
 */
void QSGThreadedTextureManager::initializeThreadContext()
{
    QGLContext *ctx = const_cast<QGLContext *>(QGLContext::currentContext());
    // Getting the share widget from the current context is rather nasty and
    // will not work for lighthouse based
    Q_ASSERT(ctx->device() && ctx->device()->devType() == QInternal::Widget);

    QGLWidget *share = static_cast<QGLWidget *>(ctx->device());

    d->widget = new QGLWidget(0, share);
    d->widget->resize(8, 8);

    d->context = const_cast<QGLContext *>(d->widget->context());
    if (!d->context)
        qFatal("QSGThreadedTextureManager: failed to create thread context...");
    d->functions.initializeGLFunctions(d->context);

    d->widget->doneCurrent();

    ctx->makeCurrent();
}



/*!
    Reimplement this function to make the threaded context current.

    This function is called from the background thread. The default
    implementation makes the context created in initializeThreadContext
    current.
 */
void QSGThreadedTextureManager::makeThreadContextCurrent()
{
    d->context->makeCurrent();
}


/*!
    Reimplement this function to upload images in the background thread.

    This function is called from the background thread. The default
    implementation does this using a single glTexImage2D call.
 */

void QSGThreadedTextureManager::uploadInThread(TextureReference *texture, const QImage &image, UploadHints hints)
{
#ifdef QT_OPENGL_ES
    qWarning("ThreadedTextureManager: Chances are that BGRA does not work on GLES");
#endif

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.constBits());

    if (hints & TextureManager::GenerateMipmapUploadHint)
        d->functions.glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    texture->setOwnsTexture(true);
    texture->setTextureId(id);
    texture->setTextureSize(image.size());
    texture->setMipmaps(hints & TextureManager::GenerateMipmapUploadHint);
    texture->setStatus(TextureReference::Uploaded);
}

QT_END_NAMESPACE
