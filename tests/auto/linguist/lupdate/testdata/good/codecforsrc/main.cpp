/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QtGui>
#include <QtCore>
#include <QTextCodec>

int main(int argc, char **argv)
{
        QApplication a(argc, argv);
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

        QWidget w;
        QLabel label1(QObject::tr("abc", "ascii"), &w);
        QLabel label2(QObject::tr("æøå", "utf-8"), &w);
        QLabel label2a(QObject::tr("\303\246\303\270\303\245", "utf-8 oct"), &w);
        QLabel label3(QObject::trUtf8("Für Élise", "trUtf8"), &w);
        QLabel label3a(QObject::trUtf8("F\303\274r \303\211lise", "trUtf8 oct"), &w);

        QBoxLayout *ly = new QVBoxLayout(&w);
        ly->addWidget(&label1);
        ly->addWidget(&label2);
        ly->addWidget(&label2a);
        ly->addWidget(&label3);
        ly->addWidget(&label3a);

        w.show();
        return a.exec();
}
