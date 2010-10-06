/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "mainwindow.h"
#include "colorbutton.h"
#include "previewframe.h"
#include "paletteeditoradvanced.h"

#include <QLabel>
#include <QApplication>
#include <QComboBox>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QAction>
#include <QStatusBar>
#include <QSettings>
#include <QMessageBox>
#include <QStyle>
#include <QtEvents>
#include <QInputContext>
#include <QInputContextFactory>
#include <QtDebug>

#include <stdlib.h>

#ifndef QT_NO_GSTREAMER
#include <gst/gst.h>
#endif
#ifdef HAVE_PHONON
#include <phonon/phononnamespace.h>
#endif

#include <QtGui/private/qt_x11_p.h>

QT_BEGIN_NAMESPACE

// from qapplication.cpp and qapplication_x11.cpp - These are NOT for
// external use ignore them
// extern bool Q_CORE_EXPORT qt_resolve_symlinks;

static const char *appearance_text =
"<p><b><font size+=2>Appearance</font></b></p>"
"<hr>"
"<p>Use this tab to customize the appearance of your Qt applications.</p>"
"<p>You can select the default GUI Style from the drop down list and "
"customize the colors.</p>"
"<p>Any GUI Style plugins in your plugin path will automatically be added "
"to the list of built-in Qt styles. (See the Library Paths tab for "
"information on adding new plugin paths.)</p>"
"<p>When you choose 3-D Effects and Window Background colors, the Qt "
"Configuration program will automatically generate a palette for you. "
"To customize colors further, press the Tune Palette button to open "
"the advanced palette editor."
"<p>The Preview Window shows what the selected Style and colors look "
"like.";

static const char *font_text =
"<p><b><font size+=2>Fonts</font></b></p>"
"<hr>"
"<p>Use this tab to select the default font for your Qt applications. "
"The selected font is shown (initially as 'Sample Text') in the line "
"edit below the Family, "
"Style and Point Size drop down lists.</p>"
"<p>Qt has a powerful font substitution feature that allows you to "
"specify a list of substitute fonts.  Substitute fonts are used "
"when a font cannot be loaded, or if the specified font doesn't have "
"a particular character."
"<p>For example, if you select the font Lucida, which doesn't have Korean "
"characters, but need to show some Korean text using the Mincho font family "
"you can do so by adding Mincho to the list. Once Mincho is added, any "
"Korean characters that are not found in the Lucida font will be taken "
"from the Mincho font.  Because the font substitutions are "
"lists, you can also select multiple families, such as Song Ti (for "
"use with Chinese text).";

static const char *interface_text =
"<p><b><font size+=2>Interface</font></b></p>"
"<hr>"
"<p>Use this tab to customize the feel of your Qt applications.</p>"
"<p>If the Resolve Symlinks checkbox is checked Qt will follow symlinks "
"when handling URLs. For example, in the file dialog, if this setting is turned "
"on and /usr/tmp is a symlink to /var/tmp, entering the /usr/tmp directory "
"will cause the file dialog to change to /var/tmp.  With this setting turned "
"off, symlinks are not resolved or followed.</p>"
"<p>The Global Strut setting is useful for people who require a "
"minimum size for all widgets (e.g. when using a touch panel or for users "
"who are visually impaired).  Leaving the Global Strut width and height "
"at 0 will disable the Global Strut feature</p>"
"<p>XIM (Extended Input Methods) are used for entering characters in "
"languages that have large character sets, for example, Chinese and "
"Japanese.";
// ### What does the 'Enhanced support for languages written R2L do?

static const char *printer_text =
"<p><b><font size+=2>Printer</font></b></p>"
"<hr>"
"<p>Use this tab to configure the way Qt generates output for the printer."
"You can specify if Qt should try to embed fonts into its generated output."
"If you enable font embedding, the resulting postscript will be more "
"portable and will more accurately reflect the "
"visual output on the screen; however the resulting postscript file "
"size will be bigger."
"<p>When using font embedding you can select additional directories where "
"Qt should search for embeddable font files.  By default, the X "
"server font path is used.";

static const char *phonon_text =
"<p><b><font size+=2>Phonon</font></b></p>"
"<hr>"
"<p>Use this tab to configure the Phonon GStreamer multimedia backend. "
"<p>It is reccommended to leave all settings on \"Auto\" to let "
"Phonon determine your settings automatically.";


QPalette::ColorGroup MainWindow::groupFromIndex(int item)
{
    switch (item) {
    case 0:
    default:
        return QPalette::Active;
    case 1:
        return QPalette::Inactive;
    case 2:
        return QPalette::Disabled;
    }
}

static void setStyleHelper(QWidget *w, QStyle *s)
{
    const QObjectList children = w->children();
    for (int i = 0; i < children.size(); ++i) {
        QObject *child = children.at(i);
        if (child->isWidgetType())
            setStyleHelper((QWidget *) child, s);
    }
    w->setStyle(s);
}

MainWindow::MainWindow()
    : QMainWindow(), editPalette(palette()), previewPalette(palette()), previewstyle(0)
{
    setupUi(this);
    statusBar();

    // signals and slots connections
    connect(fontpathlineedit, SIGNAL(returnPressed()), SLOT(addFontpath()));
    connect(PushButton15, SIGNAL(clicked()), SLOT(addFontpath()));
    connect(PushButton1, SIGNAL(clicked()), SLOT(addSubstitute()));
    connect(PushButton14, SIGNAL(clicked()), SLOT(browseFontpath()));
    connect(stylecombo, SIGNAL(activated(int)), SLOT(buildFont()));
    connect(psizecombo, SIGNAL(activated(int)), SLOT(buildFont()));
    connect(PushButton12, SIGNAL(clicked()), SLOT(downFontpath()));
    connect(PushButton3, SIGNAL(clicked()), SLOT(downSubstitute()));
    connect(familycombo, SIGNAL(activated(QString)), SLOT(familySelected(QString)));
    connect(fileExitAction, SIGNAL(activated()), SLOT(fileExit()));
    connect(fileSaveAction, SIGNAL(activated()), SLOT(fileSave()));
    connect(helpAboutAction, SIGNAL(activated()), SLOT(helpAbout()));
    connect(helpAboutQtAction, SIGNAL(activated()), SLOT(helpAboutQt()));
    connect(TabWidget3, SIGNAL(currentChanged(QWidget*)), SLOT(pageChanged(QWidget*)));
    connect(paletteCombo, SIGNAL(activated(int)), SLOT(paletteSelected(int)));
    connect(PushButton13, SIGNAL(clicked()), SLOT(removeFontpath()));
    connect(PushButton4, SIGNAL(clicked()), SLOT(removeSubstitute()));
    connect(toolboxeffect, SIGNAL(activated(int)), SLOT(somethingModified()));
    connect(dcispin, SIGNAL(valueChanged(int)), SLOT(somethingModified()));
    connect(cfispin, SIGNAL(valueChanged(int)), SLOT(somethingModified()));
    connect(wslspin, SIGNAL(valueChanged(int)), SLOT(somethingModified()));
    connect(menueffect, SIGNAL(activated(int)), SLOT(somethingModified()));
    connect(comboeffect, SIGNAL(activated(int)), SLOT(somethingModified()));
    connect(audiosinkCombo, SIGNAL(activated(int)), SLOT(somethingModified()));
    connect(videomodeCombo, SIGNAL(activated(int)), SLOT(somethingModified()));
    connect(tooltipeffect, SIGNAL(activated(int)), SLOT(somethingModified()));
    connect(strutwidth, SIGNAL(valueChanged(int)), SLOT(somethingModified()));
    connect(strutheight, SIGNAL(valueChanged(int)), SLOT(somethingModified()));
    connect(effectcheckbox, SIGNAL(toggled(bool)), SLOT(somethingModified()));
    connect(resolvelinks, SIGNAL(toggled(bool)), SLOT(somethingModified()));
    connect(fontembeddingcheckbox, SIGNAL(clicked()), SLOT(somethingModified()));
    connect(rtlExtensions, SIGNAL(toggled(bool)), SLOT(somethingModified()));
    connect(inputStyle, SIGNAL(activated(int)), SLOT(somethingModified()));
    connect(inputMethod, SIGNAL(activated(int)), SLOT(somethingModified()));
    connect(gstylecombo, SIGNAL(activated(QString)), SLOT(styleSelected(QString)));
    connect(familysubcombo, SIGNAL(activated(QString)), SLOT(substituteSelected(QString)));
    connect(btnAdvanced, SIGNAL(clicked()), SLOT(tunePalette()));
    connect(PushButton11, SIGNAL(clicked()), SLOT(upFontpath()));
    connect(PushButton2, SIGNAL(clicked()), SLOT(upSubstitute()));

    modified = true;
    desktopThemeName = tr("Desktop Settings (Default)");
    QStringList gstyles = QStyleFactory::keys();
    gstyles.sort();
    gstylecombo->addItem(desktopThemeName);
    gstylecombo->setItemData(gstylecombo->findText(desktopThemeName),
                                tr("Choose style and palette based on your desktop settings."), Qt::ToolTipRole);
    gstylecombo->addItems(gstyles);

    QSettings settings(QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("Qt"));

    QString currentstyle = settings.value(QLatin1String("style")).toString();
    if (currentstyle.isEmpty()) {
        gstylecombo->setCurrentIndex(gstylecombo->findText(desktopThemeName));
        currentstyle = QApplication::style()->objectName();
    } else {
        int index = gstylecombo->findText(currentstyle, Qt::MatchFixedString);
        if (index != -1) {
            gstylecombo->setCurrentIndex(index);
        } else { // we give up
            gstylecombo->addItem(QLatin1String("Unknown"));
            gstylecombo->setCurrentIndex(gstylecombo->count() - 1);
        }
    }
    buttonMainColor->setColor(palette().color(QPalette::Active,
                                              QPalette::Button));
    buttonWindowColor->setColor(palette().color(QPalette::Active,
                                               QPalette::Window));
    connect(buttonMainColor, SIGNAL(colorChanged(QColor)),
                this, SLOT(buildPalette()));
    connect(buttonWindowColor, SIGNAL(colorChanged(QColor)),
                this, SLOT(buildPalette()));

    if (X11->desktopEnvironment == DE_KDE)
        colorConfig->hide();
    else
        labelKDENote->hide();

    QFontDatabase db;
    QStringList families = db.families();
    familycombo->addItems(families);

    QStringList fs = families;
    QStringList fs2 = QFont::substitutions();
    QStringList::Iterator fsit = fs2.begin();
    while (fsit != fs2.end()) {
        if (!fs.contains(*fsit))
            fs += *fsit;
        fsit++;
    }
    fs.sort();
    familysubcombo->addItems(fs);

    choosesubcombo->addItems(families);
    QList<int> sizes = db.standardSizes();
    foreach(int i, sizes)
        psizecombo->addItem(QString::number(i));

    dcispin->setValue(QApplication::doubleClickInterval());
    cfispin->setValue(QApplication::cursorFlashTime());
    wslspin->setValue(QApplication::wheelScrollLines());
    // #############
//    resolvelinks->setChecked(qt_resolve_symlinks);

    effectcheckbox->setChecked(QApplication::isEffectEnabled(Qt::UI_General));
    effectbase->setEnabled(effectcheckbox->isChecked());

    if (QApplication::isEffectEnabled(Qt::UI_FadeMenu))
        menueffect->setCurrentIndex(2);
    else if (QApplication::isEffectEnabled(Qt::UI_AnimateMenu))
        menueffect->setCurrentIndex(1);

    if (QApplication::isEffectEnabled(Qt::UI_AnimateCombo))
        comboeffect->setCurrentIndex(1);

    if (QApplication::isEffectEnabled(Qt::UI_FadeTooltip))
        tooltipeffect->setCurrentIndex(2);
    else if (QApplication::isEffectEnabled(Qt::UI_AnimateTooltip))
        tooltipeffect->setCurrentIndex(1);

    if (QApplication::isEffectEnabled(Qt::UI_AnimateToolBox))
        toolboxeffect->setCurrentIndex(1);

    QSize globalStrut = QApplication::globalStrut();
    strutwidth->setValue(globalStrut.width());
    strutheight->setValue(globalStrut.height());

    // find the default family
    QStringList::Iterator sit = families.begin();
    int i = 0, possible = -1;
    while (sit != families.end()) {
        if (*sit == QApplication::font().family())
            break;
        if ((*sit).contains(QApplication::font().family()))
            possible = i;

        i++;
        sit++;
    }
    if (sit == families.end())
        i = possible;
    if (i == -1) // no clue about the current font
        i = 0;

    familycombo->setCurrentIndex(i);

    QStringList styles = db.styles(familycombo->currentText());
    stylecombo->addItems(styles);

    QString stylestring = db.styleString(QApplication::font());
    sit = styles.begin();
    i = 0;
    possible = -1;
    while (sit != styles.end()) {
        if (*sit == stylestring)
            break;
        if ((*sit).contains(stylestring))
            possible = i;

        i++;
        sit++;
    }
    if (sit == styles.end())
        i = possible;
    if (i == -1) // no clue about the current font
        i = 0;
    stylecombo->setCurrentIndex(i);

    i = 0;
    for (int psize = QApplication::font().pointSize(); i < psizecombo->count(); ++i) {
        const int sz = psizecombo->itemText(i).toInt();
        if (sz == psize) {
            psizecombo->setCurrentIndex(i);
            break;
        } else if(sz > psize) {
            psizecombo->insertItem(i, QString::number(psize));
            psizecombo->setCurrentIndex(i);
            break;
        }
    }

    QStringList subs = QFont::substitutes(familysubcombo->currentText());
    sublistbox->clear();
    sublistbox->insertItems(0, subs);

    rtlExtensions->setChecked(settings.value(QLatin1String("useRtlExtensions"), false).toBool());

#ifdef Q_WS_X11
    QString settingsInputStyle = settings.value(QLatin1String("XIMInputStyle")).toString();
    if (!settingsInputStyle.isEmpty())
      inputStyle->setCurrentIndex(inputStyle->findText(settingsInputStyle));
#else
    inputStyle->hide();
    inputStyleLabel->hide();
#endif

#if defined(Q_WS_X11) && !defined(QT_NO_XIM)
    QStringList inputMethods = QInputContextFactory::keys();
    int inputMethodIndex = -1;
    QString defaultInputMethod = settings.value(QLatin1String("DefaultInputMethod"), QLatin1String("xim")).toString();
    for (int i = inputMethods.size()-1; i >= 0; --i) {
        const QString &im = inputMethods.at(i);
        if (im.contains(QLatin1String("imsw"))) {
            inputMethods.removeAt(i);
            if (inputMethodIndex > i)
                --inputMethodIndex;
        } else if (im == defaultInputMethod) {
            inputMethodIndex = i;
        }
    }
    if (inputMethodIndex == -1 && !inputMethods.isEmpty())
        inputMethodIndex = 0;
    inputMethod->addItems(inputMethods);
    inputMethod->setCurrentIndex(inputMethodIndex);
#else
    inputMethod->hide();
    inputMethodLabel->hide();
#endif

    fontembeddingcheckbox->setChecked(settings.value(QLatin1String("embedFonts"), true).toBool());
    fontpaths = settings.value(QLatin1String("fontPath")).toStringList();
    fontpathlistbox->insertItems(0, fontpaths);

    audiosinkCombo->addItem(tr("Auto (default)"), QLatin1String("Auto"));
    audiosinkCombo->setItemData(audiosinkCombo->findText(tr("Auto (default)")),
                                tr("Choose audio output automatically."), Qt::ToolTipRole);
    audiosinkCombo->addItem(tr("aRts"), QLatin1String("artssink"));
    audiosinkCombo->setItemData(audiosinkCombo->findText(tr("aRts")),
                                tr("Experimental aRts support for GStreamer."), Qt::ToolTipRole);
#ifdef HAVE_PHONON
    phononVersionLabel->setText(QLatin1String(Phonon::phononVersion()));
#endif
#ifndef QT_NO_GSTREAMER
    if (gst_init_check(0, 0, 0)) {
        gchar *versionString = gst_version_string();
        gstversionLabel->setText(QLatin1String(versionString));
        g_free(versionString);
        GList *factoryList = gst_registry_get_feature_list(gst_registry_get_default (), GST_TYPE_ELEMENT_FACTORY);
        QString name, klass, description;
        for (GList *iter = g_list_first(factoryList) ; iter != NULL ; iter = g_list_next(iter)) {
            GstPluginFeature *feature = GST_PLUGIN_FEATURE(iter->data);
            klass = QLatin1String(gst_element_factory_get_klass(GST_ELEMENT_FACTORY(feature)));
            if (klass == QLatin1String("Sink/Audio")) {
                name = QLatin1String(GST_PLUGIN_FEATURE_NAME(feature));
                if (name == QLatin1String("sfsink"))
                    continue; //useless to output audio to file when you cannot set the file path
                else if (name == QLatin1String("autoaudiosink"))
                    continue; //This is used implicitly from the auto setting
                GstElement *sink = gst_element_factory_make (qPrintable(name), NULL);
                if (sink) {
                    description = QLatin1String(gst_element_factory_get_description (GST_ELEMENT_FACTORY(feature)));
                    audiosinkCombo->addItem(name, name);
                    audiosinkCombo->setItemData(audiosinkCombo->findText(name), description, Qt::ToolTipRole);
                    gst_object_unref (sink);
                }
            }
        }
        g_list_free(factoryList);
    }
#else
    tab4->setEnabled(false);
    phononLabel->setText(tr("Phonon GStreamer backend not available."));
#endif

    videomodeCombo->addItem(tr("Auto (default)"), QLatin1String("Auto"));
    videomodeCombo->setItemData(videomodeCombo->findText(tr("Auto (default)")), tr("Choose render method automatically"), Qt::ToolTipRole);
#ifdef Q_WS_X11
    videomodeCombo->addItem(tr("X11"), QLatin1String("X11"));
    videomodeCombo->setItemData(videomodeCombo->findText(tr("X11")), tr("Use X11 Overlays"), Qt::ToolTipRole);
#endif
#ifndef QT_NO_OPENGL
    videomodeCombo->addItem(tr("OpenGL"), QLatin1String("OpenGL"));
    videomodeCombo->setItemData(videomodeCombo->findText(tr("OpenGL")), tr("Use OpenGL if available"), Qt::ToolTipRole);
#endif
    videomodeCombo->addItem(tr("Software"), QLatin1String("Software"));
    videomodeCombo->setItemData(videomodeCombo->findText(tr("Software")), tr("Use simple software rendering"), Qt::ToolTipRole);

    QString audioSink = settings.value(QLatin1String("audiosink"), QLatin1String("Auto")).toString();
    QString videoMode = settings.value(QLatin1String("videomode"), QLatin1String("Auto")).toString();
    audiosinkCombo->setCurrentIndex(audiosinkCombo->findData(audioSink));
    videomodeCombo->setCurrentIndex(videomodeCombo->findData(videoMode));

    settings.endGroup(); // Qt

    helpview->setText(tr(appearance_text));

    setModified(false);
    updateStyleLayout();
}

MainWindow::~MainWindow()
{
}

#ifdef Q_WS_X11
extern void qt_x11_apply_settings_in_all_apps();
#endif

void MainWindow::fileSave()
{
    if (! modified) {
        statusBar()->showMessage(tr("No changes to be saved."), 2000);
        return;
    }

    statusBar()->showMessage(tr("Saving changes..."));

    {
        QSettings settings(QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("Qt"));
        QFontDatabase db;
        QFont font = db.font(familycombo->currentText(),
                             stylecombo->currentText(),
                             psizecombo->currentText().toInt());

        QStringList actcg, inactcg, discg;
        bool overrideDesktopSettings = (gstylecombo->currentText() != desktopThemeName);
        if (overrideDesktopSettings) {
            int i;
            for (i = 0; i < QPalette::NColorRoles; i++)
                actcg << editPalette.color(QPalette::Active,
                                           (QPalette::ColorRole) i).name();
            for (i = 0; i < QPalette::NColorRoles; i++)
                inactcg << editPalette.color(QPalette::Inactive,
                                             (QPalette::ColorRole) i).name();
            for (i = 0; i < QPalette::NColorRoles; i++)
                discg << editPalette.color(QPalette::Disabled,
                                           (QPalette::ColorRole) i).name();
        }

        settings.setValue(QLatin1String("font"), font.toString());
        settings.setValue(QLatin1String("Palette/active"), actcg);
        settings.setValue(QLatin1String("Palette/inactive"), inactcg);
        settings.setValue(QLatin1String("Palette/disabled"), discg);

        settings.setValue(QLatin1String("fontPath"), fontpaths);
        settings.setValue(QLatin1String("embedFonts"), fontembeddingcheckbox->isChecked());
        settings.setValue(QLatin1String("style"), overrideDesktopSettings ? gstylecombo->currentText() : QString());

        settings.setValue(QLatin1String("doubleClickInterval"), dcispin->value());
        settings.setValue(QLatin1String("cursorFlashTime"), cfispin->value() == 9 ? 0 : cfispin->value());
        settings.setValue(QLatin1String("wheelScrollLines"), wslspin->value());
        settings.setValue(QLatin1String("resolveSymlinks"), resolvelinks->isChecked());

        QSize strut(strutwidth->value(), strutheight->value());
        settings.setValue(QLatin1String("globalStrut/width"), strut.width());
        settings.setValue(QLatin1String("globalStrut/height"), strut.height());

        settings.setValue(QLatin1String("useRtlExtensions"), rtlExtensions->isChecked());

#ifdef Q_WS_X11
        QString style = inputStyle->currentText();
        QString str = QLatin1String("On The Spot");
        if (style == trUtf8("Over The Spot"))
            str = QLatin1String("Over The Spot");
        else if (style == trUtf8("Off The Spot"))
            str = QLatin1String("Off The Spot");
        else if (style == trUtf8("Root"))
            str = QLatin1String("Root");
        settings.setValue(QLatin1String("XIMInputStyle"), str);
#endif
#if defined(Q_WS_X11) && !defined(QT_NO_XIM)
        settings.setValue(QLatin1String("DefaultInputMethod"), inputMethod->currentText());
#endif

        QString audioSink = settings.value(QLatin1String("audiosink"), QLatin1String("Auto")).toString();
        QString videoMode = settings.value(QLatin1String("videomode"), QLatin1String("Auto")).toString();
        settings.setValue(QLatin1String("audiosink"), audiosinkCombo->itemData(audiosinkCombo->currentIndex()));
        settings.setValue(QLatin1String("videomode"), videomodeCombo->itemData(videomodeCombo->currentIndex()));

        QStringList effects;
        if (effectcheckbox->isChecked()) {
            effects << QLatin1String("general");

            switch (menueffect->currentIndex()) {
            case 1: effects << QLatin1String("animatemenu"); break;
            case 2: effects << QLatin1String("fademenu"); break;
            }

            switch (comboeffect->currentIndex()) {
            case 1: effects << QLatin1String("animatecombo"); break;
            }

            switch (tooltipeffect->currentIndex()) {
            case 1: effects << QLatin1String("animatetooltip"); break;
            case 2: effects << QLatin1String("fadetooltip"); break;
            }

            switch (toolboxeffect->currentIndex()) {
            case 1: effects << QLatin1String("animatetoolbox"); break;
            }
        } else
            effects << QLatin1String("none");
        settings.setValue(QLatin1String("GUIEffects"), effects);

        QStringList familysubs = QFont::substitutions();
        QStringList::Iterator fit = familysubs.begin();
        settings.beginGroup(QLatin1String("Font Substitutions"));
        while (fit != familysubs.end()) {
            QStringList subs = QFont::substitutes(*fit);
            settings.setValue(*fit, subs);
            fit++;
        }
        settings.endGroup(); // Font Substitutions
        settings.endGroup(); // Qt
    }

#if defined(Q_WS_X11)
    qt_x11_apply_settings_in_all_apps();
#endif // Q_WS_X11

    setModified(false);
    statusBar()->showMessage(QLatin1String("Saved changes."));
}

void MainWindow::fileExit()
{
    qApp->closeAllWindows();
}

void MainWindow::setModified(bool m)
{
    if (modified == m)
        return;

    modified = m;
    fileSaveAction->setEnabled(m);
}

void MainWindow::buildPalette()
{
    QPalette temp(buttonMainColor->color(), buttonWindowColor->color());
    for (int i = 0; i < QPalette::NColorGroups; i++)
        temp = PaletteEditorAdvanced::buildEffect(QPalette::ColorGroup(i), temp);

    editPalette = temp;

    updateColorButtons();

    setModified(true);
}

void MainWindow::setPreviewPalette(const QPalette &pal)
{
    QPalette::ColorGroup colorGroup = groupFromIndex(paletteCombo->currentIndex());

    for (int i = 0; i < QPalette::NColorGroups; i++) {
        for (int j = 0; j < QPalette::NColorRoles; j++) {
            QPalette::ColorGroup targetGroup = QPalette::ColorGroup(i);
            QPalette::ColorRole targetRole = QPalette::ColorRole(j);
            previewPalette.setColor(targetGroup, targetRole, pal.color(colorGroup, targetRole));
        }
    }

    previewFrame->setPreviewPalette(previewPalette);
}

void MainWindow::updateColorButtons()
{
    buttonMainColor->setColor(editPalette.color(QPalette::Active, QPalette::Button));
    buttonWindowColor->setColor(editPalette.color(QPalette::Active, QPalette::Window));
}

void MainWindow::tunePalette()
{
    bool ok;
    QPalette pal = PaletteEditorAdvanced::getPalette(&ok, editPalette,
                                                     backgroundRole(), this);
    if (!ok)
        return;

    editPalette = pal;
    setPreviewPalette(editPalette);
    setModified(true);
}

void MainWindow::paletteSelected(int)
{
    setPreviewPalette(editPalette);
}

void MainWindow::updateStyleLayout()
{
    QString currentStyle = gstylecombo->currentText();
    bool autoStyle = (currentStyle == desktopThemeName);
    previewFrame->setPreviewVisible(!autoStyle);
    groupAutoPalette->setEnabled(currentStyle.toLower() != QLatin1String("gtk") && !autoStyle);
}

void MainWindow::styleSelected(const QString &stylename)
{
    QStyle *style = 0;
    if (stylename == desktopThemeName) {
        setModified(true);
    } else {
        style = QStyleFactory::create(stylename);
        if (!style)
            return;
        setStyleHelper(previewFrame, style);
        delete previewstyle;
        previewstyle = style;
        setModified(true);
    }
    updateStyleLayout();
}

void MainWindow::familySelected(const QString &family)
{
    QFontDatabase db;
    QStringList styles = db.styles(family);
    stylecombo->clear();
    stylecombo->addItems(styles);
    familysubcombo->addItem(family);
    buildFont();
}

void MainWindow::buildFont()
{
    QFontDatabase db;
    QFont font = db.font(familycombo->currentText(),
                         stylecombo->currentText(),
                         psizecombo->currentText().toInt());
    samplelineedit->setFont(font);
    setModified(true);
}

void MainWindow::substituteSelected(const QString &family)
{
    QStringList subs = QFont::substitutes(family);
    sublistbox->clear();
    sublistbox->insertItems(0, subs);
}

void MainWindow::removeSubstitute()
{
    if (!sublistbox->currentItem())
        return;

    int row = sublistbox->currentRow();
    QStringList subs = QFont::substitutes(familysubcombo->currentText());
    subs.removeAt(sublistbox->currentRow());
    sublistbox->clear();
    sublistbox->insertItems(0, subs);
    if (row > sublistbox->count())
        row = sublistbox->count() - 1;
    sublistbox->setCurrentRow(row);
    QFont::removeSubstitution(familysubcombo->currentText());
    QFont::insertSubstitutions(familysubcombo->currentText(), subs);
    setModified(true);
}

void MainWindow::addSubstitute()
{
    if (!sublistbox->currentItem()) {
        QFont::insertSubstitution(familysubcombo->currentText(), choosesubcombo->currentText());
        QStringList subs = QFont::substitutes(familysubcombo->currentText());
        sublistbox->clear();
        sublistbox->insertItems(0, subs);
        setModified(true);
        return;
    }

    int row = sublistbox->currentRow();
    QFont::insertSubstitution(familysubcombo->currentText(), choosesubcombo->currentText());
    QStringList subs = QFont::substitutes(familysubcombo->currentText());
    sublistbox->clear();
    sublistbox->insertItems(0, subs);
    sublistbox->setCurrentRow(row);
    setModified(true);
}

void MainWindow::downSubstitute()
{
    if (!sublistbox->currentItem() || sublistbox->currentRow() >= sublistbox->count())
        return;

    int row = sublistbox->currentRow();
    QStringList subs = QFont::substitutes(familysubcombo->currentText());
    QString fam = subs.at(row);
    subs.removeAt(row);
    subs.insert(row + 1, fam);
    sublistbox->clear();
    sublistbox->insertItems(0, subs);
    sublistbox->setCurrentRow(row + 1);
    QFont::removeSubstitution(familysubcombo->currentText());
    QFont::insertSubstitutions(familysubcombo->currentText(), subs);
    setModified(true);
}

void MainWindow::upSubstitute()
{
    if (!sublistbox->currentItem() || sublistbox->currentRow() < 1)
        return;

    int row = sublistbox->currentRow();
    QStringList subs = QFont::substitutes(familysubcombo->currentText());
    QString fam = subs.at(row);
    subs.removeAt(row);
    subs.insert(row-1, fam);
    sublistbox->clear();
    sublistbox->insertItems(0, subs);
    sublistbox->setCurrentRow(row - 1);
    QFont::removeSubstitution(familysubcombo->currentText());
    QFont::insertSubstitutions(familysubcombo->currentText(), subs);
    setModified(true);
}

void MainWindow::removeFontpath()
{
    if (!fontpathlistbox->currentItem())
        return;

    int row = fontpathlistbox->currentRow();
    fontpaths.removeAt(row);
    fontpathlistbox->clear();
    fontpathlistbox->insertItems(0, fontpaths);
    if (row > fontpathlistbox->count())
        row = fontpathlistbox->count() - 1;
    fontpathlistbox->setCurrentRow(row);
    setModified(true);
}

void MainWindow::addFontpath()
{
    if (fontpathlineedit->text().isEmpty())
        return;

    if (!fontpathlistbox->currentItem()) {
        fontpaths.append(fontpathlineedit->text());
        fontpathlistbox->clear();
        fontpathlistbox->insertItems(0, fontpaths);
        setModified(true);

        return;
    }

    int row = fontpathlistbox->currentRow();
    fontpaths.insert(row + 1, fontpathlineedit->text());
    fontpathlistbox->clear();
    fontpathlistbox->insertItems(0, fontpaths);
    fontpathlistbox->setCurrentRow(row);
    setModified(true);
}

void MainWindow::downFontpath()
{
    if (!fontpathlistbox->currentItem()
        || fontpathlistbox->currentRow() >= (fontpathlistbox->count() - 1)) {
        return;
    }

    int row = fontpathlistbox->currentRow();
    QString fam = fontpaths.at(row);
    fontpaths.removeAt(row);
    fontpaths.insert(row + 1, fam);
    fontpathlistbox->clear();
    fontpathlistbox->insertItems(0, fontpaths);
    fontpathlistbox->setCurrentRow(row + 1);
    setModified(true);
}

void MainWindow::upFontpath()
{
    if (!fontpathlistbox->currentItem() || fontpathlistbox->currentRow() < 1)
        return;

    int row = fontpathlistbox->currentRow();
    QString fam = fontpaths.at(row);
    fontpaths.removeAt(row);
    fontpaths.insert(row - 1, fam);
    fontpathlistbox->clear();
    fontpathlistbox->insertItems(0, fontpaths);
    fontpathlistbox->setCurrentRow(row - 1);
    setModified(true);
}

void MainWindow::browseFontpath()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Select a Directory"));
    if (dirname.isNull())
        return;

   fontpathlineedit->setText(dirname);
}

void MainWindow::somethingModified()
{
    setModified(true);
}

void MainWindow::helpAbout()
{
    QMessageBox box(this);
    box.setText(tr("<h3>%1</h3>"
                   "<br/>Version %2"
                   "<br/><br/>Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).")
                   .arg(tr("Qt Configuration")).arg(QLatin1String(QT_VERSION_STR)));
    box.setWindowTitle(tr("Qt Configuration"));
    box.setIcon(QMessageBox::NoIcon);
    box.exec();
}

void MainWindow::helpAboutQt()
{
    QMessageBox::aboutQt(this, tr("Qt Configuration"));
}

void MainWindow::pageChanged(QWidget *page)
{
    if (page == tab)
        helpview->setText(tr(interface_text));
    else if (page == tab1)
        helpview->setText(tr(appearance_text));
    else if (page == tab2)
        helpview->setText(tr(font_text));
    else if (page == tab3)
        helpview->setText(tr(printer_text));
    else if (page == tab4)
        helpview->setText(tr(phonon_text));
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (modified) {
        switch (QMessageBox::warning(this, tr("Save Changes"),
                                     tr("Save changes to settings?"),
                                     (QMessageBox::Yes | QMessageBox::No
                                     | QMessageBox::Cancel))) {
        case QMessageBox::Yes: // save
            qApp->processEvents();
            fileSave();

            // fall through intended
        case QMessageBox::No: // don't save
            e->accept();
            break;

        case QMessageBox::Cancel: // cancel
            e->ignore();
            break;

        default: break;
        }
    } else
        e->accept();
}

QT_END_NAMESPACE
