package com.example.rootdetect;

import android.os.Build;
import java.io.File;

public class MainActivity {
    // This is just a simulated snippet for educational purposes
    
    public boolean isDeviceRooted() {
        return checkSuExists() || checkTestKeys();
    }

    private boolean checkSuExists() {
        String[] paths = {
            "/system/app/Superuser.apk",
            "/system/xbin/su",
            "/system/bin/su",
            "/sbin/su",
            "/data/local/xbin/su"
        };
        for (String path : paths) {
            if (new File(path).exists()) {
                return true;
            }
        }
        return false;
    }

    private boolean checkTestKeys() {
        String buildTags = Build.TAGS;
        return buildTags != null && buildTags.contains("test-keys");
    }

    public void run() {
        System.out.println("[*] Welcome to Advanced Challenge 3: Android Root Detection");
        System.out.println("[*] Objective: Study the source code and write a Frida script to bypass the checks.");
        
        if (isDeviceRooted()) {
            System.out.println("[-] Error: Rooted device detected! Shutting down...");
        } else {
            System.out.println("[+] Environment secure. The flag is HTB{android_root_detection_bypassed}");
        }
    }

    public static void main(String[] args) {
        new MainActivity().run();
    }
}
