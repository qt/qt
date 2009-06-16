#include <QtTest/QtTest>

#include<dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class Test : public QObject{
  Q_OBJECT
public slots:
    void initTestCase() {
        QDir testdir = QDir::tempPath();

        const QString subfolder_name = QLatin1String("test_speed");
        QVERIFY(testdir.mkdir(subfolder_name));
        QVERIFY(testdir.cd(subfolder_name));

        for (uint i=0; i<10000; ++i) {
            QFile file(testdir.absolutePath() + "/testfile_" + QString::number(i));
            file.open(QIODevice::WriteOnly);
        }
    }
    void cleanupTestCase() {
        {
            QDir testdir(QDir::tempPath() + QLatin1String("/test_speed"));

            foreach (const QString &filename, testdir.entryList()) {
                testdir.remove(filename);
            }
        }
        const QDir temp = QDir(QDir::tempPath());
        temp.rmdir(QLatin1String("test_speed"));
    }
private slots:
    void sizeSpeed() {
        QDir testdir(QDir::tempPath() + QLatin1String("/test_speed"));
        QBENCHMARK {
            QFileInfoList fileInfoList = testdir.entryInfoList(QDir::Files, QDir::Unsorted);
            foreach (const QFileInfo &fileInfo, fileInfoList) {
                fileInfo.isDir();
                fileInfo.size();
            }
        }
    }
    void sizeSpeedWithoutFilter() {
        QDir testdir(QDir::tempPath() + QLatin1String("/test_speed"));
        QBENCHMARK {
            QFileInfoList fileInfoList = testdir.entryInfoList(QDir::NoFilter, QDir::Unsorted);
            foreach (const QFileInfo &fileInfo, fileInfoList) {
                fileInfo.size();
            }
        }
    }
    void sizeSpeedWithoutFileInfoList() {
        QDir testdir(QDir::tempPath() + QLatin1String("/test_speed"));
        testdir.setSorting(QDir::Unsorted);
        QBENCHMARK {
            QStringList fileList = testdir.entryList(QDir::NoFilter, QDir::Unsorted);
            foreach (const QString &filename, fileList) {
                QFileInfo fileInfo(filename);
                fileInfo.size();
            }
        }
    }
    void iDontWantAnyStat() {
        QDir testdir(QDir::tempPath() + QLatin1String("/test_speed"));
        testdir.setSorting(QDir::Unsorted);
        testdir.setFilter(QDir::AllEntries | QDir::System | QDir::Hidden);
        QBENCHMARK {
            QStringList fileList = testdir.entryList(QDir::NoFilter, QDir::Unsorted);
            foreach (const QString &filename, fileList) {

            }
        }
    }
#ifndef Q_OS_WIN
    void testLowLevel() {
        QDir testdir(QDir::tempPath() + QLatin1String("/test_speed"));
        DIR *dir = opendir(qPrintable(testdir.absolutePath()));
        QVERIFY(!chdir(qPrintable(testdir.absolutePath())));
        QBENCHMARK {
            struct dirent *item = readdir(dir);
            while (item) {
                char *fileName = item->d_name;

                struct stat fileStat;
                QVERIFY(!stat(fileName, &fileStat));

                item = readdir(dir);
            }
        }
        closedir(dir);
    }
#endif
};

QTEST_MAIN(Test)
#include "tst_qdir.moc"
