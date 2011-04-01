#ifndef MODELPARTICLE_H
#define MODELPARTICLE_H
#include "particle.h"
#include <QPointer>
#include <QSet>
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QSGVisualDataModel;

class ModelParticle : public ParticleType
{
    Q_OBJECT

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int modelCount READ modelCount NOTIFY modelCountChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")
public:
    explicit ModelParticle(QSGItem *parent = 0);
    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    int modelCount() const;

    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);
    virtual int count();
signals:
    void modelChanged();
    void delegateChanged();
    void modelCountChanged();

public slots:
    void take(int idx);//TODO: Rename pause and unpause? freeze and unfreeze? take and restore?
    void recover(int idx);

protected:
    virtual void reset();
    void prepareNextFrame();
private:
    bool m_ownModel;
    QDeclarativeComponent* m_comp;
    QSGVisualDataModel *m_model;
    QVariant m_dataSource;
    QList<QPointer<QSGItem> > m_deletables;
    int m_particleCount;

    QVector<QSGItem*> m_items;
    QVector<ParticleData*> m_data;
    QVector<int> m_idx;
    QList<int> m_available;
    QSet<QSGItem*> m_stasis;
    qreal m_lastT;
};

QT_END_NAMESPACE

QT_END_HEADER
#endif // MODELPARTICLE_H
