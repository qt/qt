#include "qwaylandxcompositebuffer.h"

#include "wayland-client.h"

QWaylandXCompositeBuffer::QWaylandXCompositeBuffer(wl_xcomposite *xcomposite, uint32_t window, const QSize &size, wl_visual *visual)
    :mSize(size)
{
    mBuffer = wl_xcomposite_create_buffer(xcomposite,
                                              window,
                                              size.width(),
                                              size.height(),
                                              visual);
}

QSize QWaylandXCompositeBuffer::size() const
{
    return mSize;
}
