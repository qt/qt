#ifndef QMLDEBUG_H
#define QMLDEBUG_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

class QmlDebugConnection;
class QmlDebugWatch;
class QmlDebugEnginesQuery;
class QmlDebugRootContextQuery;
class QmlDebugObjectQuery;
class QmlDebugPropertyReference;
class QmlDebugContextReference;
class QmlDebugObjectReference;
class QmlDebugFileReference;
class QmlDebugEngineReference;
class QmlEngineDebugPrivate;
class Q_DECLARATIVE_EXPORT QmlEngineDebug : public QObject
{
Q_OBJECT
public:
    QmlEngineDebug(QmlDebugConnection *, QObject * = 0);

    QmlDebugWatch *addWatch(const QmlDebugPropertyReference &, 
                            QObject *parent = 0);
    QmlDebugWatch *addWatch(const QmlDebugContextReference &, const QString &,
                            QObject *parent = 0);
    QmlDebugWatch *addWatch(const QmlDebugObjectReference &, const QString &,
                            QObject *parent = 0);
    QmlDebugWatch *addWatch(const QmlDebugObjectReference &,
                            QObject *parent = 0);
    QmlDebugWatch *addWatch(const QmlDebugFileReference &,
                            QObject *parent = 0);

    QmlDebugEnginesQuery *queryAvailableEngines(QObject *parent = 0);
    QmlDebugRootContextQuery *queryRootContexts(const QmlDebugEngineReference &,
                                                QObject *parent = 0);
    QmlDebugObjectQuery *queryObject(const QmlDebugObjectReference &, 
                                     QObject *parent = 0);
    QmlDebugObjectQuery *queryObjectRecursive(const QmlDebugObjectReference &, 
                                              QObject *parent = 0);

private:
    Q_DECLARE_PRIVATE(QmlEngineDebug);
};

class Q_DECLARATIVE_EXPORT QmlDebugWatch : public QObject
{
Q_OBJECT
public:
    enum State { Waiting, Active, Inactive, Dead };

    State state() const;

signals:
    void stateChanged(State);
    void objectChanged(int, const QmlDebugObjectReference &);
    void valueChanged(int, const QVariant &);
};

class Q_DECLARATIVE_EXPORT QmlDebugQuery : public QObject
{
Q_OBJECT
public:
    enum State { Waiting, Error, Completed };

    State state() const;
    bool isWaiting() const;

//    bool waitUntilCompleted();

signals:
    void stateChanged(State);

protected:
    QmlDebugQuery(QObject *);

private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    void setState(State);
    State m_state;
};

class Q_DECLARATIVE_EXPORT QmlDebugFileReference 
{
public:
    QmlDebugFileReference();
    QmlDebugFileReference(const QmlDebugFileReference &);
    QmlDebugFileReference &operator=(const QmlDebugFileReference &);

    QUrl url() const;
    void setUrl(const QUrl &);
    int lineNumber() const;
    void setLineNumber(int);
    int columnNumber() const;
    void setColumnNumber(int);

private:
    friend class QmlEngineDebugPrivate;
    QUrl m_url;
    int m_lineNumber;
    int m_columnNumber;
};

class Q_DECLARATIVE_EXPORT QmlDebugEngineReference
{
public:
    QmlDebugEngineReference();
    QmlDebugEngineReference(int);
    QmlDebugEngineReference(const QmlDebugEngineReference &);
    QmlDebugEngineReference &operator=(const QmlDebugEngineReference &);

    int debugId() const;
    QString name() const;

private:
    friend class QmlEngineDebugPrivate;
    int m_debugId;
    QString m_name;
};

class Q_DECLARATIVE_EXPORT QmlDebugObjectReference
{
public:
    QmlDebugObjectReference();
    QmlDebugObjectReference(int);
    QmlDebugObjectReference(const QmlDebugObjectReference &);
    QmlDebugObjectReference &operator=(const QmlDebugObjectReference &);

    int debugId() const;
    QString className() const;
    QString name() const;

    QmlDebugFileReference source() const;

    QList<QmlDebugPropertyReference> properties() const;
    QList<QmlDebugObjectReference> children() const;

private:
    friend class QmlEngineDebugPrivate;
    int m_debugId;
    QString m_class;
    QString m_name;
    QmlDebugFileReference m_source;
    QList<QmlDebugPropertyReference> m_properties;
    QList<QmlDebugObjectReference> m_children;
};

class Q_DECLARATIVE_EXPORT QmlDebugContextReference
{
public:
    QmlDebugContextReference();
    QmlDebugContextReference(const QmlDebugContextReference &);
    QmlDebugContextReference &operator=(const QmlDebugContextReference &);

    int debugId() const;
    QString name() const;

    QList<QmlDebugObjectReference> objects() const;
    QList<QmlDebugContextReference> contexts() const;

private:
    friend class QmlEngineDebugPrivate;
    int m_debugId;
    QString m_name;
    QList<QmlDebugObjectReference> m_objects;
    QList<QmlDebugContextReference> m_contexts;
};

class Q_DECLARATIVE_EXPORT QmlDebugPropertyReference
{
public:
    QmlDebugPropertyReference();
    QmlDebugPropertyReference(const QmlDebugPropertyReference &);
    QmlDebugPropertyReference &operator=(const QmlDebugPropertyReference &);

    QString name() const;
    QVariant value() const;
    QString binding() const;

private:
    friend class QmlEngineDebugPrivate;
    QString m_name;
    QVariant m_value;
    QString m_binding;
};


class Q_DECLARATIVE_EXPORT QmlDebugEnginesQuery : public QmlDebugQuery
{
Q_OBJECT
public:
    virtual ~QmlDebugEnginesQuery();
    QList<QmlDebugEngineReference> engines() const;
private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    QmlDebugEnginesQuery(QObject *);
    QmlEngineDebug *m_client;
    int m_queryId;
    QList<QmlDebugEngineReference> m_engines;
};

class Q_DECLARATIVE_EXPORT QmlDebugRootContextQuery : public QmlDebugQuery
{
Q_OBJECT
public:
    virtual ~QmlDebugRootContextQuery();
    QmlDebugContextReference rootContext() const;
private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    QmlDebugRootContextQuery(QObject *);
    QmlEngineDebug *m_client;
    int m_queryId;
    QmlDebugContextReference m_context;
};

class Q_DECLARATIVE_EXPORT QmlDebugObjectQuery : public QmlDebugQuery
{
Q_OBJECT
public:
    virtual ~QmlDebugObjectQuery();
    QmlDebugObjectReference object() const;
private:
    friend class QmlEngineDebug;
    friend class QmlEngineDebugPrivate;
    QmlDebugObjectQuery(QObject *);
    QmlEngineDebug *m_client;
    int m_queryId;
    QmlDebugObjectReference m_object;

};

#endif // QMLDEBUG_H
