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
    Q_UNUSED(widget);
    if (cursor->shape() != Qt::BitmapCursor) {
        image->set(cursor->shape());
    } else {
        qDebug() << "non-shape cursor: not implemented yet";
        return;
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

    QDirectFbConvenience::dfbInterface()->CreateSurface(QDirectFbConvenience::dfbInterface(), &surfaceDesc, &surface);

    blitter = new QDirectFbBlitter(imageRect, surface);
    blitter->drawPixmap(imageRect, map, imageRect);

    int xSpot = image->hotspot().x();
    int ySpot = image->hotspot().y();

    if (m_layer->SetCooperativeLevel(m_layer, DLSCL_ADMINISTRATIVE) != DFB_OK) {
        return;
    }
    m_layer->SetCursorShape( m_layer, surface, xSpot, ySpot);
    m_layer->SetCooperativeLevel(m_layer, DLSCL_SHARED);
}
