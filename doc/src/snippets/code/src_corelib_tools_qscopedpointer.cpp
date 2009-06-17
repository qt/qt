//! [0]
void myFunction(bool useSubClass)
{
    MyClass *p = useSubClass ? new MyClass() : new MySubClass;
    QIODevice *device = handsOverOwnership();

    if (m_value > 3) {
        delete p;
        delete device;
        return;
    }

    try {
        process(device);
    }
    catch (...) {
        delete p;
        delete device;
        throw;
    }

    delete p;
    delete device;
}
//! [0]

//! [1]
void myFunction(bool useSubClass)
{
    QScopedPointer<MyClass> p(useSubClass ? new MyClass() : new MySubClass);
    QScopedPointer<QIODevice> device(handsOverOwnership());

    if (m_value > 3)
        return;

    process(device);
}
//! [1]

//! [2]
    const QWidget *const p = new QWidget();
    // is equivalent to:
    const QScopedPointer<const QWidget> p(new QWidget());

    QWidget *const p = new QWidget();
    // is equivalent to:
    const QScopedPointer<QWidget> p(new QWidget());

    QWidget *const p = new QWidget();
    // is equivalent to:
    const QScopedPointer<QWidget> p(new QWidget());

    const QWidget *p = new QWidget();
    // is equivalent to:
    QScopedPointer<const QWidget> p(new QWidget());

//! [2]

//! [3]
if (scopedPointer) {
    ...
}
//! [3]

//! [4]
class MyPrivateClass; // forward declare MyPrivateClass

class MyClass
{
private:
    QScopedPointer<MyPrivateClass> privatePtr; // QScopedPointer to forward declared class

public:
    MyClass(); // OK
    inline ~MyClass() {} // VIOLATION - Destructor must not be inline

private:
    Q_DISABLE_COPY(MyClass) // OK - copy constructor and assignment operators
                             // are now disabled, so the compiler won't implicitely
                             // generate them.
};
//! [4]
