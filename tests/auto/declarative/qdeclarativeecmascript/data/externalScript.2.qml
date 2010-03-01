import Qt 4.6

QtObject {
    property int test: external_script_func();

    Script {
        // Single source as array 
        source: [ "externalScript.js" ]
    }
}

