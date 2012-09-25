/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "window.h"

//! [constructor]
Window::Window()
{
    setupUi();
    updateWidgets();

    connect(Phonon::BackendCapabilities::notifier(),
            SIGNAL(capabilitiesChanged()), this, SLOT(updateWidgets()));
    connect(Phonon::BackendCapabilities::notifier(),
            SIGNAL(availableAudioOutputDevicesChanged()), SLOT(updateWidgets()));
}
//! [constructor]

//! [outputDevices]
void Window::updateWidgets()
{
    devicesListView->setModel(new QStandardItemModel());
    Phonon::ObjectDescriptionModel<Phonon::AudioOutputDeviceType> *model =
            new Phonon::ObjectDescriptionModel<Phonon::AudioOutputDeviceType>();
    model->setModelData(Phonon::BackendCapabilities::availableAudioOutputDevices());
    devicesListView->setModel(model);
//! [outputDevices]

//! [mimeTypes]
    mimeListWidget->clear();
    QStringList mimeTypes =
            Phonon::BackendCapabilities::availableMimeTypes();
    foreach (QString mimeType, mimeTypes) {
        QListWidgetItem *item = new QListWidgetItem(mimeListWidget);
        item->setText(mimeType);
    }
//! [mimeTypes]

//! [effects]
    effectsTreeWidget->clear();
    QList<Phonon::EffectDescription> effects =
        Phonon::BackendCapabilities::availableAudioEffects();
    foreach (Phonon::EffectDescription effect, effects) {
        QTreeWidgetItem *item = new QTreeWidgetItem(effectsTreeWidget);
        item->setText(0, tr("Effect"));
        item->setText(1, effect.name());
        item->setText(2, effect.description());
//! [effects]

//! [effectsParameters]
        Phonon::Effect *instance = new Phonon::Effect(effect, this);
        QList<Phonon::EffectParameter> parameters = instance->parameters();
        for (int i = 0; i < parameters.size(); ++i) {
                Phonon::EffectParameter parameter = parameters.at(i);

            QVariant defaultValue = parameter.defaultValue();
            QVariant minimumValue = parameter.minimumValue();
            QVariant maximumValue = parameter.maximumValue();

            QString valueString = QString("%1 / %2 / %3")
                    .arg(defaultValue.toString()).arg(minimumValue.toString())
                    .arg(maximumValue.toString()); 

            QTreeWidgetItem *parameterItem = new QTreeWidgetItem(item);
            parameterItem->setText(0, tr("Parameter"));
            parameterItem->setText(1, parameter.name());
            parameterItem->setText(2, parameter.description());
            parameterItem->setText(3, QVariant::typeToName(parameter.type()));
            parameterItem->setText(4, valueString);
        }
    }
//! [effectsParameters]
    for (int i = 0; i < effectsTreeWidget->columnCount(); ++i) {
        if (i == 0)
            effectsTreeWidget->setColumnWidth(0, 150);
        else if (i == 2)
            effectsTreeWidget->setColumnWidth(2, 350);
        else
            effectsTreeWidget->resizeColumnToContents(i);
    }
}

void Window::setupUi()
{

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_SIMULATOR)
    devicesListView = new QListView;
    mimeListWidget = new QListWidget;

    QStringList headerLabels;
    headerLabels << tr("Type") << tr("Name") << tr("Description") <<
                    tr("Value Type") << tr("Default/Min/Max Values");

    effectsTreeWidget = new QTreeWidget;
    effectsTreeWidget->setHeaderLabels(headerLabels);
    effectsTreeWidget->setColumnCount(5);

    QTabWidget *tabWidget = new QTabWidget;

    QWidget *widgetDevices = new QWidget;
    QVBoxLayout *devicesLayout = new QVBoxLayout;
    devicesLayout->addWidget(devicesListView);
    widgetDevices->setLayout(devicesLayout);

    QWidget *widgetMimes = new QWidget;
    QVBoxLayout *mimesLayout = new QVBoxLayout;
    mimesLayout->addWidget(mimeListWidget);
    widgetMimes->setLayout(mimesLayout);

    QWidget *widgetEffects = new QWidget;
    QVBoxLayout *effectsLayout = new QVBoxLayout;
    effectsLayout->addWidget(effectsTreeWidget);
    widgetEffects->setLayout(effectsLayout);

    tabWidget->addTab(widgetDevices, tr("Audio Devices"));
    tabWidget->addTab(widgetMimes, tr("MIME Types"));
    tabWidget->addTab(widgetEffects, tr("Audio Effects"));

    QLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
#else
    devicesLabel = new QLabel(tr("Available Audio Devices:"));
    devicesListView = new QListView;

    mimeTypesLabel = new QLabel(tr("Supported MIME Types:"));
    mimeListWidget = new QListWidget;

    effectsLabel = new QLabel(tr("Available Audio Effects:"));

    QStringList headerLabels;
    headerLabels << tr("Type") << tr("Name") << tr("Description") <<
                    tr("Value Type") << tr("Default/Min/Max Values");

    effectsTreeWidget = new QTreeWidget;
    effectsTreeWidget->setHeaderLabels(headerLabels);
    effectsTreeWidget->setColumnCount(5);

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(devicesLabel, 0, 0);
    layout->addWidget(devicesListView, 1, 0);
    layout->addWidget(mimeTypesLabel, 0, 1);
    layout->addWidget(mimeListWidget, 1, 1);
    layout->addWidget(effectsLabel, 2, 0);
    layout->addWidget(effectsTreeWidget, 3, 0, 2, 2);
    layout->setRowStretch(3, 100);

    backendBox = new QGroupBox(tr("Backend Capabilities"));
    backendBox->setLayout(layout);

    QLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(backendBox);
#endif

    setLayout(mainLayout);
    setWindowTitle(tr("Backend Capabilities Example"));

}
