
#include <QtGui>

class Notepad : public QMainWindow
{
    Q_OBJECT

public:
    Notepad();

private slots:
    void newDocument();
    void load();
    void save();

    void documentChanged(int index);

private:
    QTextEdit *textEdit;

    QAction *newDocumentAction;
    QAction *loadAction;
    QAction *saveAction;
    QAction *exitAction;

    QMenu *fileMenu;

    QDockWidget *dockWidget;
    QListWidget *listWidget;

    QStringList documents;
    int documentCount;
};

Notepad::Notepad()
{

    newDocumentAction = new QAction(tr("&New"), this);
    loadAction = new QAction(tr("&Load"), this);
    saveAction = new QAction(tr("&Save"), this);
    exitAction = new QAction(tr("E&xit"), this);

    connect(newDocumentAction, SIGNAL(triggered()), this, SLOT(newDocument()));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newDocumentAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    textEdit = new QTextEdit;
    setCentralWidget(textEdit);

    listWidget = new QListWidget;

    connect(listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(documentChanged(int)));

    dockWidget = new QDockWidget;
    dockWidget->setWidget(listWidget);

    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    documentCount = 0;
    newDocument();

    setWindowTitle(tr("Notepad"));
}

void Notepad::documentChanged(int index)
{

}

void Notepad::newDocument()
{
    listWidget->addItem(tr("Document %1").arg(documentCount++));
    documents.append("");
}

void Notepad::load()
{

}

void Notepad::save()
{

}

int main(int argv, char **args)
{
    QApplication app(argv, args);

    Notepad notepad;
    notepad.show();

    return app.exec();
};

#include "main.moc"

