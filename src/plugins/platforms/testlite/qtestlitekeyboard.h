#ifndef QTESTLITEKEYBOARD_H
#define QTESTLITEKEYBOARD_H

#include "qtestliteintegration.h"

class QTestLiteKeyboard
{
public:
    QTestLiteKeyboard(QTestLiteScreen *screen);

    void changeLayout();

    void handleKeyEvent(QWidget *widget, QEvent::Type type, XKeyEvent *ev);

    Qt::KeyboardModifiers translateModifiers(int s);

    enum { MaxBits = sizeof(uint) * 8 };
private:

    void setMask(KeySym sym, uint mask);
    int translateKeySym(uint key) const;
    QString translateKeySym(KeySym keysym, uint xmodifiers,
                                   int &code, Qt::KeyboardModifiers &modifiers,
                                   QByteArray &chars, int &count);

    QTestLiteScreen *m_screen;

    uint m_alt_mask;
    uint m_super_mask;
    uint m_hyper_mask;
    uint m_meta_mask;
    uint m_mode_switch_mask;
    uint m_num_lock_mask;
};

#endif // QTESTLITEKEYBOARD_H
