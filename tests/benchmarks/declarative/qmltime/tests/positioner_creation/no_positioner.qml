import Qt 4.7
import QmlTime 1.0 as QmlTime

Item {
    QmlTime.Timer {
        component: Component {
            Item {
                Rectangle { }
                Rectangle { }
                Item {
                    Image { }
                    Text { }
                }

                Item {
                    Item {
                        Image { }
                        Image { }
                        Item {
                            Image { }
                            Image { }
                        }
                    }

                    Item {
                        Item {
                            Text { }
                            Text { }
                        }
                        Text { }
                    }
                }
                MouseArea { }
            }
        }
    }
}
