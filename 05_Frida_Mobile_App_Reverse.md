# Mobile App Reverse Engineering with Frida

Mobile reverse engineering (Android and iOS) is where Frida truly shines. Because mobile apps run in sandboxed environments and heavily use high-level runtimes (ART for Android, Objective-C/Swift for iOS), dynamic analysis is often much faster and more effective than pure static analysis.

## Setup Requirements

### 1. Rooted / Jailbroken Device (The Easy Way)
For full system-wide access, you need a rooted Android device (or an emulator like Corellium/Genymotion) or a jailbroken iOS device.

### 2. Frida-Server
You must transfer the `frida-server` binary to the device, run it as root, and leave it running in the background.
*   **Android:**
    ```bash
    adb push frida-server /data/local/tmp/
    adb shell "chmod 755 /data/local/tmp/frida-server"
    adb shell "su -c /data/local/tmp/frida-server &"
    ```
*   **iOS:** Install via Cydia/Sileo from the official Frida repository (`build.frida.re`).

Once running, your host machine communicates with it via USB:
`frida-ps -U` (List USB processes)

## Non-Rooted Devices: The Frida Gadget

If you cannot root the device (e.g., testing on a locked enterprise phone or bypassing intense root detection), you can use `frida-gadget`.

The Gadget is a shared library (`frida-gadget.so` or `frida-gadget.dylib`).
1.  **Unpack the App:** Decompile the APK using `apktool` or unzip the IPA.
2.  **Inject the Gadget:** Place the gadget library inside the app's native `lib/` folder.
3.  **Patch the Binary:** Modify the app's code (Smali for Android, Mach-O load commands for iOS) to force the app to load `frida-gadget` as soon as it starts.
4.  **Repack & Sign:** Rebuild the app and sign it with your own developer certificate.
5.  **Run:** When the modified app opens, it will load the Gadget, which starts a mini frida-server *inside* that specific app's sandbox. You can then connect to it using `frida -U Gadget`.

*Tools like `objection patchapk` automate this entire Gadget injection process.*

## Android Reversing Focus Areas

1.  **Bypassing SSL Pinning:** Apps often refuse to communicate if they don't see a specific SSL certificate (blocking Burp Suite/Proxies). Frida can hook the trust managers to always return true.
2.  **Bypassing Root Detection:** Hook functions like `java.io.File.exists()` to hide the presence of `/system/xbin/su` and Magisk.
3.  **Dumping Secrets:** Hook crypto libraries (`javax.crypto.Cipher`) to print AES keys or plaintext data before it is encrypted.

## iOS Reversing Focus Areas (Objective-C)

Frida provides an `ObjC` object specifically tailored for iOS/macOS.

### Hooking ObjC Methods
Objective-C methods are mapped dynamically, making them very easy to hook.

```javascript
if (ObjC.available) {
    // 1. Find the class
    var loginManager = ObjC.classes.LoginManager;

    // 2. Hook an instance method (-) or class method (+)
    var hook = loginManager["- verifyPin:"];
    
    Interceptor.attach(hook.implementation, {
        onEnter: function(args) {
            // args[0] is 'self', args[1] is the selector '_cmd'
            // args[2] is the first actual argument
            var pin = new ObjC.Object(args[2]);
            console.log("PIN entered: " + pin.toString());
        },
        onLeave: function(retval) {
            // Change return value to true (1) to bypass
            retval.replace(ptr("0x1")); 
        }
    });
}
```

## Summary
Whether rooted or non-rooted, Frida provides the ultimate toolkit for mobile dynamic analysis, allowing you to bypass SSL pinning, evade jailbreak detection, and extract sensitive data on the fly.

---

## Advanced Android Dynamic Analysis

When auditing Android applications, many secure validation mechanisms, cryptography engines, and root/debugger checks are compiled into native C/C++ libraries (loaded via `.so` files) to make analysis harder.

### 1. The JNI (Java Native Interface) Bridge
Java classes declare native methods using the `native` keyword. At runtime, Java loads a compiled `.so` file via `System.loadLibrary()`. 

To hook native functions, we must find the address of the native method in memory. 

#### Hooking Exported JNI Functions
For standard JNI exports, the C function has a fully qualified name matching the Java package and class structure (e.g., `Java_com_example_app_MainActivity_verifyKey`). These are exported symbols, meaning we can locate them by name.

```javascript
// Find the base address of the native library
var moduleBase = Module.findBaseAddress("libnative_challenge.so");

if (moduleBase) {
    // Standard JNI exports are listed in the export table
    var jniVerifyAddress = Module.findExportByName("libnative_challenge.so", "Java_Challenge15_1JNIBridge_verifyKey");
    
    if (jniVerifyAddress) {
        Interceptor.attach(jniVerifyAddress, {
            onEnter: function(args) {
                console.log("[*] Intercepted Java_Challenge15_JNIBridge_verifyKey");
                // args[0] = JNIEnv pointer
                // args[1] = jobject (this)
                // args[2] = jstring (argument)
            },
            onLeave: function(retval) {
                console.log("[*] Returning true (1) to bypass key verification.");
                retval.replace(ptr(1)); // Return JNI_TRUE (1)
            }
        });
    }
}
```

#### Hooking Non-Exported Helper Functions
If functions are declared `static` or are registered dynamically via `RegisterNatives`, they will not appear in the export table. You must locate their offset using a static analysis tool (Ghidra, IDA) and hook them via offset:

```javascript
var libBase = Module.findBaseAddress("libnative_challenge.so");
if (libBase) {
    var helperOffset = 0x11B0; // Offset found in Ghidra
    var helperAddress = libBase.add(helperOffset);
    
    Interceptor.attach(helperAddress, {
        onEnter: function(args) {
            console.log("[*] Intercepted internal native helper function");
        }
    });
}
```
*Practice these concepts with **Challenge 15: JNI Bridge Hooking**.*

### 2. Libc-Level Hooking for Anti-Analysis Bypass
Many native root detection and anti-debugging libraries call standard Linux system APIs to gather environment info. Instead of reversing a complex obfuscated library, you can hook the underlying libc calls that the library relies on:

*   **File Access (`access`, `fopen`, `stat`)**: Libraries check if files like `/system/bin/su` or `/system/xbin/su` exist.
*   **Tracer Identification (`/proc/self/status` parsing)**: Anti-debug checks open `/proc/self/status` and read the `TracerPid:` field. If it is not `0`, a debugger is active.

```javascript
// Hooking access() to hide files
var accessPtr = Module.findExportByName("libc.so", "access");
if (accessPtr) {
    Interceptor.attach(accessPtr, {
        onEnter: function(args) {
            this.path = args[0].readUtf8String();
        },
        onLeave: function(retval) {
            if (this.path.indexOf("su") !== -1 || this.path.indexOf("Superuser") !== -1) {
                console.log("[*] Bypassing native file existence check for: " + this.path);
                retval.replace(ptr(-1)); // -1 means file not found/access denied
            }
        }
    });
}

// Hooking fopen() to mock /proc/self/status reading
var fopenPtr = Module.findExportByName("libc.so", "fopen");
if (fopenPtr) {
    Interceptor.attach(fopenPtr, {
        onEnter: function(args) {
            this.path = args[0].readUtf8String();
        },
        onLeave: function(retval) {
            if (this.path === "/proc/self/status") {
                console.log("[*] Detected access to /proc/self/status!");
                // Optionally redirect to a custom, fake status file in /tmp/clean_status
                // args[0].writeUtf8String("/tmp/clean_status");
            }
        }
    });
}
```
*Practice these concepts with **Challenge 16: Libc-Level Anti-Analysis & Root Detection**.*

---

## Advanced iOS Dynamic Analysis

### 1. Swift Name Mangling & Interception
Modern iOS applications are primarily written in Swift. Unlike Objective-C, Swift compiles to native C-like executables, and its functions are not dynamically dispatched by default.

Swift uses **Name Mangling** to encode method names, class structures, argument types, and modules into a single unique ASCII string.

For example, a Swift method `verifyPin(pin: String)` on class `AppSecurity` in module `SecureApp` might mangle to:
`_T09SecureApp0B8SecurityC9verifyPinACSgSS3pin_tF` (or similar depending on Swift compiler version).

To hook Swift methods:
1. Load the Swift symbols.
2. Demangle the symbols using Frida's `Swift` module or external demanglers (e.g. `swift demangle` or `Swift.demangle`).
3. Locate the mangled name and hook it natively using `Interceptor.attach`.

```javascript
// Check if Swift runtime is loaded in the app
if (Swift.available) {
    // Locate the mangled function name in the main module
    var verifyPinMangled = Module.findExportByName(null, "$s9SecureApp0B8SecurityC9verifyPin3pinSbSS_tF");
    
    if (verifyPinMangled) {
        Interceptor.attach(verifyPinMangled, {
            onEnter: function(args) {
                console.log("[*] Intercepted Swift verifyPin method");
                // Swift calling conventions use registers differently than C.
                // Depending on the CPU architecture (usually ARM64), arguments might be passed in X0, X1, etc.
            },
            onLeave: function(retval) {
                console.log("[*] Forcing Swift verifyPin to return true (1)");
                retval.replace(ptr(1));
            }
        });
    }
}
```

### 2. iOS Native Anti-Analysis
iOS apps often use `sysctl` to detect if they are being debugged (checking if the `P_TRACED` flag is set in the `kinfo_proc` struct returned by `sysctl`).

We can bypass this by hooking the `sysctl` system call:

```javascript
var sysctlPtr = Module.findExportByName(null, "sysctl");
if (sysctlPtr) {
    Interceptor.attach(sysctlPtr, {
        onEnter: function(args) {
            this.mib = args[0]; // Management Information Base array
            this.mibLength = args[1].toInt32();
            this.oldp = args[2]; // Buffer to write process info into
        },
        onLeave: function(retval) {
            if (this.mibLength > 0 && this.mib.readInt() === 1) { // CTL_KERN
                var mib2 = this.mib.add(4).readInt();
                if (mib2 === 14) { // KERN_PROC
                    var mib3 = this.mib.add(8).readInt();
                    if (mib3 === 1) { // KERN_PROC_PID
                        // Clear the P_TRACED flag in the process structure
                        // The offset of p_flag in kinfo_proc varies, typically:
                        var p_flag_offset = 32; 
                        var p_flag = this.oldp.add(p_flag_offset).readInt();
                        if ((p_flag & 0x00000800) !== 0) { // P_TRACED flag
                            console.log("[*] Bypassing iOS debug detection (sysctl P_TRACED flag cleared)!");
                            this.oldp.add(p_flag_offset).writeInt(p_flag & ~0x00000800);
                        }
                    }
                }
            }
        }
    });
}
```

