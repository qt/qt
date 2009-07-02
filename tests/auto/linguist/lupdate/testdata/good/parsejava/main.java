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
