/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
