import Qt 4.6

import com.nokia.Foo 1.6
import com.nokia.Foo 1.7 as NewFoo
VerticalLayout {
    Bar { }
    Baz { }
    NewFoo.Bar { }
}
