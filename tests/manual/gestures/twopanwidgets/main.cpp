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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

static const char text[] =
        "Hello world! This is just a lot of text with to make sure scrollbar appear";

class TextEdit : public QTextEdit
{
    Q_OBJECT
public Q_SLOTS:
    void acceptTouch()
    {
        viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
        if (QWidget *w = qobject_cast<QPushButton*>(sender()))
            w->setEnabled(false);
    }
};

class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public Q_SLOTS:
    void acceptTouch()
    {
        viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
        if (QWidget *w = qobject_cast<QPushButton*>(sender()))
            w->setEnabled(false);
    }
};

class MainWindow : public QMainWindow
{
public:
    MainWindow();
};

MainWindow::MainWindow()
{
    QTabWidget *tw = new QTabWidget;
    setCentralWidget(tw);
    {
        QWidget *tab = new QWidget;
        QGridLayout *layout = new QGridLayout(tab);
        QTextEdit *edit1 = new TextEdit;
        QTextEdit *edit2 = new TextEdit;
        QString text1 = QString(text).replace(' ', '\n');
        for (int i = 0; i < 5; ++i) text1 += text1;
        QString text2 = QString(text);
        for (int i = 0; i < 5; ++i) text2 += text2;
        edit1->setPlainText(text1);
        edit2->setPlainText(text2);
        edit2->setWordWrapMode(QTextOption::NoWrap);
        QPushButton *btn1 = new QPushButton(QLatin1String("AcceptTouchEvents"));
        connect(btn1, SIGNAL(clicked()), edit1, SLOT(acceptTouch()));
        QPushButton *btn2 = new QPushButton(QLatin1String("AcceptTouchEvents"));
        connect(btn2, SIGNAL(clicked()), edit2, SLOT(acceptTouch()));
        layout->addWidget(btn1, 0, 0);
        layout->addWidget(btn2, 0, 1);
        layout->addWidget(edit1, 1, 0);
        layout->addWidget(edit2, 1, 1);
        tw->addTab(tab, QLatin1String("QTextEdit"));
    }
    {
        QWidget *tab = new QWidget;
        QGridLayout *layout = new QGridLayout(tab);
        QPlainTextEdit *edit1 = new PlainTextEdit;
        QPlainTextEdit *edit2 = new PlainTextEdit;
        QString text1 = QString(text).replace(' ', '\n');
        for (int i = 0; i < 5; ++i) text1 += text1;
        QString text2 = QString(text);
        for (int i = 0; i < 5; ++i) text2 += text2;
        edit1->setPlainText(text1);
        edit2->setPlainText(text2);
        edit2->setWordWrapMode(QTextOption::NoWrap);
        QPushButton *btn1 = new QPushButton(QLatin1String("AcceptTouchEvents"));
        connect(btn1, SIGNAL(clicked()), edit1, SLOT(acceptTouch()));
        QPushButton *btn2 = new QPushButton(QLatin1String("AcceptTouchEvents"));
        connect(btn2, SIGNAL(clicked()), edit2, SLOT(acceptTouch()));
        layout->addWidget(btn1, 0, 0);
        layout->addWidget(btn2, 0, 1);
        layout->addWidget(edit1, 1, 0);
        layout->addWidget(edit2, 1, 1);
        tw->addTab(tab, QLatin1String("QPlainTextEdit"));
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
