import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import javax.crypto.spec.IvParameterSpec;
import java.util.Base64;
import java.util.Scanner;
import java.security.MessageDigest;

public class Challenge13_JavaCrypto {
    private static final String ENCRYPTED_FLAG = "kRYLQlIXci87mGtwnzTTEQ1LHNTIDYS0JKMOQdnMBQ0PkH1fGvDa3Ecf2CDyaIsK";

    public static byte[] deriveKeyOrIV(String seed) throws Exception {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        byte[] hash = digest.digest(seed.getBytes("UTF-8"));
        byte[] result = new byte[16];
        System.arraycopy(hash, 0, result, 0, 16);
        return result;
    }

    public static String decrypt(String ciphertextBase64, byte[] key, byte[] iv) throws Exception {
        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        SecretKeySpec keySpec = new SecretKeySpec(key, "AES");
        IvParameterSpec ivSpec = new IvParameterSpec(iv);
        cipher.init(Cipher.DECRYPT_MODE, keySpec, ivSpec);
        byte[] decryptedBytes = cipher.doFinal(Base64.getDecoder().decode(ciphertextBase64));
        return new String(decryptedBytes, "UTF-8");
    }

    public static void main(String[] args) {
        System.out.println("[*] Welcome to Advanced Challenge 4: Java Cryptography Hooking");
        System.out.println("[*] Objective: Hook Cipher.init and Cipher.doFinal to intercept the key, IV, and decrypted flag.");
        
        Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.print("\n[-] Enter password to decrypt the flag: ");
            String input = scanner.nextLine();
            
            if (input.equals("unlock_flag_2026_magic")) {
                try {
                    byte[] key = deriveKeyOrIV("java_crypto_key_seed_2026");
                    byte[] iv = deriveKeyOrIV("java_crypto_iv_seed_2026");
                    String flag = decrypt(ENCRYPTED_FLAG, key, iv);
                    System.out.println("[+] Success! Decrypted flag: " + flag);
                    break;
                } catch (Exception e) {
                    System.out.println("[-] Error decrypting flag: " + e.getMessage());
                }
            } else {
                try {
                    byte[] key = deriveKeyOrIV("java_crypto_key_seed_2026");
                    byte[] iv = deriveKeyOrIV("java_crypto_iv_seed_2026");
                    String decrypted = decrypt(ENCRYPTED_FLAG, key, iv);
                    
                    if (decrypted.startsWith("T3mp1e{")) {
                        System.out.println("[*] Status: Decryption operation executed successfully in memory.");
                        System.out.println("[*] Hint: The decrypted bytes are present in the Cipher stream, but not printed to stdout!");
                    }
                } catch (Exception e) {
                    System.out.println("[-] Decryption processing failed.");
                }
            }
        }
        scanner.close();
    }
}
