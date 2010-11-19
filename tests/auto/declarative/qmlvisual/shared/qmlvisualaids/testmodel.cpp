#include "testmodel.h"
#include <QProcess>
#include <QDebug>
#include <QFile>

TestModel::TestModel(QObject *parent) :
    QObject(parent), _testName("Invalid")
{

}

//testPath is the path to the test script, and assumes the file under test has the same name and is in the dir above
bool TestModel::setTest(const QString &testPath)
{
    _good.clear();
    _bad.clear();
    _diff.clear();

    _testScriptPath = testPath;
    if(!_testScriptPath.endsWith(".qml"))
        _testScriptPath += ".qml";
    _testName = _testScriptPath.split('/').last();

    //Assumed that the test case is in the directory above and has the same name as the script
    _testPath = _testScriptPath.left(_testScriptPath.lastIndexOf('/', _testScriptPath.lastIndexOf('/') - 1))
            + '/' + _testName;

    bool ret = QFile::exists(_testPath) && QFile::exists(_testScriptPath);
    if(!ret)
        return ret;

    QFile test(_testPath);
    test.open(QFile::ReadOnly | QFile::Text);
    _testCase = test.readAll();

    QFile script(_testScriptPath);
    script.open(QFile::ReadOnly | QFile::Text);
    _testScript = script.readAll();

    QString base = _testScriptPath;
    base.chop(4);//remove .qml, replace with .%1.png
    base += ".%1.png";
    int c = 0;
    while (QFile::exists(base.arg(c))) {
        _good << "file://" + base.arg(c);
        if(QFile::exists(base.arg(c) + ".reject.png"))
            _bad << "file://" + base.arg(c) + ".reject.png";
        else
            _bad << "";

        if(QFile::exists(base.arg(c) + ".diff.png"))
            _diff << "file://" + base.arg(c) + ".diff.png";
        else
            _diff << "";

        c++;
    }

    return ret;
}

//returns true iff running the test changed the failure images.
bool TestModel::runTest()
{
    launchTester("-play");
    return false;//TODO: Actually check that
}

void TestModel::updateVisuals()
{
    launchTester("-updatevisuals");
}

void TestModel::updatePlatformVisuals()
{
    launchTester("-updateplatformvisuals");
}

void TestModel::launchTester(const QString &args)
{
    QStringList arguments;
    arguments << args << _testPath;

    QString visualTest;
#if defined(Q_WS_WIN) || defined(Q_WS_S60)
    visualTest = "tst_qmlvisual.exe";
#else
    visualTest = "./tst_qmlvisual";
#endif

    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start(visualTest, arguments);
    p.waitForFinished();
}

