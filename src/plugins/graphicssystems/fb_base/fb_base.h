#ifndef QLIGHTHOUSEGRAPHICSSCREEN_H
#define QLIGHTHOUSEGRAPHICSSCREEN_H

#include <QtGui/private/qgraphicssystem_p.h>
#include <qrect.h>
#include <qimage.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qgraphicssystemcursor.h>

class QMouseEvent;
class QSize;
class QPainter;

class QGraphicsSystemFbWindowSurface;
class QGraphicsSystemFbScreen;

class QGraphicsSystemFbWindowSurface : public QWindowSurface
{
public:
    QGraphicsSystemFbWindowSurface(QGraphicsSystemFbScreen *screen, QWidget *window);
    ~QGraphicsSystemFbWindowSurface();

    virtual QPaintDevice *paintDevice() { return &mImage; }
    virtual void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    virtual bool scroll(const QRegion &area, int dx, int dy);

    virtual void beginPaint(const QRegion &region);
    virtual void endPaint(const QRegion &region);
    virtual void setVisible(bool visible);
    virtual bool visible() { return visibleFlag; }

    const QImage image() { return mImage; }
    void setGeometry(const QRect &rect);

    virtual void raise();
    virtual void lower();

protected:
    QGraphicsSystemFbScreen *mScreen;
    QRect oldGeometry;
    QImage mImage;
    bool visibleFlag;
};

class QGraphicsSystemFbScreen : public QGraphicsSystemScreen
{
    Q_OBJECT
public:
    QGraphicsSystemFbScreen();
    ~QGraphicsSystemFbScreen();

    virtual QRect geometry() const { return mGeometry; }
    virtual int depth() const { return mDepth; }
    virtual QImage::Format format() const { return mFormat; }
    virtual QSize physicalSize() const { return mPhysicalSize; }

    virtual void setGeometry(QRect rect);
    virtual void setDepth(int depth);
    virtual void setFormat(QImage::Format format);
    virtual void setPhysicalSize(QSize size);

    virtual void setDirty(const QRect &rect);

    virtual void removeWindowSurface(QGraphicsSystemFbWindowSurface * surface);
    virtual void addWindowSurface(QGraphicsSystemFbWindowSurface * surface) { windowStack.prepend(surface); }
    virtual void raise(QWindowSurface * surface);
    virtual void lower(QWindowSurface * surface);

    virtual void pointerEvent(QMouseEvent & me);

    QImage * image() const { return mScreenImage; }
    QPaintDevice * paintDevice() const { return mScreenImage; }

protected:
    QList<QGraphicsSystemFbWindowSurface *> windowStack;
    QRegion repaintRegion;
    QGraphicsSystemCursor * cursor;
    QTimer redrawTimer;

protected slots:
    virtual QRegion doRedraw();

protected:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
    QImage *mScreenImage;
};

#endif // QLIGHTHOUSEGRAPHICSSCREEN_H
