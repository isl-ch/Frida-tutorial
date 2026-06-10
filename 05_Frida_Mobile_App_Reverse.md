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
