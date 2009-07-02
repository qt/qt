#ifndef TESTLUPDATE_H
#define TESTLUPDATE_H

#include <QObject>
#include <QProcess>
#include <QStringList>

class TestLUpdate : public QObject
{
    Q_OBJECT

public:
    TestLUpdate();
    virtual ~TestLUpdate();

    void setWorkingDirectory( const QString &workDir);
    bool run( const QString &commandline);
    bool updateProFile( const QString &arguments);
    bool qmake();
    QStringList getErrorMessages() {
        return make_result;
    }
    void clearResult() {
        make_result.clear();
    }
private:
    QString     m_cmdLupdate;
    QString     m_cmdQMake;
    QString     m_workDir;
    QProcess	*childProc;
    QStringList env_list;
    QStringList make_result;

    bool	child_show;
    bool    qws_mode;
    bool	exit_ok;

    bool runChild( bool showOutput, const QString &program, const QStringList &argList = QStringList());
    void addMakeResult( const QString &result );
    void childHasData();

private slots:
    void childReady(int exitCode);
};

#endif // TESTLUPDATE_H
