// IMPORTANT!!!! If you want to add testdata to this file,
// always add it to the end in order to not change the linenumbers of translations!!!

#include <QtCore>


//
// Test 'lacks Q_OBJECT' reporting on namespace scopes
//

class B : public QObject {
    //Q_OBJECT
    void foo();
};

void B::foo() {
    tr("Bla", "::B");
}


class C : public QObject {
    //Q_OBJECT
    void foo() {
        tr("Bla", "::C");
    }
};


namespace nsB {

    class B : public QObject {
        //Q_OBJECT
        void foo();
    };

    void B::foo() {
        tr("Bla", "nsB::B");
    }

    class C : public QObject {
        //Q_OBJECT
        void foo() {
            tr("Bla", "nsB::C");
        }
    };
}

