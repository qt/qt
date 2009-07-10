#ifndef HAPPYBIRTHDAY_H
#define HAPPYBIRTHDAY_H

#include <QmlPropertyValueSource>

class HappyBirthday : public QmlPropertyValueSource
{
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
    HappyBirthday(QObject *parent = 0);

    virtual void setTarget(const QmlMetaProperty &);

    QString name() const;
    void setName(const QString &);

private slots:
    void advance();

signals:
    void nameChanged();
private:
    int m_line;
    QStringList m_lyrics;
    QmlMetaProperty m_target;
    QString m_name;
};
QML_DECLARE_TYPE(HappyBirthday);

#endif // HAPPYBIRTHDAY_H

