#include "fb_base.h"

#include <qpainter.h>
#include <qdebug.h>
#include <qbitmap.h>
#include <QGraphicsSystemCursor>
#include <QWindowSystemInterface>

QGraphicsSystemSoftwareCursor::QGraphicsSystemSoftwareCursor(QPlatformScreen *scr)
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

QFbScreen::QFbScreen() : cursor(0), mGeometry(), mDepth(16), mFormat(QImage::Format_RGB16), mScreenImage(0), compositePainter(0), isUpToDate(false)
{
    mScreenImage = new QImage(mGeometry.size(), mFormat);
    redrawTimer.setSingleShot(true);
    redrawTimer.setInterval(0);
    QObject::connect(&redrawTimer, SIGNAL(timeout()), this, SLOT(doRedraw()));
}

void QFbScreen::setGeometry(QRect rect)
{
    delete mScreenImage;
    mGeometry = rect;
    mScreenImage = new QImage(mGeometry.size(), mFormat);
    delete compositePainter;
    compositePainter = 0;
    invalidateRectCache();
}

void QFbScreen::setDepth(int depth)
{
    mDepth = depth;
}

void QFbScreen::setPhysicalSize(QSize size)
{
    mPhysicalSize = size;
}

void QFbScreen::setFormat(QImage::Format format)
{
    mFormat = format;
    delete mScreenImage;
    mScreenImage = new QImage(mGeometry.size(), mFormat);
    delete compositePainter;
    compositePainter = 0;
}

QFbScreen::~QFbScreen()
{
    delete compositePainter;
    delete mScreenImage;
}

void QFbScreen::setDirty(const QRect &rect)
{

//    qDebug() << "QFbScreen::setDirty" << rect;
    repaintRegion += rect;
    if (!redrawTimer.isActive()) {
        redrawTimer.start();
    }
}

void QFbScreen::generateRects()
{
    cachedRects.clear();
    QRegion remainingScreen(mGeometry);

    for (int i = 0; i < windowStack.length(); i++) {
        if (remainingScreen.isEmpty())
            break;
        if (!windowStack[i]->visible())
            continue;

        if (!windowStack[i]->widget()->testAttribute(Qt::WA_TranslucentBackground)) {
            remainingScreen -= windowStack[i]->geometry();
            QRegion windowRegion(windowStack[i]->geometry());
            windowRegion -= remainingScreen;
            foreach(QRect rect, windowRegion.rects()) {
                cachedRects += QPair<QRect, int>(rect, i);
            }
        }
    }
    foreach (QRect rect, remainingScreen.rects())
        cachedRects += QPair<QRect, int>(rect, -1);
    isUpToDate = true;
    return;
}



QRegion QFbScreen::doRedraw()
{
    QRegion touchedRegion;
    if (cursor)
        repaintRegion += cursor->dirtyRect();
    if (repaintRegion.isEmpty())
        return touchedRegion;

    QVector<QRect> rects = repaintRegion.rects();

    if (!isUpToDate)
        generateRects();

    if (!compositePainter)
        compositePainter = new QPainter(mScreenImage);
    for (int rectIndex = 0; rectIndex < repaintRegion.numRects(); rectIndex++) {
        QRegion rectRegion = rects[rectIndex];

        for(int i = 0; i < cachedRects.length(); i++) {
            QRect screenSubRect = cachedRects[i].first;
            int layer = cachedRects[i].second;
            QRegion intersect = rectRegion.intersected(screenSubRect);

            if (intersect.isEmpty())
                continue;

            rectRegion -= intersect;

            // we only expect one rectangle, but defensive coding...
            foreach (QRect rect, intersect.rects()) {
                bool firstLayer = true;
                if (layer == -1) {
                    compositePainter->fillRect(rect, Qt::black);
                    firstLayer = false;
                    layer = windowStack.size() - 1;
                }

                for (int layerIndex = layer; layerIndex != -1; layerIndex--) {
                    if (!windowStack[layerIndex]->visible())
                        continue;
                    if (windowStack[layerIndex]->widget()->isMinimized())
                        continue;
                    QRect windowRect = windowStack[layerIndex]->geometry();
                    QRect windowIntersect = rect.translated(-windowRect.left(),
                                                                 -windowRect.top());
//                    qDebug() << "      compositing" << layerIndex << windowStack[layerIndex]->surface->image().size();
                    compositePainter->drawImage(rect, windowStack[layerIndex]->surface->image(),
                                                windowIntersect);
                    if (firstLayer) {
                        firstLayer = false;
                    }
                }
            }
        }
        if (!rectRegion.isEmpty())
            qWarning() << "non-empty region!" << rectRegion;
        // Everything on screen should be mapped to a sub-rectangle
        // unless it's off the screen...
    }

    QRect cursorRect;
    if (cursor) {
        cursorRect = cursor->drawCursor(*compositePainter);
        touchedRegion += cursorRect;
    }
    touchedRegion += repaintRegion;
    repaintRegion = QRegion();



//    qDebug() << "QFbScreen::doRedraw"  << windowStack.size() << mScreenImage->size() << touchedRegion;


    return touchedRegion;
}

void QFbScreen::removeWindow(QFbWindow * surface)
{
    windowStack.removeOne(surface);
    invalidateRectCache();
    setDirty(surface->geometry());
}

void QFbWindow::raise()
{
    mScreen->raise(this);
}

void QFbScreen::raise(QPlatformWindow * surface)
{
    QFbWindow *s = static_cast<QFbWindow *>(surface);
    int index = windowStack.indexOf(s);
    if (index <= 0)
        return;
    windowStack.move(index, 0);
    invalidateRectCache();
    setDirty(s->geometry());
}

void QFbWindow::lower()
{
    mScreen->lower(this);
}

void QFbScreen::lower(QPlatformWindow * surface)
{
    QFbWindow *s = static_cast<QFbWindow *>(surface);
    int index = windowStack.indexOf(s);
    if (index == -1 || index == (windowStack.size() - 1))
        return;
    windowStack.move(index, windowStack.size() - 1);
    invalidateRectCache();
    setDirty(s->geometry());
}

QWidget * QFbScreen::topLevelAt(const QPoint & p) const
{
    for(int i = 0; i < windowStack.size(); i++) {
        if (windowStack[i]->geometry().contains(p, false) &&
            windowStack[i]->visible() &&
            !windowStack[i]->widget()->isMinimized()) {
            return windowStack[i]->widget();
        }
    }
    return 0;
}

QFbWindow::QFbWindow(QFbScreen *screen, QWidget *window)
    :QPlatformWindow(window),
      mScreen(screen),
      visibleFlag(false)
{
    static QAtomicInt winIdGenerator(1);
    windowId = winIdGenerator.fetchAndAddRelaxed(1);
}


QFbWindow::~QFbWindow()
{
    mScreen->removeWindow(this);
}


QFbWindowSurface::QFbWindowSurface(QFbScreen *screen, QWidget *window)
    : QWindowSurface(window),
      mScreen(screen)
{
    mImage = QImage(window->size(), mScreen->format());

    platformWindow = static_cast<QFbWindow*>(window->platformWindow());
    platformWindow->surface = this;
}

QFbWindowSurface::~QFbWindowSurface()
{
}

void QFbWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);


//    qDebug() << "QFbWindowSurface::flush" << region;


    platformWindow->repaint(region);
}


void QFbWindow::repaint(const QRegion &region)
{
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

void QFbWindowSurface::resize(const QSize &size)
{
    // change the widget's QImage if this is a resize
    if (mImage.size() != size)
        mImage = QImage(size, mScreen->format());
    QWindowSurface::resize(size);
}

void QFbWindow::setGeometry(const QRect &rect)
{
// store previous geometry for screen update
    oldGeometry = geometry();


    mScreen->invalidateRectCache();
//###    QWindowSystemInterface::handleGeometryChange(window(), rect);

    QPlatformWindow::setGeometry(rect);
}

bool QFbWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    return QWindowSurface::scroll(area, dx, dy);
}

void QFbWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QFbWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QFbWindow::setVisible(bool visible)
{
    visibleFlag = visible;
    mScreen->invalidateRectCache();
    mScreen->setDirty(geometry());
}

Qt::WindowFlags QFbWindow::setWindowFlags(Qt::WindowFlags type)
{
    flags = type;
    mScreen->invalidateRectCache();
    return flags;
}

Qt::WindowFlags QFbWindow::windowFlags() const
{
    return flags;
}
