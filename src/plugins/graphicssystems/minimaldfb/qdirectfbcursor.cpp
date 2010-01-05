#include "qdirectfbcursor.h"
#include "qblitter_directfb.h"
#include "qgraphicssystem_minimaldfb.h"
#include "qdirectfbconvenience.h"

#include <private/qpixmap_blitter_p.h>

QDirectFBCursor::QDirectFBCursor(QGraphicsSystemScreen * screen) :
        QGraphicsSystemCursor(screen), surface(0)
{
    QDirectFbConvenience::dfbInterface()->GetDisplayLayer(QDirectFbConvenience::dfbInterface(),DLID_PRIMARY, &m_layer);
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

    IDirectFBSurface *surface = QDirectFbConvenience::dfbSurfaceForPixmapData(map.pixmapData());

    int xSpot = image->hotspot().x();
    int ySpot = image->hotspot().y();

    if (m_layer->SetCooperativeLevel(m_layer, DLSCL_ADMINISTRATIVE) != DFB_OK) {
        return;
    }
    m_layer->SetCursorShape( m_layer, surface, xSpot, ySpot);
    m_layer->SetCooperativeLevel(m_layer, DLSCL_SHARED);
}
