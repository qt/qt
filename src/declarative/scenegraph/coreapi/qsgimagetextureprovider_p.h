#ifndef QSGIMAGETEXTUREPROVIDER_P_H
#define QSGIMAGETEXTUREPROVIDER_P_H

#include <QSGTextureProvider>


class QSGImageTextureProvider : public QSGTextureProvider
{
    Q_OBJECT

public:
    QSGImageTextureProvider(QObject *parent = 0);
    virtual void setImage(const QImage &image);

    void updateTexture();
    QSGTextureRef texture();

    QSize textureSize() const;

protected:
    QImage m_image;
    QSGTextureRef m_texture;
};



#endif // QSGIMAGETEXTUREPROVIDER_P_H
