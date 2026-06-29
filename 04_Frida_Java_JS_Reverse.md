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

---

## Advanced Java Hooking Techniques

When dealing with production Android applications, standard method hooking is often insufficient due to obfuscation, dynamic class loading, and multiple method overloads.

### 1. Hooking Java Constructors (`$init`)
To hook a class constructor in Frida, you use the special method name `$init`.

```javascript
Java.perform(function() {
    var TargetClass = Java.use("com.example.app.SecureUser");
    
    // Hooking the constructor:
    TargetClass.$init.implementation = function(username, age) {
        console.log("[*] SecureUser constructor called: " + username + " (" + age + ")");
        
        // Always call the original constructor to avoid crashing the application
        this.$init(username, age);
        
        // We can inspect or modify fields after the object is created
        console.log("[*] Current object instance: " + this.toString());
    };
});
```

If the constructor is overloaded, use `.overload(...)` just like a regular method:
```javascript
TargetClass.$init.overload('java.lang.String').implementation = function(name) {
    console.log("[*] Overloaded constructor called with name: " + name);
    this.$init(name);
};
```

### 2. Handling Dynamic Class Loading & Custom Class Loaders
In many Android applications (especially those using packing, security wrappers, or plugins), classes are loaded dynamically at runtime via custom class loaders. If you use `Java.use("com.example.dynamic.Class")` immediately upon startup, Frida may throw a `ClassNotFoundException`.

To bypass this, you can:
1. Enumerate loaded classes to wait until the class appears.
2. Hook `java.lang.ClassLoader.loadClass` to detect when the class is loaded.
3. Switch class loaders using `Java.classFactory`.

```javascript
Java.perform(function() {
    // Enumerate all loaded classes in the JVM
    Java.enumerateLoadedClasses({
        onMatch: function(className, handle) {
            if (className.indexOf("com.example.dynamic") !== -1) {
                console.log("[+] Found dynamic class: " + className);
            }
        },
        onComplete: function() {
            console.log("[*] Class enumeration complete.");
        }
    });

    // Alternatively, use Java.classFactory to locate the class loader containing your class
    Java.enumerateClassLoaders({
        onMatch: function(loader) {
            try {
                // Attempt to load the class using this specific loader's factory
                var factory = Java.classFactory.get(loader);
                var DynamicClass = factory.use("com.example.dynamic.SecureVerification");
                
                DynamicClass.verify.implementation = function() {
                    console.log("[*] Hooked dynamically loaded verify function!");
                    return true;
                };
                console.log("[+] Successfully hooked dynamic class using loader: " + loader);
            } catch(e) {
                // Class not found in this loader, continue searching
            }
        },
        onComplete: function() {}
    });
});
```

### 3. Hooking Cryptographic APIs (`javax.crypto.Cipher`)
One of the most practical applications of Frida in mobile security testing is hooking standard encryption APIs to extract keys, initialization vectors (IVs), and plaintexts before encryption or after decryption.

```javascript
Java.perform(function() {
    var Cipher = Java.use("javax.crypto.Cipher");
    var SecretKeySpec = Java.use("javax.crypto.spec.SecretKeySpec");
    var IvParameterSpec = Java.use("javax.crypto.spec.IvParameterSpec");

    // Hook the Cipher.init() overload
    Cipher.init.overload('int', 'java.security.Key', 'java.security.spec.AlgorithmParameterSpec').implementation = function(opmode, key, params) {
        console.log("\n[+] --- Cipher.init() Intercepted ---");
        console.log("[*] Operation Mode: " + (opmode === 1 ? "ENCRYPT" : "DECRYPT"));
        
        // Extract Key Material
        try {
            var keyBytes = Java.cast(key, SecretKeySpec).getEncoded();
            console.log("[*] Key (Hex): " + bytesToHex(keyBytes));
        } catch(e) {
            console.log("[-] Failed to cast/read key specs: " + e.message);
        }

        // Extract IV
        try {
            var ivBytes = Java.cast(params, IvParameterSpec).getIV();
            console.log("[*] IV (Hex): " + bytesToHex(ivBytes));
        } catch(e) {
            console.log("[*] No IV found or unable to parse params.");
        }

        // Execute original init()
        this.init(opmode, key, params);
    };

    // Hook Cipher.doFinal() to capture plaintexts or ciphertexts
    Cipher.doFinal.overload('[B').implementation = function(bytes) {
        var result = this.doFinal(bytes);
        console.log("\n[+] --- Cipher.doFinal() Intercepted ---");
        console.log("[*] Input bytes (Hex): " + bytesToHex(bytes));
        console.log("[*] Output bytes (Hex): " + bytesToHex(result));
        
        try {
            // Attempt to print as UTF-8 string if printable
            var str = byteToString(result);
            console.log("[*] Decrypted/Plaintext UTF-8: " + str);
        } catch(e) {}

        return result;
    };
});

// Helper functions for byte array conversions
function bytesToHex(byteArray) {
    if (!byteArray) return "null";
    var hex = [];
    for (var i = 0; i < byteArray.length; i++) {
        var byteVal = byteArray[i] & 0xFF;
        var hexStr = byteVal.toString(16);
        if (hexStr.length === 1) hexStr = '0' + hexStr;
        hex.push(hexStr);
    }
    return hex.join("");
}

function byteToString(byteArray) {
    var buffer = Java.array('byte', byteArray);
    var StringClass = Java.use("java.lang.String");
    return StringClass.$new(buffer, "UTF-8").toString();
}
```
*Practice these concepts with **Challenge 13: Java Cryptography Hooking**.*

---

## Advanced JavaScript & V8 VM Hooking

In modern JS environments (like Node.js, Electron, or browser runtimes), programs frequently run dynamic or obfuscated code using evaluate statements or sandbox runtimes.

### Hooking V8 VM Sandbox Execution
In Node.js, the `vm` module is often used to run scripts dynamically in a sandbox context:
```javascript
const vm = require('vm');
vm.runInNewContext(code, sandbox);
```
Since this uses standard Node/V8 bindings, we can intercept the `vm` module's methods directly to extract cleartext JavaScript code before it is compiled and executed.

```javascript
// Run with: frida -p <PID> -l solve.js
// Hook the runInNewContext export from the VM module
var vm = require('vm'); // If executing from a client-side environment

// In Frida, we can intercept the JavaScript vm module directly:
var ModuleObj = null;
try {
    // Retrieve the active Node.js VM module
    var vm = Java.use("vm"); // Only applicable if java/android bridge includes JS,
} catch(e) {}

// For pure Node.js hooks, we can hook the native V8/Node execution layer.
// When Node.js compiles code, it calls native functions in the Node/V8 runtime.
// For example, node::contextify::ContextifyScript::New is called when vm.runInNewContext is run.
// However, an easier JavaScript-only approach using Frida's Node agent or intercepting require() is:
```

If we are running Frida inside a Node.js process, we can hook JavaScript functions by simply modifying their prototype descriptors or redefining their exports:

```javascript
// Intercepting JavaScript functions from within the JS runtime:
// Since Frida injects a JS runtime, we can hook standard JS APIs in the context of the application:
var vmModule = require('vm');
var originalRun = vmModule.runInNewContext;

vmModule.runInNewContext = function(code, sandbox, options) {
    console.log("\n[+] --- Intercepted vm.runInNewContext ---");
    console.log("[*] Code to execute:\n" + code);
    
    // Call the original execution engine
    return originalRun.apply(this, arguments);
};
```
This enables dumping any decrypted, unpacked, or dynamically generated JavaScript payloads before they run inside the sandbox.
*Practice these concepts with **Challenge 14: Node.js VM Hooking**.*

