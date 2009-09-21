#include <qtest.h>
#include <QLibraryInfo>
#include <QDir>
#include <QProcess>

class tst_examples : public QObject
{
    Q_OBJECT
public:
    tst_examples();

private slots:
    void examples_data();
    void examples();

    void namingConvention();
private:
    QString qmlviewer;

    void namingConvention(const QDir &);
    QStringList findQmlFiles(const QDir &);
};

tst_examples::tst_examples()
{
    QString binaries = QLibraryInfo::location(QLibraryInfo::BinariesPath);

#if defined(Q_WS_MAC)
    qmlviewer = QDir(binaries).absoluteFilePath("qmlviewer.app/Contents/MacOS/qmlviewer");
#elif defined(Q_WS_WIN)
    qmlviewer = QDir(binaries).absoluteFilePath("qmlviewer.exe");
#else
    qmlviewer = QDir(binaries).absoluteFilePath("qmlviewer");
#endif
}

/*
This tests that the demos and examples follow the naming convention required
to have them tested by the examples() test.
*/
void tst_examples::namingConvention(const QDir &d)
{
    QStringList files = d.entryList(QStringList() << QLatin1String("*.qml"), 
                                    QDir::Files);

    bool seenQml = !files.isEmpty();
    bool seenLowercase = false;

    foreach (const QString &file, files) {
        if (file.at(0).isLower()) 
            seenLowercase = true;
    }

    if (!seenQml) {
        QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot | 
                QDir::NoSymLinks);
        foreach (const QString &dir, dirs) {
            QDir sub = d;
            sub.cd(dir);
            namingConvention(sub);
        }
    } else if(!seenLowercase) {
        QTest::qFail(QString("Directory " + d.absolutePath() + " violates naming convention").toLatin1().constData(), __FILE__, __LINE__);
    }
}

void tst_examples::namingConvention()
{
    QString examples = QLibraryInfo::location(QLibraryInfo::ExamplesPath);
    QString demos = QLibraryInfo::location(QLibraryInfo::DemosPath);

    namingConvention(QDir(examples));
    namingConvention(QDir(demos));
}

QStringList tst_examples::findQmlFiles(const QDir &d)
{
    QStringList rv;

    QStringList files = d.entryList(QStringList() << QLatin1String("*.qml"), 
                                    QDir::Files);
    foreach (const QString &file, files) {
        if (file.at(0).isLower()) {
            rv << d.absoluteFilePath(file);
        }
    }

    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot | 
                                   QDir::NoSymLinks);
    foreach (const QString &dir, dirs) {
        QDir sub = d;
        sub.cd(dir);
        rv << findQmlFiles(sub);
    }

    return rv;
}

/*
This test runs all the examples in the declarative UI source tree and ensures 
that they start and exit cleanly.

Examples are any .qml files under the examples/ or demos/ directory that start
with a lower case letter.  
*/
void tst_examples::examples_data()
{
    QTest::addColumn<QString>("file");

    QString examples = QLibraryInfo::location(QLibraryInfo::ExamplesPath);
    QString demos = QLibraryInfo::location(QLibraryInfo::DemosPath);

    QStringList files;
    files << findQmlFiles(QDir(examples));
    files << findQmlFiles(QDir(demos));

    foreach (const QString &file, files)
        QTest::newRow(file.toLatin1().constData()) << file;
}

void tst_examples::examples()
{
    QFETCH(QString, file);

    QStringList arguments;
    arguments << "-script" << "data/dummytest" 
              << "-scriptopts" << "play,exitoncomplete,exitonfailure" 
              << file;
    QProcess p;
    p.start(qmlviewer, arguments);
    QVERIFY(p.waitForFinished());
    QCOMPARE(p.exitStatus(), QProcess::NormalExit);
    QCOMPARE(p.exitCode(), 0);
}

QTEST_MAIN(tst_examples)

#include "tst_examples.moc"
