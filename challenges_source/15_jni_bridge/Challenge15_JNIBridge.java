import java.util.Scanner;

public class Challenge15_JNIBridge {
    static {
        // Loads libnative_challenge.so from the java.library.path
        System.loadLibrary("native_challenge");
    }

    // Native method declaration
    private native boolean verifyKey(String key);

    public void run() {
        System.out.println("[*] Welcome to Advanced Challenge 6: JNI Bridge Hooking");
        System.out.println("[*] Objective: Hook the native JNI method 'verifyKey' or find/hook the native helper functions in the compiled library.");
        
        Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.print("\n[-] Enter secret key: ");
            String key = scanner.nextLine();
            
            if (verifyKey(key)) {
                System.out.println("\n[+] Verification successful! Flag is T3mp1e{jni_native_bridge_hooked}");
                break;
            } else {
                System.out.println("[-] Verification failed. Try again.");
            }
        }
        scanner.close();
    }

    public static void main(String[] args) {
        new Challenge15_JNIBridge().run();
    }
}
