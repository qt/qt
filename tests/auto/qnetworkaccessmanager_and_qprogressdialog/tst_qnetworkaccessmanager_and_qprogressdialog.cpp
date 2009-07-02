/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <QtGui>
#include <QtCore>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <qdebug.h>

#include "../network-settings.h"


class tst_QNetworkAccessManager_And_QProgressDialog : public QObject
{
Q_OBJECT
private slots:
    void downloadCheck();
};

class DownloadCheckWidget : public QWidget
{
    Q_OBJECT;
public:
    DownloadCheckWidget(QWidget *parent = 0) : QWidget(parent)
                                    , progressDlg(this), netmanager(this)
                                    , lateReadyRead(true)
    {
        progressDlg.setRange(1, 100);
        QMetaObject::invokeMethod(this, "go", Qt::QueuedConnection);
    }
    bool lateReadyRead;
public slots:
    void go()
    {
        QNetworkReply *reply = netmanager.get(
                QNetworkRequest(
                QUrl("http://" + QtNetworkSettings::serverName() + "/qtest/bigfile")
                ));
        connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
                this, SLOT(dataReadProgress(qint64, qint64)));
        connect(reply, SIGNAL(readyRead()),
                this, SLOT(dataReadyRead()));
        connect(reply, SIGNAL(finished()), this, SLOT(finishedFromReply()));

        progressDlg.exec();
    }
    void dataReadProgress(qint64 done, qint64 total)
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        progressDlg.setMaximum(total);
        progressDlg.setValue(done);
    }
    void dataReadyRead()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        lateReadyRead = true;
    }
    void finishedFromReply()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        lateReadyRead = false;
        reply->deleteLater();
        QTestEventLoop::instance().exitLoop();
    }


private:
    QProgressDialog progressDlg;
    QNetworkAccessManager netmanager;
};

void tst_QNetworkAccessManager_And_QProgressDialog::downloadCheck()
{
    DownloadCheckWidget widget;
    widget.show();
    // run and exit on finished()
    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    // run some more to catch the late readyRead() (or: to not catch it)
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(QTestEventLoop::instance().timeout());
    // the following fails when a readyRead() was received after finished()
    QVERIFY(!widget.lateReadyRead);
}



QTEST_MAIN(tst_QNetworkAccessManager_And_QProgressDialog)
#include "tst_qnetworkaccessmanager_and_qprogressdialog.moc"
