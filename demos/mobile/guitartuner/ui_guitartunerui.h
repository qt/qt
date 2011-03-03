/********************************************************************************
** Form generated from reading UI file 'guitartunerui.ui'
**
** Created: Wed Mar 2 14:59:33 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUITARTUNERUI_H
#define UI_GUITARTUNERUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GuitarTunerUI
{
public:
    QVBoxLayout *verticalLayout_2;
    QLabel *tabulatorIcon;
    QSlider *correctSoundSlider;
    QHBoxLayout *horizontalLayout;
    QToolButton *buttonPrev;
    QLabel *noteLabel;
    QToolButton *buttonNext;
    QPushButton *modeButton;
    QFrame *line;
    QHBoxLayout *horizontalLayout_2;
    QSlider *soundSlider;
    QToolButton *soundButton;
    QLabel *micSensitivityLabel;

    void setupUi(QWidget *GuitarTunerUI)
    {
        if (GuitarTunerUI->objectName().isEmpty())
            GuitarTunerUI->setObjectName(QString::fromUtf8("GuitarTunerUI"));
        GuitarTunerUI->resize(187, 279);
        verticalLayout_2 = new QVBoxLayout(GuitarTunerUI);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tabulatorIcon = new QLabel(GuitarTunerUI);
        tabulatorIcon->setObjectName(QString::fromUtf8("tabulatorIcon"));
        tabulatorIcon->setText(QString::fromUtf8(""));
        tabulatorIcon->setPixmap(QPixmap(QString::fromUtf8(":/images/guitartab.svg")));
        tabulatorIcon->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(tabulatorIcon);

        correctSoundSlider = new QSlider(GuitarTunerUI);
        correctSoundSlider->setObjectName(QString::fromUtf8("correctSoundSlider"));
        correctSoundSlider->setEnabled(false);
        correctSoundSlider->setAutoFillBackground(false);
        correctSoundSlider->setMinimum(-50);
        correctSoundSlider->setMaximum(50);
        correctSoundSlider->setOrientation(Qt::Horizontal);
        correctSoundSlider->setTickPosition(QSlider::TicksAbove);
        correctSoundSlider->setTickInterval(50);

        verticalLayout_2->addWidget(correctSoundSlider);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonPrev = new QToolButton(GuitarTunerUI);
        buttonPrev->setObjectName(QString::fromUtf8("buttonPrev"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonPrev->sizePolicy().hasHeightForWidth());
        buttonPrev->setSizePolicy(sizePolicy);
        buttonPrev->setArrowType(Qt::LeftArrow);

        horizontalLayout->addWidget(buttonPrev);

        noteLabel = new QLabel(GuitarTunerUI);
        noteLabel->setObjectName(QString::fromUtf8("noteLabel"));
        noteLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(noteLabel);

        buttonNext = new QToolButton(GuitarTunerUI);
        buttonNext->setObjectName(QString::fromUtf8("buttonNext"));
        sizePolicy.setHeightForWidth(buttonNext->sizePolicy().hasHeightForWidth());
        buttonNext->setSizePolicy(sizePolicy);
        buttonNext->setAutoRaise(false);
        buttonNext->setArrowType(Qt::RightArrow);

        horizontalLayout->addWidget(buttonNext);


        verticalLayout_2->addLayout(horizontalLayout);

        modeButton = new QPushButton(GuitarTunerUI);
        modeButton->setObjectName(QString::fromUtf8("modeButton"));

        verticalLayout_2->addWidget(modeButton);

        line = new QFrame(GuitarTunerUI);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        soundSlider = new QSlider(GuitarTunerUI);
        soundSlider->setObjectName(QString::fromUtf8("soundSlider"));
        soundSlider->setValue(65);
        soundSlider->setOrientation(Qt::Vertical);

        horizontalLayout_2->addWidget(soundSlider);

        soundButton = new QToolButton(GuitarTunerUI);
        soundButton->setObjectName(QString::fromUtf8("soundButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/note.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/images/noteNo.svg"), QSize(), QIcon::Normal, QIcon::On);
        icon.addFile(QString::fromUtf8(":/images/noteNo.svg"), QSize(), QIcon::Active, QIcon::On);
        soundButton->setIcon(icon);
        soundButton->setIconSize(QSize(30, 30));
        soundButton->setCheckable(true);
        soundButton->setAutoRaise(true);

        horizontalLayout_2->addWidget(soundButton);

        micSensitivityLabel = new QLabel(GuitarTunerUI);
        micSensitivityLabel->setObjectName(QString::fromUtf8("micSensitivityLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(micSensitivityLabel->sizePolicy().hasHeightForWidth());
        micSensitivityLabel->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(micSensitivityLabel);


        verticalLayout_2->addLayout(horizontalLayout_2);


        retranslateUi(GuitarTunerUI);

        QMetaObject::connectSlotsByName(GuitarTunerUI);
    } // setupUi

    void retranslateUi(QWidget *GuitarTunerUI)
    {
        GuitarTunerUI->setWindowTitle(QApplication::translate("GuitarTunerUI", "GuitarTunerUI", 0, QApplication::UnicodeUTF8));
        buttonPrev->setText(QApplication::translate("GuitarTunerUI", "...", 0, QApplication::UnicodeUTF8));
        noteLabel->setText(QApplication::translate("GuitarTunerUI", "A", 0, QApplication::UnicodeUTF8));
        buttonNext->setText(QApplication::translate("GuitarTunerUI", "...", 0, QApplication::UnicodeUTF8));
        modeButton->setText(QApplication::translate("GuitarTunerUI", "Change mode", 0, QApplication::UnicodeUTF8));
        soundButton->setText(QApplication::translate("GuitarTunerUI", "...", 0, QApplication::UnicodeUTF8));
        micSensitivityLabel->setText(QApplication::translate("GuitarTunerUI", "microphone\n"
"sensitivity", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GuitarTunerUI: public Ui_GuitarTunerUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUITARTUNERUI_H
