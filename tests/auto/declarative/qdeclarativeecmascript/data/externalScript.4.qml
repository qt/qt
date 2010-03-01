import Qt 4.6

QtObject {
    property int test: external_script_func();
    property bool test2: is_a_undefined();

    // Disconnected scripts
    Script {
        source: "externalScript.js"
    }

    Script {
        source: "externalScript.2.js"
    }
}
