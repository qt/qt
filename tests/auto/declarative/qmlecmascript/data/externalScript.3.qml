import Qt 4.6

QtObject {
    property int test: external_script_func();
    property int test2: external_script_func2();
    property bool test3: is_a_undefined();

    Script {
        // Multiple script
        source: [ "externalScript.js", "externalScript.2.js" ]
    }
}

