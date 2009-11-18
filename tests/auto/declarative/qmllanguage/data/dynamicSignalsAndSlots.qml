import Qt 4.6
Object {
    signal signal1
    function slot1() {}
    signal signal2
    function slot2() {}

    property int test: 0
    function slot3(a) { print(1921); test = a; }
}
