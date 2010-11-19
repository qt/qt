#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>

class TestModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int snapshotCount READ snapshotCount CONSTANT)
    Q_PROPERTY(QStringList goodImages READ goodImages CONSTANT)//List of image locatoins
    Q_PROPERTY(QStringList badImages READ badImages CONSTANT)
    Q_PROPERTY(QStringList diffImages READ diffImages CONSTANT)
    Q_PROPERTY(QString testName READ testName CONSTANT) //The qml file name
    Q_PROPERTY(QString testCase READ testCase CONSTANT) //The actual contents, not the location
    Q_PROPERTY(QString testScript READ testScript CONSTANT) //The actual contents, not the location
public:
    explicit TestModel(QObject *parent = 0);
    bool setTest(const QString &testPath);//testPath is the path to the test script, and assumes the file under test has the same name and is in the dir above

    int snapshotCount() { return _count; }
    QString testCase() { return _testCase; }
    QString testName() { return _testName; }
    QString testScript() {return _testScript; }
    QStringList goodImages() {return _good;}
    QStringList badImages() { return _bad; }
    QStringList diffImages() {return _diff;}

signals:
    void moveOn();

public slots:
    bool runTest();//returns true iff running the test changed the failure images.
    void updateVisuals();
    void updatePlatformVisuals();

private:
    void launchTester(const QString &args);

    int _count;
    QStringList _good,_bad,_diff;
    QString _testCase;
    QString _testScript;
    QString _testPath;
    QString _testScriptPath;
    QString _testName;
};

#endif // TESTMODEL_H
