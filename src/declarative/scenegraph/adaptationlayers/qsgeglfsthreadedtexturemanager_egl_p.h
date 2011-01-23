#ifndef QSGEGLFSTHREADEDTEXTUREMANAGER_H
#define QSGEGLFSTHREADEDTEXTUREMANAGER_H

#include "threadedtexturemanager.h"

class QSGEglFSThreadedTextureManagerPrivate;

class QSGEglFSThreadedTextureManager : public QSGThreadedTextureManager
{
public:    
    QSGEglFSThreadedTextureManager();

    void initializeThreadContext();
    void makeThreadContextCurrent();
    void uploadInThread(TextureReference *texture, const QImage &image, UploadHints hints);

    void setUploadsScanlines(bool);
    bool uploadsScanlines() const;

private:
    QSGEglFSThreadedTextureManagerPrivate *d;
};
#endif // QSGEGLFSTHREADEDTEXTUREMANAGER_H
