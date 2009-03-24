#include "randomlistmodel.h"
#include <QListView>
#include <QApplication>

int main(int c, char **v)
{
    QApplication a(c, v);

    QListView view;
    view.setUniformItemSizes(true);
    view.setModel(new RandomListModel(&view));
    view.show();

    return a.exec();
}
