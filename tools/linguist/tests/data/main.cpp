/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtCore>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStringList args = app.arguments();

    if (argc <= 1) {
        qDebug() << "Usage: " << qPrintable(args[0]) << " <ts-file>";
        return 1;
    }

    QTranslator trans;
    trans.load(args[1], ".");
    app.installTranslator(&trans);

    QWidget w;
    QVBoxLayout *layout = new QVBoxLayout(&w);

    QLabel label1(QObject::tr("XXXXXXXXX \33 XXXXXXXXXXX  • and → "), 0);
    QLabel label2(QObject::tr("\32"), 0);
    QLabel label3(QObject::tr("\176"), 0);
    QLabel label4(QObject::tr("\301"), 0);

    layout->addWidget(&label1);
    layout->addWidget(&label2);
    layout->addWidget(&label3);
    layout->addWidget(&label4);

    w.show();

    return app.exec();
}
