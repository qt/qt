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

#include <QApplication>
#include <QPushButton>
//! [0]
#include <QTranslator>
//! [0]

//! [1] //! [2]
int main(int argc, char *argv[])
//! [1] //! [3] //! [4]
{
    QApplication app(argc, argv);
//! [3]

//! [5]
    QTranslator translator;
//! [5] //! [6]
    translator.load("hellotr_ja");
//! [6] //! [7]
    app.installTranslator(&translator);
//! [4] //! [7]

//! [8]
    QPushButton hello(QPushButton::tr("Hello world!"));
//! [8]
    hello.resize(100, 30);

    hello.show();
    return app.exec();
}
//! [2]
