#ifndef QSGENGINE_H
#define QSGENGINE_H

#include <QObject>

#include <qsgtexture.h>

class QSGEnginePrivate;

class QSGContext;
class Q_DECLARATIVE_EXPORT QSGEngine : public QObject
{
    Q_OBJECT

    Q_DECLARE_PRIVATE(QSGEngine)

public:

    enum TextureOption {
        TextureHasAlphaChannel  = 0x0001,
        TextureHasMipmaps       = 0x0002,
        TextureOwnsGLTexture    = 0x0004
    };
    Q_DECLARE_FLAGS(TextureOptions, TextureOption)

    QSGTexture *createTextureFromImage(const QImage &image) const;
    QSGTexture *createTextureFromId(uint id, const QSize &size, TextureOptions options = TextureOption(0)) const;

private:
    QSGEngine(QObject *parent = 0);
    ~QSGEngine();

    friend class QSGContext;
    friend class QSGContextPrivate;
    void setContext(QSGContext *context);

};

#endif // QSGENGINE_H
