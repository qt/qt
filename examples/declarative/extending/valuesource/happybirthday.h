#ifndef HAPPYBIRTHDAY_H
#define HAPPYBIRTHDAY_H

#include <QmlPropertyValueSource>

// ![0]
class HappyBirthday : public QmlPropertyValueSource
{
Q_OBJECT
// ![0]
Q_PROPERTY(QString name READ name WRITE setName)
// ![1]
public:
    HappyBirthday(QObject *parent = 0);

    virtual void setTarget(const QmlMetaProperty &);
// ![1]

    QString name() const;
    void setName(const QString &);

private slots:
    void advance();

private:
    int m_line;
    QStringList m_lyrics;
    QmlMetaProperty m_target;
    QString m_name;
// ![2]
};
// ![2]
QML_DECLARE_TYPE(HappyBirthday);

#endif // HAPPYBIRTHDAY_H

