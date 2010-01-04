#include "qdirectfbcursor.h"
#include "qblitter_directfb.h"
#include "qgraphicssystem_minimaldfb.h"
#include "qdirectfbconvenience.h"

QDirectFBCursor::QDirectFBCursor(QGraphicsSystemScreen * screen) :
        QGraphicsSystemCursor(screen), surface(0)
{
    QDirectFbGraphicsSystemScreen * s;
    s = static_cast<QDirectFbGraphicsSystemScreen *>(screen);
    m_layer = s->m_layer;
    image = new QGraphicsSystemCursorImage(0, 0, 0, 0, 0, 0);
}

void QDirectFBCursor::changeCursor(QCursor * cursor, QWidget * widget)
{
    qDebug() << "cursor change request:" << cursor << widget;

    if (cursor->shape() != Qt::BitmapCursor) {
        image->set(cursor->shape());
    } else {
        qDebug() << "non-shape cursor: not implemented yet";
        return; // not attempting to deal with application supplied cursors yet
    }

    // create a window to show the cursor for debugging purposes
    static IDirectFBWindow * window = 0;
    static IDirectFBSurface * windowSurface;
    qDebug() << "window is" << window;
    if (!window) {
        qDebug() << "creating cursor preview window";
        DFBWindowDescription description;
        description.width = 32;
        description.height = 32;
        description.flags = DFBWindowDescriptionFlags(DSDESC_WIDTH | DSDESC_HEIGHT);
        qDebug() << m_layer->CreateWindow(m_layer, &description,&window);
        qDebug() << window->RaiseToTop(window);
        qDebug() << window->MoveTo(window, 500, 500);
        qDebug() << window->GetSurface(window, &windowSurface);
    }

    QImage *i = image->image();
    QRect imageRect = i->rect();
    QPixmap map = QPixmap::fromImage(*i);

    if (surface)
        surface->Release(surface);
    DFBSurfaceDescription surfaceDesc;
    surfaceDesc.width = map.width();
    surfaceDesc.height = map.height();
    surfaceDesc.flags = DFBSurfaceDescriptionFlags(DSDESC_WIDTH | DSDESC_HEIGHT);

    IDirectFB *dfb = QDirectFbConvenience::dfbInterface();
    QDirectFbConvenience::dfbInterface()->CreateSurface(QDirectFbConvenience::dfbInterface(), &surfaceDesc, &surface);

    blitter = new QDirectFbBlitter(imageRect, windowSurface); // jeremy - should be surface
    blitter->drawPixmap(imageRect, map, imageRect);


    //
    // blit to the window
    //
    //windowSurface->Blit(windowSurface, surface, NULL, 0, 0);

    int xSpot = cursor->hotSpot().x();
    int ySpot = cursor->hotSpot().y();
    // The SetCursorShape() call fails if the cooperative level is DLSCL_SHARED
    // Question is, how can we determine the level, to reset is properly after?
    m_layer->SetCooperativeLevel(m_layer, DLSCL_ADMINISTRATIVE);
    DFBResult res = m_layer->SetCursorShape( m_layer, surface, xSpot, ySpot);
    m_layer->SetCooperativeLevel(m_layer, DLSCL_SHARED);    // This may be wrong. could be DFSCL_FULLSCREEN or DLSCL_ADMINISTRATIVE
    qDebug() << "setCursorShape result:";
    switch(res) {
    case DFB_OK: qDebug() << "OK"; break;
    case DFB_INVARG: qDebug() << "DFB_INVARG"; break;
    case DFB_ACCESSDENIED: qDebug() << "DFB_ACCESSDENIED"; break;
    default: qDebug() << "unknown error" << res;
    }
}
