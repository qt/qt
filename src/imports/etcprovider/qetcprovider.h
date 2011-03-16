#ifndef QETCPROVIDER_H
#define QETCPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QSGTextureProvider>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFileInfo>

// #define ETC_DEBUG

class EtcTextureProvider : public QSGTextureProvider
{
    Q_OBJECT
public:
    void updateTexture ();
    QSGTextureRef texture ();
    QSize textureSize() const;

    QByteArray m_data;
    QSGTextureRef m_texture;
    QSize m_size;
    QSize m_paddedSize;
};

class BaseUrlHelper : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QString baseDir (QObject *obj)
    {
        QDeclarativeContext *ctx = QDeclarativeEngine::contextForObject(obj);
        if (ctx) {
            QFileInfo fi(ctx->baseUrl().toLocalFile());
#ifdef ETC_DEBUG
            qDebug () << "BaseDir: " << fi.absolutePath();
#endif
            return fi.absolutePath();
        }
        return QString();
    }
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
    QSGTextureProvider *requestTexture(const QString &id, QSize *size, const QSize &requestedSize);
};


#endif // QETCPROVIDER_H
