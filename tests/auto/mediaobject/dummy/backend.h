#ifndef PHONON_DUMMY_BACKEND_H
#define PHONON_DUMMY_BACKEND_H

#include <phonon/objectdescription.h>
#include <phonon/backendinterface.h>

#include <phonon/medianode.h>

#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Dummy
{
class AudioOutput;
class MediaObject;

class Backend : public QObject, public BackendInterface
{
    Q_OBJECT
    Q_INTERFACES(Phonon::BackendInterface)

public:
    Backend(QObject *parent = 0, const QVariantList & = QVariantList());
    virtual ~Backend();

    QObject *createObject(BackendInterface::Class, QObject *parent, const QList<QVariant> &args);

    QStringList availableMimeTypes() const;

    QList<int> objectDescriptionIndexes(ObjectDescriptionType type) const;
    QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType type, int index) const;

    bool startConnectionChange(QSet<QObject *>);
    bool connectNodes(QObject *, QObject *);
    bool disconnectNodes(QObject *, QObject *);
    bool endConnectionChange(QSet<QObject *>);

Q_SIGNALS:
    void objectDescriptionChanged(ObjectDescriptionType);

private:
    QList<QPointer<AudioOutput> > m_audioOutputs;
};
}
} // namespace Phonon::Dummy

QT_END_NAMESPACE

#endif // PHONON_DUMMY_BACKEND_H
