#ifndef QETCPROVIDER_H
#define QETCPROVIDER_H

#include <qgl.h>
#include <QDeclarativeImageProvider>
#include <QSGTexture>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFileInfo>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

// #define ETC_DEBUG

class EtcTexture : public QSGTexture
{
    Q_OBJECT
public:
    EtcTexture();
    ~EtcTexture();

    void bind();
    QSize textureSize() const;

    int textureId() const { return m_texture_id; }

    void setImage(const QImage &image) { Q_UNUSED(image); }

    bool hasAlphaChannel() const { return false; }
    bool hasMipmaps() const { return false; }

    QByteArray m_data;
    QSize m_size;
    QSize m_paddedSize;
    GLuint m_texture_id;
};

class QEtcProvider : public QDeclarativeImageProvider
{
public:
    QEtcProvider()
        : QDeclarativeImageProvider(QDeclarativeImageProvider::Texture)
    {
#ifdef ETC_DEBUG
        qDebug () << "Creating QEtcProvider.";
#endif
    }
    QSGTexture *requestTexture(const QString &id, QSize *size, const QSize &requestedSize);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QETCPROVIDER_H
