/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "window.h"

Window::Window()
{
    localeCombo = new QComboBox;

    localeCombo->addItem("System", QLocale::system());

    int index = 0;
    for (int _lang = QLocale::C; _lang <= QLocale::LastLanguage; ++_lang) {
        QLocale::Language lang = static_cast<QLocale::Language>(_lang);
        QList<QLocale::Country> countries = QLocale::countriesForLanguage(lang);
        for (int i = 0; i < countries.count(); ++i) {
            QLocale::Country country = countries.at(i);
            QString label = QLocale::languageToString(lang);
            label += QLatin1Char('/');
            label += QLocale::countryToString(country);
            localeCombo->addItem(label, QLocale(lang, country));
            ++index;
        }
    }
    connect(localeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(localeChanged(int)));

    tabWidget = new QTabWidget;
    calendar = new CalendarWidget;
    connect(this, SIGNAL(localeChanged(QLocale)), calendar, SLOT(localeChanged(QLocale)));
    currency = new CurrencyWidget;
    connect(this, SIGNAL(localeChanged(QLocale)), currency, SLOT(localeChanged(QLocale)));
    languages = new LanguagesWidget;
    connect(this, SIGNAL(localeChanged(QLocale)), languages, SLOT(localeChanged(QLocale)));
    miscellaneous = new MiscWidget;
    connect(this, SIGNAL(localeChanged(QLocale)), miscellaneous, SLOT(localeChanged(QLocale)));

    localeName = new QLabel("Locale: foo_BAR");

    QWidget *w = new QWidget;
    QHBoxLayout *headerLayout = new QHBoxLayout(w);
    headerLayout->addWidget(localeCombo);
    headerLayout->addWidget(localeName);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(w);
    l->addWidget(tabWidget);

    tabWidget->addTab(calendar, "Calendar");
    tabWidget->addTab(currency, "Currency");
    tabWidget->addTab(languages, "Languages");
    tabWidget->addTab(miscellaneous, "Misc");
    localeCombo->setCurrentIndex(0);
    systemLocaleChanged();
}

void Window::systemLocaleChanged()
{
    QLocale l = QLocale::system();
    QString lang = QLocale::languageToString(l.language());
    QString country = QLocale::countryToString(l.country());
    localeCombo->setItemText(0, QString("System: %1/%2").arg(lang, country));
    emit localeChanged(0);
}

void Window::localeChanged(int idx)
{
    QLocale locale = localeCombo->itemData(idx).toLocale();
    localeName->setText(QString("Locale: %1").arg(locale.name()));
    emit localeChanged(locale);
}

bool Window::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LocaleChange: {
        if (localeCombo->currentIndex() == 0)
            systemLocaleChanged();
        return true;
    }
    default:
        break;
    }
    return QWidget::event(event);
}
