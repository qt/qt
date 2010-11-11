import QtQuick 1.0

Rectangle {
    id: main
    width: 800; height: 600


    Grid {
        x: 4; y: 4
        spacing: 8
        columns: 4

        Column {
            spacing: 4
            TestText { }
            TestText { horizontalAlignment: Text.AlignHCenter }
            TestText { horizontalAlignment: Text.AlignRight }
        }

        Column {
            spacing: 4
            TestText { wrapMode: Text.Wrap }
            TestText { horizontalAlignment: Text.AlignHCenter; wrapMode: Text.Wrap }
            TestText { horizontalAlignment: Text.AlignRight; wrapMode: Text.Wrap }
        }

        Column {
            spacing: 4
            TestText { wrapMode: Text.Wrap; elide: Text.ElideRight }
            TestText { horizontalAlignment: Text.AlignHCenter; wrapMode: Text.Wrap; elide: Text.ElideRight }
            TestText { horizontalAlignment: Text.AlignRight; wrapMode: Text.Wrap; elide: Text.ElideRight }
        }

        Column {
            spacing: 4
            TestText { width: 230; wrapMode: Text.Wrap; elide: Text.ElideRight }
            TestText { width: 230; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.Wrap; elide: Text.ElideRight }
            TestText { width: 230; horizontalAlignment: Text.AlignRight; wrapMode: Text.Wrap; elide: Text.ElideRight }
        }

        Column {
            spacing: 4
            TestText { width: 120; wrapMode: Text.Wrap; elide: Text.ElideRight }
            TestText { width: 120; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.Wrap; elide: Text.ElideRight }
            TestText { width: 120; horizontalAlignment: Text.AlignRight; wrapMode: Text.Wrap; elide: Text.ElideRight }
        }

        Column {
            spacing: 4
            TestText { width: 120; wrapMode: Text.Wrap }
            TestText { width: 120; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.Wrap }
            TestText { width: 120; horizontalAlignment: Text.AlignRight; wrapMode: Text.Wrap }
        }

        Column {
            spacing: 4
            TestText { width: 120 }
            TestText { width: 120; horizontalAlignment: Text.AlignHCenter }
            TestText { width: 120; horizontalAlignment: Text.AlignRight }
        }
    }
}
