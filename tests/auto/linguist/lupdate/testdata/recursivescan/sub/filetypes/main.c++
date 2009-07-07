#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QApplication::translate("text/c++", "test");
    return app.exec();
}
