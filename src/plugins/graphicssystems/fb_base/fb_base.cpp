#include "fb_base.h"

#include <qpainter.h>
#include <qdebug.h>
#include <qbitmap.h>
#include <qgraphicssystemcursor.h>
#include <QWindowSystemInterface>

QGraphicsSystemSoftwareCursor::QGraphicsSystemSoftwareCursor(QGraphicsSystemScreen *scr)
        : QGraphicsSystemCursor(scr), currentRect(QRect()), prevRect(QRect())
{
    graphic = new QGraphicsSystemCursorImage(0, 0, 0, 0, 0, 0);
    setCursor(Qt::ArrowCursor);
}

QRect QGraphicsSystemSoftwareCursor::getCurrentRect()
{
    QRect rect = graphic->image()->rect().translated(-graphic->hotspot().x(),
                                                     -graphic->hotspot().y());
    rect.translate(QCursor::pos());
    return rect;
}


void QGraphicsSystemSoftwareCursor::pointerEvent(const QMouseEvent & e)
{
    Q_UNUSED(e);
    currentRect = getCurrentRect();
    screen->setDirty(currentRect);
}

QRect QGraphicsSystemSoftwareCursor::drawCursor(QPainter & painter)
{
    if (currentRect.isNull())
        return QRect();

    prevRect = currentRect;
    painter.drawImage(prevRect, *graphic->image());
    return prevRect;
}

QRect QGraphicsSystemSoftwareCursor::dirtyRect()
{
    if (!prevRect.isNull()) {
        QRect rect = prevRect;
        prevRect = QRect();
        return rect;
    }
    return QRect();
}

void QGraphicsSystemSoftwareCursor::setCursor(Qt::CursorShape shape)
{
    graphic->set(shape);
}

void QGraphicsSystemSoftwareCursor::setCursor(const QImage * image, int hotx, int hoty)
{
    graphic->set(image, hotx, hoty);
}

void QGraphicsSystemSoftwareCursor::setCursor(const uchar *data, const uchar *mask, int width, int height, int hotX, int hotY)
{
    graphic->set(data, mask, width, height, hotX, hotY);
}

void QGraphicsSystemSoftwareCursor::changeCursor(QCursor * widgetCursor, QWidget * widget)
{
    Q_UNUSED(widget);
    Qt::CursorShape shape = widgetCursor->shape();

    if (shape == Qt::BitmapCursor) {
        // application supplied cursor
        QPoint spot = widgetCursor->hotSpot();
        setCursor(&widgetCursor->pixmap().toImage(), spot.x(), spot.y());
    } else {
        // system cursor
        setCursor(shape);
    }
    currentRect = getCurrentRect();
    screen->setDirty(currentRect);
}

QGraphicsSystemFbScreen::QGraphicsSystemFbScreen() : cursor(0), mGeometry(), mDepth(16), mFormat(QImage::Format_RGB16), mScreenImage(0)
{
    mScreenImage = new QImage(mGeometry.size(), mFormat);
    redrawTimer.setSingleShot(true);
    redrawTimer.setInterval(0);
    QObject::connect(&redrawTimer, SIGNAL(timeout()), this, SLOT(doRedraw()));
}

void QGraphicsSystemFbScreen::setGeometry(QRect rect)
{
    delete mScreenImage;
    mGeometry = rect;
    mScreenImage = new QImage(mGeometry.size(), mFormat);
}

void QGraphicsSystemFbScreen::setDepth(int depth)
{
    mDepth = depth;
}

void QGraphicsSystemFbScreen::setPhysicalSize(QSize size)
{
    mPhysicalSize = size;
}

void QGraphicsSystemFbScreen::setFormat(QImage::Format format)
{
    mFormat = format;
    delete mScreenImage;
    mScreenImage = new QImage(mGeometry.size(), mFormat);
}

QGraphicsSystemFbScreen::~QGraphicsSystemFbScreen()
{
    delete mScreenImage;
}

void QGraphicsSystemFbScreen::setDirty(const QRect &rect)
{
    repaintRegion += rect;
    if (!redrawTimer.isActive()) {
        redrawTimer.start();
    }
}

QRegion QGraphicsSystemFbScreen::doRedraw()
{
    QRegion touchedRegion;
    if (cursor)
        repaintRegion += cursor->dirtyRect();
    if (repaintRegion.isEmpty())
        return touchedRegion;

    QPainter compositePainter(mScreenImage);
    QVector<QRect> rects = repaintRegion.rects();

    for (int rectIndex = 0; rectIndex < repaintRegion.numRects(); rectIndex++) {
        QRect rect = rects[rectIndex];
        // Blank the affected area, just in case there's nothing to display
        compositePainter.fillRect(rect, Qt::black);
        for (int i = windowStack.length() - 1; i >= 0; i--) {
            if (!windowStack[i]->visible())
                continue;
            if (windowStack[i]->window()->isMinimized())
                continue;
            QRect windowRect = windowStack[i]->geometry();
            QRect intersect = windowRect.intersected(rect);
            QRect windowIntersect = intersect.translated(-windowRect.left(),
                                                         -windowRect.top());
            if (intersect.isNull())
                continue;
            compositePainter.drawImage(intersect, windowStack[i]->image(),
                                       windowIntersect);
        }
    }

    QRect cursorRect;
    if (cursor) {
        cursorRect = cursor->drawCursor(compositePainter);
        touchedRegion += cursorRect;
    }
    touchedRegion += repaintRegion;
    repaintRegion = QRegion();

    return touchedRegion;
}

void QGraphicsSystemFbScreen::removeWindowSurface(QGraphicsSystemFbWindowSurface * surface)
{
    windowStack.removeOne(surface);
    setDirty(surface->geometry());
}

void QGraphicsSystemFbWindowSurface::raise()
{
    mScreen->raise(this);
}

void QGraphicsSystemFbScreen::raise(QWindowSurface * surface)
{
    QGraphicsSystemFbWindowSurface *s = static_cast<QGraphicsSystemFbWindowSurface *>(surface);
    int index = windowStack.indexOf(s);
    if (index <= 0)
        return;
    windowStack.move(index, 0);
    setDirty(s->geometry());
}

void QGraphicsSystemFbWindowSurface::lower()
{
    mScreen->lower(this);
}

void QGraphicsSystemFbScreen::lower(QWindowSurface * surface)
{
    QGraphicsSystemFbWindowSurface *s = static_cast<QGraphicsSystemFbWindowSurface *>(surface);
    int index = windowStack.indexOf(s);
    if (index == -1 || index == (windowStack.size() - 1))
        return;
    windowStack.move(index, windowStack.size() - 1);
    setDirty(s->geometry());
}

QWidget * QGraphicsSystemFbScreen::topLevelAt(const QPoint & p) const
{
    for(int i = 0; i < windowStack.size(); i++) {
        if (windowStack[i]->geometry().contains(p, false) &&
            windowStack[i]->visible() &&
            !windowStack[i]->window()->isMinimized()) {
            return windowStack[i]->window();
        }
    }
    return 0;
}

QGraphicsSystemFbWindowSurface::QGraphicsSystemFbWindowSurface(QGraphicsSystemFbScreen *screen, QWidget *window)
    : QWindowSurface(window),
      mScreen(screen),
      visibleFlag(false)
{
    static QAtomicInt winIdGenerator(1);

    mImage = QImage(window->size(), mScreen->format());
    windowId = winIdGenerator.fetchAndAddRelaxed(1);
}

QGraphicsSystemFbWindowSurface::~QGraphicsSystemFbWindowSurface()
{
    mScreen->removeWindowSurface(this);
}

void QGraphicsSystemFbWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);

    QRect currentGeometry = geometry();
    // If this is a move, redraw the previous location
    if (oldGeometry != currentGeometry) {
        mScreen->setDirty(oldGeometry);
        oldGeometry = currentGeometry;
    }

    QRect dirtyClient = region.boundingRect();
    QRect dirtyRegion(currentGeometry.left() + dirtyClient.left(),
                      currentGeometry.top() + dirtyClient.top(),
                      dirtyClient.width(),
                      dirtyClient.height());
    mScreen->setDirty(dirtyRegion);
}

void QGraphicsSystemFbWindowSurface::setGeometry(const QRect &rect)
{
    // store previous geometry for screen update
    oldGeometry = geometry();

    // change the widget's QImage if this is a resize
    if (mImage.size() != rect.size())
        mImage = QImage(rect.size(), mScreen->format());

    QWindowSystemInterface::handleGeometryChange(window(), rect);

    QWindowSurface::setGeometry(rect);
}

bool QGraphicsSystemFbWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    return QWindowSurface::scroll(area, dx, dy);
}

void QGraphicsSystemFbWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QGraphicsSystemFbWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QGraphicsSystemFbWindowSurface::setVisible(bool visible)
{
    visibleFlag = visible;
    mScreen->setDirty(geometry());
}

Qt::WindowFlags QGraphicsSystemFbWindowSurface::setWindowFlags(Qt::WindowFlags type)
{
    flags = type;
    return flags;
}

Qt::WindowFlags QGraphicsSystemFbWindowSurface::windowFlags() const
{
    return flags;
}
