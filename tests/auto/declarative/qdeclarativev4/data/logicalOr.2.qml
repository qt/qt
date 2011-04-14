import Qt.v4 1.0

Result {
    property string s: "foo" || "bar"
    result: s == "foo"
}
