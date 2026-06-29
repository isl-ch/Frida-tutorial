# Solutions for Frida CTF Challenges

Here are the solution scripts for the 9 challenges if you get stuck.

## Easy 1: `solve_01.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "check_pin"), {
    onLeave: function(retval) {
        retval.replace(1);
        console.log("[+] Replaced return value with 1!");
    }
});
```

## Easy 2: `solve_02.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "log_debug"), {
    onEnter: function(args) {
        var msg = args[0].readUtf8String();
        console.log("[+] Intercepted log_debug msg: " + msg);
    }
});
```

## Easy 3: `solve_03.js`
```javascript
var win_addr = Module.getExportByName(null, "win");
var win_func = new NativeFunction(win_addr, 'void', []);
win_func();
```

## Medium 1: `solve_04.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "player_attack"), {
    onEnter: function(args) {
        args[0] = ptr(9999);
        console.log("[+] Modified damage to 9999!");
    }
});
```

## Medium 2: `solve_05.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "check_user"), {
    onEnter: function(args) {
        var user_ptr = args[0];
        var is_admin_ptr = user_ptr.add(32); // offset
        is_admin_ptr.writeInt(1);
    }
});
```

## Medium 3: `solve_06.js`
```javascript
var ranges = Process.enumerateRanges({protection: 'rw-', coalesce: true});
ranges.forEach(function(range) {
    try {
        var matches = Memory.scanSync(range.base, range.size, "48 54 42 7B"); // HTB{
        matches.forEach(function(match) {
            console.log("[+] Found flag at: " + match.address);
            console.log("[+] Value: " + match.address.readUtf8String());
        });
    } catch(e) {}
});
```

## Hard 1: `solve_07.js`
```javascript
// Run with: frida -f ./07_hard_antidebug -l solve.js --no-pause
Interceptor.attach(Module.getExportByName(null, "ptrace"), {
    onLeave: function(retval) {
        if (retval.toInt32() === -1) {
            retval.replace(0);
        }
    }
});

setTimeout(function() {
    var print_flag = new NativeFunction(Module.getExportByName(null, "print_flag"), 'void', []);
    print_flag();
}, 500);
```

## Hard 2: `solve_08.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "check_integrity"), {
    onLeave: function(retval) {
        retval.replace(1); // Force it to return true
    }
});

Interceptor.attach(Module.getExportByName(null, "secure_function"), {
    onEnter: function() {
        console.log("[+] successfully hooked!");
    }
});
```

## Hard 3: `solve_09.js`
```javascript
// A simple bypass is hooking the XOR instruction directly using base + offset
var baseAddr = Module.getBaseAddress('09_hard_stalker');

// Note: 0x1247 is a placeholder. You must find the real offset using objdump -d 09_hard_stalker
// It is the instruction immediately after the XOR.
var instruction_offset = 0x1247; 

Interceptor.attach(baseAddr.add(instruction_offset), {
    onEnter: function(args) {
        // Read the AL/EAX register which contains the computed byte
        // console.log("Char:", this.context.rax);
    }
});
```

## Advanced 1 (Java): `solve_10.js`
```javascript
Java.perform(function() {
    var LicenseManager = Java.use("LicenseManager");
    LicenseManager.isValid.implementation = function(key) {
        console.log("[*] Intercepted key check: " + key);
        return true;
    };
});
```

## Advanced 2 (Node.js): `solve_11.js`
```javascript
// Run with: frida -p <PID> -l solve_11.js
// Node.js does string comparisons natively. We can hook memcmp to bypass the check.
Interceptor.attach(Module.getExportByName(null, "memcmp"), {
    onEnter: function(args) {
        try {
            var str = args[0].readUtf8String(24);
            if (str && str.indexOf("node_js_super_secret") !== -1) {
                console.log("[+] Intercepted memcmp with secret!");
                this.found = true;
            }
        } catch(e) {}
    },
    onLeave: function(retval) {
        if (this.found) {
            retval.replace(0); // 0 means memory matches
        }
    }
});
```

## Advanced 3 (Android Root Detect): `solve_12.js`
```javascript
Java.perform(function() {
    // 1. Bypass File.exists() for su binaries
    var File = Java.use("java.io.File");
    File.exists.implementation = function() {
        var path = this.getAbsolutePath();
        if (path.indexOf("su") !== -1 || path.indexOf("Superuser") !== -1) {
            console.log("[*] Bypassing root check for: " + path);
            return false;
        }
        return this.exists(); 
    };

    // 2. Bypass android.os.Build.TAGS
    var Build = Java.use("android.os.Build");
    try {
        var tagsField = Build.class.getDeclaredField("TAGS");
        tagsField.setAccessible(true);
        tagsField.set(null, "release-keys");
        console.log("[*] Spoofed Build.TAGS to release-keys");
    } catch(e) {}
});
```

## Advanced 4 (Java Cryptography Hooking): `solve_13.js`

To extract the flag, hook `javax.crypto.Cipher.init` to capture key/IV material and hook `Cipher.doFinal` to capture the decrypted plaintext flag before it is checked.

```javascript
Java.perform(function() {
    var Cipher = Java.use("javax.crypto.Cipher");
    var SecretKeySpec = Java.use("javax.crypto.spec.SecretKeySpec");
    var IvParameterSpec = Java.use("javax.crypto.spec.IvParameterSpec");

    // Hook the Cipher.init() overload
    Cipher.init.overload('int', 'java.security.Key', 'java.security.spec.AlgorithmParameterSpec').implementation = function(opmode, key, params) {
        console.log("\n[+] --- Cipher.init() Intercepted ---");
        console.log("[*] Operation Mode: " + (opmode === 1 ? "ENCRYPT" : "DECRYPT"));
        
        try {
            var keyBytes = Java.cast(key, SecretKeySpec).getEncoded();
            console.log("[*] Key (Hex): " + bytesToHex(keyBytes));
        } catch(e) {}

        try {
            var ivBytes = Java.cast(params, IvParameterSpec).getIV();
            console.log("[*] IV (Hex): " + bytesToHex(ivBytes));
        } catch(e) {}

        this.init(opmode, key, params);
    };

    // Hook Cipher.doFinal() to capture plaintext
    Cipher.doFinal.overload('[B').implementation = function(bytes) {
        var result = this.doFinal(bytes);
        console.log("\n[+] --- Cipher.doFinal() Intercepted ---");
        
        try {
            var StringClass = Java.use("java.lang.String");
            var decrypted = StringClass.$new(result, "UTF-8");
            console.log("[+] Decrypted Flag: " + decrypted);
        } catch(e) {}

        return result;
    };
});

function bytesToHex(byteArray) {
    var hex = [];
    for (var i = 0; i < byteArray.length; i++) {
        var byteVal = byteArray[i] & 0xFF;
        var hexStr = byteVal.toString(16);
        if (hexStr.length === 1) hexStr = '0' + hexStr;
        hex.push(hexStr);
    }
    return hex.join("");
}
```
*Run the target:* `java -cp challenges Challenge13_JavaCrypto`  
*Run the hook:* `frida -l solve_13.js -f /usr/bin/java -- -cp challenges Challenge13_JavaCrypto` (and enter any input to trigger decryption).

---

## Advanced 5 (Node.js VM Hooking): `solve_14.js`

We can intercept `vm.runInNewContext` directly by accessing Node's global object module cache.

```javascript
// Access target's Node require context
var require = global.require || process.mainModule.require;
var vm = require('vm');
var originalRun = vm.runInNewContext;

vm.runInNewContext = function(code, sandbox, options) {
    console.log("\n[+] --- Intercepted vm.runInNewContext ---");
    console.log("[*] Deobfuscated code:");
    console.log(code);
    return originalRun.apply(this, arguments);
};
```
*Run with:* `frida -p <PID> -l solve_14.js` or `frida --no-pause -l solve_14.js -f /usr/bin/node -- challenges/14_node_obfuscated.js`

---

## Advanced 6 (JNI Bridge Hooking): `solve_15.js`

We can solve JNI challenges using two different levels of abstraction: Java-level JNI method hooking or Native library hooking.

### Option A: Java-Level Bypass
```javascript
Java.perform(function() {
    var Challenge = Java.use("Challenge15_JNIBridge");
    Challenge.verifyKey.implementation = function(key) {
        console.log("[*] Intercepted Java-level verifyKey: " + key);
        console.log("[+] Returning true to bypass check!");
        return true;
    };
});
```

### Option B: Native-Level Bypass
```javascript
var verifyAddr = Module.findExportByName("libnative_challenge.so", "Java_Challenge15_1JNIBridge_verifyKey");
if (verifyAddr) {
    Interceptor.attach(verifyAddr, {
        onLeave: function(retval) {
            console.log("[+] Hooked JNI Export in libnative_challenge.so!");
            retval.replace(ptr(1)); // Force return JNI_TRUE (1)
        }
    });
}
```
*Run target:* `java -Djava.library.path=./challenges -cp ./challenges Challenge15_JNIBridge`  
*Run hook:* `frida -l solve_15.js -f /usr/bin/java -- -Djava.library.path=./challenges -cp ./challenges Challenge15_JNIBridge`

---

## Advanced 7 (Libc Anti-Debugging and Root Detection): `solve_16.js`

To bypass the libc checks:
1. Hook `access` and return `-1` if it checks for `su` binaries.
2. Hook `fgets` and intercept lines containing `TracerPid:`, modifying them to read `TracerPid:\t0\n`.

```javascript
// 1. Bypass access() checks for su
var accessPtr = Module.findExportByName("libc.so", "access");
if (accessPtr) {
    Interceptor.attach(accessPtr, {
        onEnter: function(args) {
            this.path = args[0].readUtf8String();
        },
        onLeave: function(retval) {
            if (this.path.indexOf("su") !== -1) {
                console.log("[*] Bypassing access() check for su path: " + this.path);
                retval.replace(ptr(-1)); // File not found
            }
        }
    });
}

// 2. Bypass TracerPid check in /proc/self/status
var fgetsPtr = Module.findExportByName("libc.so", "fgets");
if (fgetsPtr) {
    Interceptor.attach(fgetsPtr, {
        onEnter: function(args) {
            this.buf = args[0];
        },
        onLeave: function(retval) {
            if (!retval.isNull()) {
                var line = this.buf.readUtf8String();
                if (line.indexOf("TracerPid:") !== -1) {
                    console.log("[*] Intercepted TracerPid status line: " + line.trim());
                    this.buf.writeUtf8String("TracerPid:\t0\n");
                    console.log("[+] Spoofed TracerPid to 0");
                }
            }
        }
    });
}
```
*Run target and hook:* `frida -l solve_16.js -f ./challenges/16_advanced_root_detect --no-pause`

