#include "qwaylandglcontext.h"

#include "qwaylanddisplay.h"
#include "qwaylandwindow.h"
#include "qwaylandwindowsurface.h"
#include "qfontconfigdatabase.h"

#include <QImageReader>
#include <QWindowSystemInterface>
#include <QPlatformCursor>
#include <QPaintEngine>

#include <QtGui/QPlatformGLContext>
#include <QtGui/QPlatformWindowFormat>

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>

#include <private/qwindowsurface_gl_p.h>
#include <private/qpixmapdata_gl_p.h>
#include <private/qpaintengineex_opengl2_p.h>

#include <unistd.h>
#include <fcntl.h>

extern "C" {
#include <xf86drm.h>
}

QWaylandGLContext::QWaylandGLContext(QWaylandDisplay *wd, QWaylandWindow *window, const QPlatformWindowFormat &format)
    : QPlatformGLContext()
    , mFormat(format)
    , mDisplay(wd)
    , mWindow(window)
    , parentFbo(0)
    , parentRbo(0)
{
}

QWaylandGLContext::~QWaylandGLContext()
{
    glDeleteRenderbuffers(1, &parentRbo);
    glDeleteFramebuffers(1, &parentFbo);
}

void QWaylandGLContext::makeCurrent()
{
    QWaylandDrmBuffer *mBuffer = (QWaylandDrmBuffer *)mWindow->getBuffer();
    QRect geometry = mWindow->geometry();

    if (!mBuffer)
	return;

    eglMakeCurrent(mDisplay->eglDisplay(), 0, 0, mBuffer->mContext);

    glViewport(0, 0, geometry.width(), geometry.height());
    glBindFramebuffer(GL_FRAMEBUFFER, mBuffer->mFbo);
    glBindTexture(GL_TEXTURE_2D, mBuffer->mTexture);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mBuffer->mImage);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			   GL_TEXTURE_2D, mBuffer->mTexture, 0);
}

void QWaylandGLContext::doneCurrent()
{
}

/* drawTexture - Draw from a texture into a the current framebuffer
 * @rect: GL normalized coords for drawing (between -1.0f and 1.0f)
 * @tex_id: texture source
 * @texSize: size of source rectangle in Qt coords
 * @br: bounding rect for drawing
 */
static void drawTexture(const QRectF &rect, GLuint tex_id,
			const QSize &texSize, const QRectF &br)
{
    QRectF src = br.isEmpty()
        ? QRectF(QPointF(), texSize)
        : QRectF(QPointF(br.x(), texSize.height() - br.bottom()), br.size());
    qreal width = texSize.width();
    qreal height = texSize.height();

    src.setLeft(src.left() / width);
    src.setRight(src.right() / width);
    src.setTop(src.top() / height);
    src.setBottom(src.bottom() / height);

    const GLfloat tx1 = src.left();
    const GLfloat tx2 = src.right();
    const GLfloat ty1 = src.top();
    const GLfloat ty2 = src.bottom();

    GLfloat texCoordArray[4*2] = {
        tx1, ty2, tx2, ty2, tx2, ty1, tx1, ty1
    };

    GLfloat vertexArray[4*2];
    extern void qt_add_rect_to_array(const QRectF &r, GLfloat *array);
    qt_add_rect_to_array(rect, vertexArray);

    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, vertexArray);
    glVertexAttribPointer(QT_TEXTURE_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, texCoordArray);

    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glEnableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glDisableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void QWaylandGLContext::swapBuffers()
{
    QWaylandWindow *mParentWindow = mWindow->getParentWindow();
    QWaylandDrmBuffer *mBuffer, *mParentBuffer;
    QRect geometry = mWindow->geometry(), parentGeometry;
    QGLShaderProgram *blitProgram;
    QRectF r;
    qreal w;
    qreal h;

    if (!mParentWindow) {
	qDebug("swap without parent widget?\n");
	return;
    }

    if (!mParentWindow->surface()) {
	qDebug("parent has no surface??\n");
	return;
    }

    parentGeometry = mParentWindow->geometry();
    mBuffer = (QWaylandDrmBuffer *)mWindow->getBuffer();
    mParentBuffer = (QWaylandDrmBuffer *)mParentWindow->getBuffer();

    glDisable(GL_DEPTH_TEST);

    /* These need to be generated against the src context */
    if (!parentFbo)
	glGenFramebuffers(1, &parentFbo);
    if (!parentRbo)
	glGenRenderbuffers(1, &parentRbo);

    glBindFramebuffer(GL_FRAMEBUFFER, parentFbo);
    glBindRenderbuffer(GL_RENDERBUFFER, parentRbo);
    glEGLImageTargetRenderbufferStorageOES(GL_RENDERBUFFER,
					   mParentBuffer->mImage);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			      GL_RENDERBUFFER, parentRbo);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mBuffer->mImage);
    glViewport(0, 0, parentGeometry.width(), parentGeometry.height());

    blitProgram = QGLEngineSharedShaders::shadersForContext(QGLContext::currentContext())->blitProgram();
    blitProgram->bind();
    blitProgram->setUniformValue("imageTexture", 0);

    /* Transform the target rect to the appropriate coords on the parent */
    w = parentGeometry.width();
    h = parentGeometry.height();

    r.setLeft((geometry.left() / w) * 2.0f - 1.0f);
    if (geometry.right() == (parentGeometry.width() - 1))
	r.setRight(1.0f);
    else
	r.setRight((geometry.right() / w) * 2.0f - 1.0f);

    r.setTop((geometry.top() / h) * 2.0f - 1.0f);
    if (geometry.bottom() == (parentGeometry.height() - 1))
       r.setBottom(-1.0f);
    else
	r.setBottom((geometry.bottom() / h) * 2.0f - 1.0f);

    drawTexture(r, mBuffer->mTexture, mParentWindow->widget()->size(), parentGeometry);

    wl_surface_damage(mParentWindow->surface(), geometry.left(), geometry.top(),
		      geometry.right(), geometry.bottom());
    /* restore things to the last valid GL state */
    makeCurrent();
    /* hack: avoid tight swapBuffers loops */
    usleep(20000);
}

void *QWaylandGLContext::getProcAddress(const QString &string)
{
    return (void *) eglGetProcAddress(string.toLatin1().data());
}

QPlatformGLContext *QWaylandWindow::glContext() const
{
    if (!mGLContext) {
        QWaylandWindow *that = const_cast<QWaylandWindow *>(this);
        that->mGLContext = new QWaylandGLContext(mDisplay, that, widget()->platformWindowFormat());
    }

    return mGLContext;
}

