#ifndef TST_LINGUIST
#define TST_LINGUIST

#include <QtTest/QtTest>
#include <QtCore/QtCore>

//TESTED_CLASS=
//TESTED_FILES=


class tst_linguist : public QObject
{
    Q_OBJECT
private slots:
    void fetchtr();
    void fetchtr_data();

    void simtexth();
    void simtexth_data();
};

#endif
