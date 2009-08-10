#include <QtGui>
#include "tabletwidget.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  TabletWidget tabletWidget;
  tabletWidget.showMaximized();
  return app.exec();
} 
