#ifndef QDIRECTFBCONVENIENCE_H
#define QDIRECTFBCONVENIENCE_H

#include <QtGui/qimage.h>

#include <directfb/directfb.h>

class QDirectFbConvenience
{
public:
    static QImage::Format imageFormatFromSurface(IDirectFBSurface *surface);

    //This is set by the graphicssystem constructor
    static IDirectFB *dfbInterface() { return dfb; }

private:
    static void setDfbInterface(IDirectFB *dfbInterface) {dfb = dfbInterface;}
    static IDirectFB *dfb;
    friend class QDirectFbGraphicsSystem;
};

#endif // QDIRECTFBCONVENIENCE_H
