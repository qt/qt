#include <QtGui>
#include "ui_keypadnavigation.h"

class KeypadNavigation : public QMainWindow
{
    Q_OBJECT

public:
    KeypadNavigation(QWidget *parent = 0)
        : QMainWindow(parent)
        , ui(new Ui_KeypadNavigation)
    {
        ui->setupUi(this);

        connect(ui->m_actionLayoutVerticalSimple, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutVerticalSimple, ui->m_pageVerticalSimple);
        connect(ui->m_actionLayoutVerticalComplex, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutVerticalComplex, ui->m_pageVerticalComplex);
        connect(ui->m_actionLayoutTwoDimensional, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutTwoDimensional, ui->m_pageTwoDimensional);
        connect(ui->m_actionLayoutSliderMagic, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutSliderMagic, ui->m_pageSliderMagic);
        connect(ui->m_actionLayoutChaos, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutChaos, ui->m_pageChaos);
        connect(&m_layoutSignalMapper, SIGNAL(mapped(QWidget*)), ui->m_stackWidget, SLOT(setCurrentWidget(QWidget*)));

        connect(ui->m_actionModeNone, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeNone, int(Qt::NavigationModeNone));
        connect(ui->m_actionModeKeypadTabOrder, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeKeypadTabOrder, int(Qt::NavigationModeKeypadTabOrder));
        connect(ui->m_actionModeKeypadDirectional, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeKeypadDirectional, int(Qt::NavigationModeKeypadDirectional));
        connect(ui->m_actionModeCursorAuto, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeCursorAuto, int(Qt::NavigationModeCursorAuto));
        connect(ui->m_actionModeCursorForceVisible, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeCursorForceVisible, int(Qt::NavigationModeCursorForceVisible));
        connect(&m_modeSignalMapper, SIGNAL(mapped(int)), SLOT(setNavigationMode(int)));
    }

    ~KeypadNavigation()
    {
        delete ui;
    }

public slots:
    void setNavigationMode(int mode)
    {
        QApplication::setNavigationMode(Qt::NavigationMode(mode));
    }

private:
    Ui_KeypadNavigation *ui;
    QSignalMapper m_layoutSignalMapper;
    QSignalMapper m_modeSignalMapper;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KeypadNavigation w;
#ifdef Q_WS_S60
    w.showMaximized();
#else
    w.show();
#endif
    return a.exec();
}

#include "main.moc"
