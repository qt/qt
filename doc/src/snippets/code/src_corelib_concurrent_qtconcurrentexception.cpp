//! [0]

class MyException : public QtConcurrent::Exception
{
public:
    void raise() const { throw *this; }
    Exception *clone() const { return new MyException(*this); }
};

//! [0]


//! [1]

try  {
    QtConcurrent::blockingMap(list, throwFunction); // throwFunction throws MyException
} catch (MyException &e) {
    // handle exception
}

//! [1]


//! [2]

void MyException::raise() const { throw *this; }

//! [2]


//! [3]

MyException *MyException::clone() const { return new MyException(*this); }

//! [3]
