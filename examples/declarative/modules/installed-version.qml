import Qt 4.6

import com.nokia.Foo 1.6
import com.nokia.Foo 1.7 as NewFoo
VerticalPositioner {
    Bar { }
    Baz { }
    NewFoo.Bar { }
}
