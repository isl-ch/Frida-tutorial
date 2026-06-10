import java.util.Scanner;

class LicenseManager {
    public boolean isValid(String key) {
        // Very complex and secure validation algorithm
        return key.equals("FRIDA-SUPER-SECRET-LICENSE-KEY");
    }
}

public class Challenge10_JavaLicense {
    public static void main(String[] args) {
        System.out.println("[*] Welcome to Advanced Challenge 1: Java/JVM Hooking");
        System.out.println("[*] Objective: Use Frida to hook LicenseManager.isValid() to always return true.\n");
        
        Scanner scanner = new Scanner(System.in);
        LicenseManager manager = new LicenseManager();
        
        while (true) {
            System.out.print("[-] Enter your license key: ");
            String key = scanner.nextLine();
            
            if (manager.isValid(key)) {
                System.out.println("\n[+] License Validated! The flag is HTB{java_jvm_hooking_master}");
                break;
            } else {
                System.out.println("[-] Invalid license key. Please try again.\n");
            }
        }
        scanner.close();
    }
}
