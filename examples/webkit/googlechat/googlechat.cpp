/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtWebKit>
#include <QSslSocket>

#include "googlechat.h"

#define GOOGLECHAT_URL "http://talkgadget.google.com/talkgadget/m"

GoogleChat::GoogleChat(): QWidget() {
    form.setupUi(this);
    setFixedSize(320, 480);

    form.userNameEdit->setFocus();
    connect(form.userNameEdit, SIGNAL(textChanged(QString)), SLOT(adjustLoginButton()));
    connect(form.userNameEdit, SIGNAL(returnPressed()), SLOT(inputPassword()));

    connect(form.passwordEdit, SIGNAL(textChanged(QString)), SLOT(adjustLoginButton()));
    connect(form.passwordEdit, SIGNAL(returnPressed()), SLOT(doLogin()));

    form.loginButton->setEnabled(false);
    connect(form.loginButton, SIGNAL(clicked()), SLOT(doLogin()));

    connect(form.webView, SIGNAL(loadFinished(bool)), SLOT(initialPage(bool)));
    connect(form.webView, SIGNAL(loadProgress(int)),
            form.progressBar, SLOT(setValue(int)));
    form.webView->setUrl((QUrl(GOOGLECHAT_URL)));
    form.webView->setContextMenuPolicy(Qt::PreventContextMenu);

    showStatus("Wait...");
}

void GoogleChat::showStatus(const QString &msg) {
    form.statusLabel->setText(msg);
    form.stackedWidget->setCurrentIndex(0);
}

void GoogleChat::showError(const QString &msg) {
    form.progressBar->hide();
    showStatus(QString("Error: %1").arg(msg));
}

QWebElement GoogleChat::document() const {
    return form.webView->page()->mainFrame()->documentElement();
}

void GoogleChat::adjustLoginButton() {
    userName = form.userNameEdit->text();
    password = form.passwordEdit->text();
    bool ok = !userName.isEmpty() && !password.isEmpty();
    form.loginButton->setEnabled(ok);
}

void GoogleChat::inputPassword() {
    if (!form.userNameEdit->text().isEmpty())
        form.passwordEdit->setFocus();
}

void GoogleChat::doLogin() {
    userName = form.userNameEdit->text();
    password = form.passwordEdit->text();
    bool ok = !userName.isEmpty() && !password.isEmpty();
    if (!ok)
        return;

    form.progressBar->setValue(0);
    form.progressBar->show();
    connect(form.webView, SIGNAL(loadFinished(bool)), SLOT(loginPage(bool)));
    connect(form.webView, SIGNAL(loadProgress(int)),
            form.progressBar, SLOT(setValue(int)));
    showStatus("Logging in...");

    QString userEmail = userName + "@gmail.com";

    document().findFirst("#Email").setAttribute("value", userEmail);
    document().findFirst("#Passwd").setAttribute("value", password);
    document().findFirst("#gaia_loginform").evaluateJavaScript("this.submit();");

}

void GoogleChat::initialPage(bool ok) {
    if (!QSslSocket::supportsSsl()) {
        showError("This example requires SSL support.");
        return;
    }

    if (ok) {
        QWebElement email = document().findFirst("#Email");
        QWebElement passwd = document().findFirst("#Passwd");
        QWebElement loginForm = document().findFirst("#gaia_loginform");
        if (!email.isNull() && !passwd.isNull() && !loginForm.isNull()) {
            form.stackedWidget->setCurrentIndex(1);
            form.userNameEdit->setFocus();
            form.webView->disconnect();
            return;
        }
    }

    showError("SERVICE unavailable.");
}

void GoogleChat::hideElements()
{
    document().findFirst(".footer-footer").removeFromDocument();
    document().findFirst(".title-bar-bg .title-bar").removeFromDocument();
    QTimer::singleShot(2000, this, SLOT(hideElements()));
}

void GoogleChat::loginPage(bool ok) {
    QString location = form.webView->url().toString();
    if (!ok) {
        if (location.indexOf("CheckCookie"))
            return;
        showError("Service unavailable");
    } else {
        // check for any error message

        QWebElement  e = document().findFirst(".errormsg");
        if (e.isNull()) {
            form.stackedWidget->setCurrentIndex(2);
            QTimer::singleShot(500, this, SLOT(hideElements()));
            return;
        }

       QString err = "Unknown login failure.";
       const QString errorMessage = e.toPlainText();
        if (!errorMessage.isEmpty()) {
            err = errorMessage;
            err = err.simplified();
        }
        showError(err);
    }
}
