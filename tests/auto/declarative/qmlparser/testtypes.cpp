#include "testtypes.h"

QML_DEFINE_INTERFACE(MyInterface);
QML_DEFINE_TYPE(Test,1,0,0,MyQmlObject,MyQmlObject);
QML_DEFINE_TYPE(Test,1,0,0,MyTypeObject,MyTypeObject);
QML_DEFINE_TYPE(Test,1,0,0,MyContainer,MyContainer);
QML_DEFINE_TYPE(Test,1,0,0,MyPropertyValueSource,MyPropertyValueSource);
QML_DEFINE_TYPE(Test,1,0,0,MyDotPropertyObject,MyDotPropertyObject);

