
#include <QtGui>

int main(int argv, char **args)
{
    QApplication app(argv, args);
    
//![0]
    QList<QUrl> urls;
    urls << QUrl::fromLocalFile("/home/gvatteka/dev/qt-45")
         << QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

    QFileDialog dialog;
    dialog.setSidebarUrls(urls);
    dialog.setFileMode(QFileDialog::AnyFile);
    if(dialog.exec()) {
        // ...
    }
//![0]

    return app.exec();
}
