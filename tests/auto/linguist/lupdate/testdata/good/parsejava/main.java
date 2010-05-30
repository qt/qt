/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

// IMPORTANT!!!! If you want to add testdata to this file,
// always add it to the end in order to not change the linenumbers of translations!!!

package com.trolltech.examples;

public class I18N extends QDialog {

    private class MainWindow extends QMainWindow {
        private String foo = tr("pack class class");

        //: extra comment for t-tor
        private String bar = tr("pack class class extra");

        public MainWindow(QWidget parent) {
            super(parent);

            listWidget = new QListWidget();
            listWidget.addItem(tr("pack class class method"));

        }
    }

    public I18N(QWidget parent) {
        super(parent, new Qt.WindowFlags(Qt.WindowType.WindowStaysOnTopHint));

        tr("pack class method");

        tr("QT_LAYOUT_DIRECTION",
           "Translate this string to the string 'LTR' in left-to-right" +
           " languages or to 'RTL' in right-to-left languages (such as Hebrew" +
           " and Arabic) to get proper widget layout.");

        tr("%n files", "plural form", n);
        tr("%n cars", null, n);
        tr("Age: %1");
        tr("There are %n house(s)", "Plurals and function call", getCount());

        QTranslator trans;
        trans.translate("QTranslator", "Simple");
        trans.translate("QTranslator", "Simple", null);
        trans.translate("QTranslator", "Simple with comment", "with comment");
        trans.translate("QTranslator", "Plural without comment", null, 1);
        trans.translate("QTranslator", "Plural with comment", "comment 1", n);
        trans.translate("QTranslator", "Plural with comment", "comment 2", getCount());

        /*: with extra comment! */
        QCoreApplication.translate("Plurals, QCoreApplication", "%n house(s)", "Plurals and identifier", n);

        // FIXME: This will fail.
        //QApplication.tr("QT_LAYOUT_DIRECTION", "scoped to qapp");

    }

}
