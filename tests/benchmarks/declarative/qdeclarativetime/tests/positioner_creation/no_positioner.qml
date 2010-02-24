import Qt 4.6
import QDeclarativeTime 1.0 as QDeclarativeTime

Item {
    QDeclarativeTime.Timer {
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
