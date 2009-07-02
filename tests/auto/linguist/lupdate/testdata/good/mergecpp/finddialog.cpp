
// The first line in this file should always be empty, its part of the test!!
class FindDialog : public QDialog
{
    Q_OBJECT
public:
    FindDialog(MainWindow *parent);
    void reset();
};

FindDialog::FindDialog(MainWindow *parent)
    : QDialog(parent)
{
    QString trans = tr("Enter the text you want to find.");
    trans = tr("Search reached end of the document");
    trans = tr("Search reached start of the document");
    trans = tr( "Text not found" );
}

void FindDialog::reset()
{
    tr("%n item(s)", "merge from singular to plural form", 4);
    tr("%n item(s)", "merge from a finished singular form to an unfinished plural form", 4);
}

