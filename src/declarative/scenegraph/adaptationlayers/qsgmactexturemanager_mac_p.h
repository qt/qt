#ifndef QSGMACTEXTUREMANAGER_MAC_P_H
#define QSGMACTEXTUREMANAGER_MAC_P_H

#include <QSGTextureManager>

class QSGMacTextureManager : public QSGTextureManager
{
    Q_OBJECT
public:
    explicit QSGMacTextureManager();

    QSGTextureRef upload(const QImage &image);
};

#endif // QSGMACTEXTUREMANAGER_MAC_P_H
