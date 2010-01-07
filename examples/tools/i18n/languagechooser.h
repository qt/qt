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

#ifndef LANGUAGECHOOSER_H
#define LANGUAGECHOOSER_H

#include <QDialog>
#include <QMap>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
QT_END_NAMESPACE
class MainWindow;

class LanguageChooser : public QDialog
{
    Q_OBJECT

public:
    LanguageChooser(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void checkBoxToggled();
    void showAll();
    void hideAll();

private:
    QStringList findQmFiles();
    QString languageName(const QString &qmFile);
    QColor colorForLanguage(const QString &language);

    QGroupBox *groupBox;
    QDialogButtonBox *buttonBox;
    QAbstractButton *showAllButton;
    QAbstractButton *hideAllButton;
    QMap<QCheckBox *, QString> qmFileForCheckBoxMap;
    QMap<QCheckBox *, MainWindow *> mainWindowForCheckBoxMap;
};

#endif
