# Reversing Java and JavaScript with Frida

While Frida is heavily used for native code (C/C++), it has incredible built-in support for interacting with high-level language runtimes like Java (Android) and JavaScript (V8/JSC/Electron/Node.js).

## Reversing Java (Android / JVM)

Frida provides the `Java` API object to interact directly with the Dalvik/ART virtual machine on Android, or even standard JVMs.

### `Java.perform()`
Everything involving the Java API must be wrapped in `Java.perform()`. This ensures your script is safely attached to the VM thread.

```javascript
Java.perform(function () {
    console.log("Attached to Java VM!");
});
```

### Finding and Hooking Classes
You don't need to deal with memory addresses. You can use the fully qualified class name.

```javascript
Java.perform(function () {
    // 1. Get a reference to the class
    var MainActivity = Java.use("com.example.myapp.MainActivity");

    // 2. Overload the specific method
    MainActivity.checkPassword.implementation = function (password) {
        console.log("[*] Intercepted password check: " + password);
        
        // 3. Call the original method (if you want)
        var result = this.checkPassword(password); 
        console.log("[*] Original result: " + result);
        
        // 4. Return a modified result
        return true; 
    };
});
```

*Note: If a method is overloaded (multiple methods with the same name but different arguments), you must specify the exact one using `.overload('java.lang.String', 'int')`.*

### Instantiating and Calling
You can create new Java objects and call their methods directly from Frida.

```javascript
Java.perform(function () {
    var StringClass = Java.use("java.lang.String");
    var myString = StringClass.$new("Hello from Frida!");
    console.log("Length is: " + myString.length());
});
```

## Reversing JavaScript (V8 / Node / Electron)

If you are reverse engineering an Electron app or a Node.js binary, you are dealing with a V8 engine. Frida provides the `V8` or `Duktape` modules, but often you will hook the native functions that execute the JS.

### Hooking Native V8 Engine
In Electron apps, the actual Javascript is often compiled or obfuscated. You can hook the V8 compilation functions natively to dump the raw Javascript before it executes.

```javascript
// Example: Hooking v8::Script::Compile in an Electron app
var compilePtr = Module.findExportByName(null, "_ZN2v86Script7CompileENS_5LocalINS_7ContextEEENS1_INS_6StringEEEPNS_12ScriptOriginE");

if (compilePtr) {
    Interceptor.attach(compilePtr, {
        onEnter: function (args) {
            // args[1] is typically the source code string in V8 internals
            // Reading V8 strings natively requires understanding V8 memory layout!
            console.log("Script compilation intercepted!");
        }
    });
}
```

### Injecting into the JS Context
If the app exposes a webview or Node context, you can sometimes use platform-specific tricks (like hooking Android WebView's `evaluateJavascript` or macOS's `WKWebView`) to inject standard JS into the app's frontend dynamically.
