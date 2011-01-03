#ifndef QTESTLITECLIPBOARD_H
#define QTESTLITECLIPBOARD_H

#include <QPlatformClipboard>
#include "qtestlitestaticinfo.h"

class QTestLiteScreen;
class QTestLiteClipboard : public QPlatformClipboard
{
public:
    QTestLiteClipboard(QTestLiteScreen *screen);

    const QMimeData *mimeData(QClipboard::Mode mode) const;
    void setMimeData(QMimeData *data, QClipboard::Mode mode);

    bool supportsMode(QClipboard::Mode mode) const;

    QTestLiteScreen *screen() const;

    Window requestor() const;
    void setRequestor(Window window);

    Window owner() const;

    void handleSelectionRequest(XEvent *event);

    bool clipboardReadProperty(Window win, Atom property, bool deleteProperty, QByteArray *buffer, int *size, Atom *type, int *format) const;
    QByteArray clipboardReadIncrementalProperty(Window win, Atom property, int nbytes, bool nullterm);

    QByteArray getDataInFormat(Atom modeAtom, Atom fmtatom);

private:
    void setOwner(Window window);

    Atom sendTargetsSelection(QMimeData *d, Window window, Atom property);
    Atom sendSelection(QMimeData *d, Atom target, Window window, Atom property);

    QTestLiteScreen *m_screen;

    QMimeData *m_xClipboard;
    QMimeData *m_clientClipboard;

    QMimeData *m_xSelection;
    QMimeData *m_clientSelection;

    Window m_requestor;
    Window m_owner;

    static const int clipboard_timeout;

};

#endif // QTESTLITECLIPBOARD_H
