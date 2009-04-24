//! [0]
    QScriptEngine engine;
    QScriptEngineDebugger debugger;
    debugger.attachTo(&engine);
//! [0]

//! [1]
    engine.evaluate("debugger");
//! [1]
