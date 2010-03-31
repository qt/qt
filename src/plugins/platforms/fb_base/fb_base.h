#ifndef QLIGHTHOUSEGRAPHICSSCREEN_H
#define QLIGHTHOUSEGRAPHICSSCREEN_H

#include <QtGui/private/qgraphicssystem_p.h>
#include <qrect.h>
#include <qimage.h>
#include <qtimer.h>
#include <qpainter.h>
#include <QGraphicsSystemCursor>

class QMouseEvent;
class QSize;
class QPainter;

class QGraphicsSystemFbScreen;

class QGraphicsSystemSoftwareCursor : public QGraphicsSystemCursor
{
public:
    QGraphicsSystemSoftwareCursor(QGraphicsSystemScreen * scr);

    // output methods
    QRect dirtyRect();
    virtual QRect drawCursor(QPainter & painter);

    // input methods
    virtual void pointerEvent(const QMouseEvent & event);
    virtual void changeCursor(QCursor * widgetCursor, QWidget * widget);

protected:
    QGraphicsSystemCursorImage * graphic;

private:
    void setCursor(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY);
    void setCursor(Qt::CursorShape shape);
    void setCursor(const QImage * image, int hotx, int hoty);
    QRect currentRect;      // next place to draw the cursor
    QRect prevRect;         // last place the cursor was drawn
    QRect getCurrentRect();
};

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

    virtual Qt::WindowFlags setWindowFlags(Qt::WindowFlags type);
    virtual Qt::WindowFlags windowFlags() const;

    WId winId() const { return windowId; }
protected:
    QGraphicsSystemFbScreen *mScreen;
    QRect oldGeometry;
    QImage mImage;
    bool visibleFlag;
    Qt::WindowFlags flags;

    WId windowId;
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
    virtual void addWindowSurface(QGraphicsSystemFbWindowSurface * surface) {
        windowStack.prepend(surface); invalidateRectCache(); }
    virtual void raise(QWindowSurface * surface);
    virtual void lower(QWindowSurface * surface);
    virtual QWidget * topLevelAt(const QPoint & p) const;

    QImage * image() const { return mScreenImage; }
    QPaintDevice * paintDevice() const { return mScreenImage; }

protected:
    QList<QGraphicsSystemFbWindowSurface *> windowStack;
    QRegion repaintRegion;
    QGraphicsSystemSoftwareCursor * cursor;
    QTimer redrawTimer;

protected slots:
    virtual QRegion doRedraw();

protected:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
    QImage *mScreenImage;

private:
    QPainter * compositePainter;
    void generateRects();
    QList<QPair<QRect, int> > cachedRects;

    void invalidateRectCache() { isUpToDate = false; }
    friend class QGraphicsSystemFbWindowSurface;
    bool isUpToDate;
};

#endif // QLIGHTHOUSEGRAPHICSSCREEN_H
