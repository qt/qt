#ifndef THREADEDTEXTUREMANAGER_H
#define THREADEDTEXTUREMANAGER_H

#include "adaptationlayer.h"

class QSGThreadedTextureManagerPrivate;

class QSGThreadedTextureManager : public TextureManager
{
public:
    QSGThreadedTextureManager();

    const QSGTextureRef &requestUploadedTexture(const QImage &image, UploadHints hints, QObject *listener, const char *slot);

    virtual void initializeThreadContext();
    virtual void makeThreadContextCurrent();
    virtual void uploadInThread(TextureReference *texture, const QImage &image, UploadHints hints);

private:
    QSGThreadedTextureManagerPrivate *d;
};

#endif // THREADEDTEXTUREMANAGER_H
