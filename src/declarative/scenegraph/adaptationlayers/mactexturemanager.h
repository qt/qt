#ifndef MACTEXTUREMANAGER_H
#define MACTEXTUREMANAGER_H

#include "adaptationlayer.h"

class Q_DECLARATIVE_EXPORT QSGMacTextureManager : public TextureManager
{
public:
    QSGMacTextureManager();

    const QSGTextureRef &requestUploadedTexture(const QImage &image, UploadHints hints, QObject *listener, const char *slot);
};

#endif // MACTEXTUREMANAGER_H
