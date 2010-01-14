#include "qblitter_directfb.h"
#include "qgraphicssystem_minimaldfb.h"
#include "qdirectfbconvenience.h"

#include <QtGui/private/qpixmap_blitter_p.h>

#include <QDebug>

#include <directfb.h>

QDirectFbBlitter::QDirectFbBlitter(const QRect &rect, IDirectFBSurface *surface)
        : QBlittable(rect, QBlittable::Capabilities(QBlittable::SolidRectCapability
                                                          |QBlittable::SourcePixmapCapability
                                                          |QBlittable::SourceOverPixmapCapability
                                                          |QBlittable::SourceOverScaledPixmapCapability))
{
    if (surface) {
        m_surface = surface;
    } else {
        DFBSurfaceDescription surfaceDesc;
        memset(&surfaceDesc,0,sizeof(DFBSurfaceDescription));
        surfaceDesc.width = rect.width();
        surfaceDesc.height = rect.height();
        surfaceDesc.caps = DSCAPS_PREMULTIPLIED;
        surfaceDesc.pixelformat = DSPF_ARGB;
        surfaceDesc.flags = DFBSurfaceDescriptionFlags(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS | DSDESC_PIXELFORMAT);

        IDirectFB *dfb = QDirectFbConvenience::dfbInterface();
        dfb->CreateSurface(dfb,&surfaceDesc, &m_surface);
        m_surface->Clear(m_surface,0,0,0,0);
    }
}

QDirectFbBlitter::~QDirectFbBlitter()
{
    m_surface->Release(m_surface);
}

void QDirectFbBlitter::fillRect(const QRectF &rect, const QColor &color)
{
    m_surface->SetColor(m_surface, color.red(), color.green(), color.blue(), color.alpha());
//    When the blitter api supports non opaque blits, also remember to change
//    qpixmap_blitter.cpp::fill
//    DFBSurfaceDrawingFlags drawingFlags = color.alpha() ? DSDRAW_BLEND : DSDRAW_NOFX;
//    m_surface->SetDrawingFlags(m_surface, drawingFlags);
    m_surface->SetDrawingFlags(m_surface, DSDRAW_NOFX);
    m_surface->FillRectangle(m_surface, rect.x(), rect.y(),
                              rect.width(), rect.height());
}

void QDirectFbBlitter::drawPixmap(const QRectF &rect, const QPixmap &pixmap, const QRectF &srcRect)
{
    QPixmapData *data = pixmap.pixmapData();
    Q_ASSERT(data->width() && data->height());
    Q_ASSERT(data->classId() == QPixmapData::BlitterClass);
    QBlittablePixmapData *blitPm = static_cast<QBlittablePixmapData*>(data);
    QDirectFbBlitter *dfbBlitter = static_cast<QDirectFbBlitter *>(blitPm->blittable());

    IDirectFBSurface *s = dfbBlitter->m_surface;

    quint32 blittingFlags = pixmap.hasAlpha()? DSBLIT_BLEND_ALPHACHANNEL : DSBLIT_NOFX;

    s->SetBlittingFlags(s, DFBSurfaceBlittingFlags(blittingFlags));
    m_surface->SetBlittingFlags(m_surface, DFBSurfaceBlittingFlags(blittingFlags));
    m_surface->SetPorterDuff(m_surface,DSPD_SRC_OVER);
    m_surface->SetDstBlendFunction(m_surface,DSBF_INVSRCALPHA);

    const DFBRectangle sRect = { srcRect.x(), srcRect.y(), rect.width(), rect.height() };

    DFBResult result;
    if (rect.width() == srcRect.width() && rect.height() == srcRect.height())
        result = m_surface->Blit(m_surface, s, &sRect, rect.x(), rect.y());
    else {
        const DFBRectangle dRect = { rect.x(), rect.y(), rect.width(), rect.height() };
        result = m_surface->StretchBlit(m_surface, s, &sRect, &dRect);
    }
    if (result != DFB_OK)
        DirectFBError("QDirectFBBlitter::drawPixmap()", result);
}

QImage *QDirectFbBlitter::doLock()
{
    Q_ASSERT(m_surface);
    Q_ASSERT(rect().isValid());

    void *mem;
    int bpl;
    const DFBResult result = m_surface->Lock(m_surface, DFBSurfaceLockFlags(DSLF_WRITE|DSLF_READ), static_cast<void**>(&mem), &bpl);
    if (result == DFB_OK) {
        DFBSurfacePixelFormat dfbFormat;
        DFBSurfaceCapabilities dfbCaps;
        m_surface->GetPixelFormat(m_surface,&dfbFormat);
        m_surface->GetCapabilities(m_surface,&dfbCaps);
        QImage::Format format = QDirectFbConvenience::imageFormatFromSurfaceFormat(dfbFormat, dfbCaps);
        int w, h;
        m_surface->GetSize(m_surface,&w,&h);
        m_image = QImage(static_cast<uchar *>(mem),w,h,bpl,format);
    } else {
        DirectFBError("Failed to lock image", result);
    }

    return &m_image;
}

void QDirectFbBlitter::doUnlock()
{
    m_surface->Unlock(m_surface);
}
