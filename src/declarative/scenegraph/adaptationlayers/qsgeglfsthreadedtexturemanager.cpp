#include "qsgeglfsthreadedtexturemanager.h"

#include <QThread>
#include <qdatetime.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>


class QSGEglFSThreadedTextureManagerPrivate
{
public:
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    bool uploadsScanlines;
};


QSGEglFSThreadedTextureManager::QSGEglFSThreadedTextureManager()
    : d(new QSGEglFSThreadedTextureManagerPrivate)
{
    d->uploadsScanlines = false;
}


void QSGEglFSThreadedTextureManager::setUploadsScanlines(bool b)
{
    d->uploadsScanlines = b;
}


bool QSGEglFSThreadedTextureManager::uploadsScanlines() const
{
    return d->uploadsScanlines;
}


void QSGEglFSThreadedTextureManager::initializeThreadContext()
{
    d->display = eglGetCurrentDisplay();

    EGLint attribs[] = {
        EGL_WIDTH, 8,
        EGL_HEIGHT, 8,

        EGL_NONE
    };

    EGLConfig configs[256];
    EGLint count;

    d->surface = 0;
    EGLConfig *config = 0;
    if (eglGetConfigs(d->display, configs, 256, &count)) {
        for (int i=0; i<count; ++i) {
            int type;
            eglGetConfigAttrib(d->display, configs[i], EGL_RENDERABLE_TYPE, &type);
            if (!(type & EGL_OPENGL_ES2_BIT))
                continue;

            int r, g, b, a;
            eglGetConfigAttrib(d->display, configs[i], EGL_RED_SIZE, &r);
            eglGetConfigAttrib(d->display, configs[i], EGL_GREEN_SIZE, &g);
            eglGetConfigAttrib(d->display, configs[i], EGL_BLUE_SIZE, &b);
            eglGetConfigAttrib(d->display, configs[i], EGL_ALPHA_SIZE, &a);

            if ((r & g & b & a) == 8) {
                d->surface = eglCreatePbufferSurface(d->display, configs[i], attribs);
                config = configs + i;
                break;
            }
        }
    }
    d->context = eglCreateContext(d->display, *config, eglGetCurrentContext(), 0);
}


void QSGEglFSThreadedTextureManager::makeThreadContextCurrent()
{
    eglMakeCurrent(d->display, d->surface, d->surface, d->context);
}

class ThreadAccess  : public QThread
{
public:
    static void doSleep(int us) {
        usleep(us);
    }
};


static inline void qgl_byteSwapImage(QImage &img)
{
    const int width = img.width();
    const int height = img.height();

    for (int i = 0; i < height; ++i) {
        uint *p = (uint *) img.scanLine(i);
        for (int x = 0; x < width; ++x)
            p[x] = ((p[x] << 16) & 0xff0000) | ((p[x] >> 16) & 0xff) | (p[x] & 0xff00ff00);
    }
}

#define DO_TIMING

void QSGEglFSThreadedTextureManager::uploadInThread(TextureReference *texture, const QImage &im, UploadHints hints)
{
#ifdef DO_TIMING
    QTime time;
    time.start();
#endif

    QImage image = im;
    qgl_byteSwapImage(image);

#ifdef DO_TIMING
    int time_byteswap = time.elapsed();
#endif

    int w = image.width();
    int h = image.height();
    int bpl = image.bytesPerLine();
    const uchar *bits = image.constBits();

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    if (d->uploadsScanlines) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        for (int y=0; y<h; ++y) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, w, 1, GL_RGBA, GL_UNSIGNED_BYTE, y * bpl + bits);
            if (glGetError() != GL_NO_ERROR) {
                qWarning("ERROR: failed to upload chunk...\n");
                break;
            }
            ThreadAccess::doSleep(2);
        }
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
        if (glGetError() != GL_NO_ERROR) {
            qWarning("ERROR: failed to upload chunk...\n");
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);

#ifdef DO_TIMING
    int time_upload = time.elapsed();
    printf("EGLFS Threaded upload: byteSwap: %d ms, upload: %d ms, id: %d\n",
           time_byteswap,
           time_upload,
           id);
#endif

    texture->setAlphaChannel(image.hasAlphaChannel());
    texture->setOwnsTexture(true);
    texture->setTextureId(id);
    texture->setTextureSize(image.size());
    texture->setMipmaps(hints & TextureManager::GenerateMipmapUploadHint);
    texture->setStatus(TextureReference::Uploaded);
}


