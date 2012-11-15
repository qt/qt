/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

#include "tetrixboard.h"

#include <QtGui>
#include <QtScript>
#include <QUiLoader>

#ifndef QT_NO_SCRIPTTOOLS
#include <QtScriptTools>
#endif

struct QtMetaObject : private QObject
{
public:
    static const QMetaObject *get()
        { return &static_cast<QtMetaObject*>(0)->staticQtMetaObject; }
};

//! [0]
class TetrixUiLoader : public QUiLoader
{
public:
    TetrixUiLoader(QObject *parent = 0)
        : QUiLoader(parent)
        { }
    virtual QWidget *createWidget(const QString &className, QWidget *parent = 0,
                                  const QString &name = QString())
    {
        if (className == QLatin1String("TetrixBoard")) {
            QWidget *board = new TetrixBoard(parent);
            board->setObjectName(name);
            return board;
        }
        return QUiLoader::createWidget(className, parent, name);
    }
};
//! [0]

static QScriptValue evaluateFile(QScriptEngine &engine, const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    return engine.evaluate(file.readAll(), fileName);
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(tetrix);

//! [1]
    QApplication app(argc, argv);
    QScriptEngine engine;

    QScriptValue Qt = engine.newQMetaObject(QtMetaObject::get());
    Qt.setProperty("App", engine.newQObject(&app));
    engine.globalObject().setProperty("Qt", Qt);
//! [1]

#ifndef QT_NO_SCRIPTTOOLS
    QScriptEngineDebugger debugger;
    debugger.attachTo(&engine);
    QMainWindow *debugWindow = debugger.standardWindow();
    debugWindow->resize(1024, 640);
#endif

//! [2]
    evaluateFile(engine, ":/tetrixpiece.js");
    evaluateFile(engine, ":/tetrixboard.js");
    evaluateFile(engine, ":/tetrixwindow.js");
//! [2]

//! [3]
    TetrixUiLoader loader;
    QFile uiFile(":/tetrixwindow.ui");
    uiFile.open(QIODevice::ReadOnly);
    QWidget *ui = loader.load(&uiFile);
    uiFile.close();

    QScriptValue ctor = engine.evaluate("TetrixWindow");
    QScriptValue scriptUi = engine.newQObject(ui, QScriptEngine::ScriptOwnership);
    QScriptValue tetrix = ctor.construct(QScriptValueList() << scriptUi);
//! [3]

    QPushButton *debugButton = qFindChild<QPushButton*>(ui, "debugButton");
#ifndef QT_NO_SCRIPTTOOLS
    QObject::connect(debugButton, SIGNAL(clicked()),
                     debugger.action(QScriptEngineDebugger::InterruptAction),
                     SIGNAL(triggered()));
    QObject::connect(debugButton, SIGNAL(clicked()),
                     debugWindow, SLOT(show()));
#else
    debugButton->hide();
#endif

//! [4]
    ui->resize(550, 370);
    ui->show();

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    return app.exec();
//! [4]
}
