#include "qdirectfbconvenience.h"

IDirectFB *QDirectFbConvenience::dfb = 0;

QImage::Format QDirectFbConvenience::imageFormatFromSurface(IDirectFBSurface *surface)
{
    DFBSurfacePixelFormat format;
    surface->GetPixelFormat(surface, &format);

    switch (format) {
    case DSPF_LUT8:
        return QImage::Format_Indexed8;
    case DSPF_RGB24:
        return QImage::Format_RGB888;
    case DSPF_ARGB4444:
        return QImage::Format_ARGB4444_Premultiplied;
    case DSPF_RGB444:
        return QImage::Format_RGB444;
    case DSPF_RGB555:
    case DSPF_ARGB1555:
        return QImage::Format_RGB555;
    case DSPF_RGB16:
        return QImage::Format_RGB16;
    case DSPF_ARGB6666:
        return QImage::Format_ARGB6666_Premultiplied;
    case DSPF_RGB18:
        return QImage::Format_RGB666;
    case DSPF_RGB32:
        return QImage::Format_RGB32;
    case DSPF_ARGB: {
            DFBSurfaceCapabilities caps;
            const DFBResult result = surface->GetCapabilities(surface, &caps);
            Q_ASSERT(result == DFB_OK);
            Q_UNUSED(result);
            return (caps & DSCAPS_PREMULTIPLIED
                    ? QImage::Format_ARGB32_Premultiplied
                        : QImage::Format_ARGB32); }
    default:
        break;
    }
    return QImage::Format_Invalid;

}
