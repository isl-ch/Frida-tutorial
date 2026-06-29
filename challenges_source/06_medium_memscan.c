#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void decrypt_flag(char *dest) {
    // Encrypted flag: Each char is XORed with 0x42
    unsigned char enc[] = {0x0a ,0x16 ,0x00 ,0x39 ,0x3b ,0x72 ,0x37 ,0x1d ,0x31 ,0x21 ,0x23 ,0x0c ,0x0c ,0x71 ,0x26 ,0x1d ,0x75 ,0x2a ,0x71 ,0x1d ,0x2f ,0x27 ,0x2f ,0x2d ,0x30 ,0x3b, 0x3f };
    for (int i = 0; i < sizeof(enc); i++) {
        dest[i] = enc[i] ^ 0x42;
    }
    dest[sizeof(enc)] = '\0';
}

int main() {
    printf("[*] Welcome to Medium Challenge 3: Memory Scanning\n");
    printf("[*] My PID is %d\n", getpid());
    printf("[*] Objective: The program decrypts the flag into a heap buffer, but never prints it.\n");
    printf("[*] Use Frida's Memory.scanSync to find the flag in memory (Hint: starts with HTB{).\n\n");
    
    char *secret_buffer = malloc(64);
    decrypt_flag(secret_buffer);
    
    printf("[-] Flag decrypted into memory. Going to sleep forever...\n");
    
    while(1) {
        sleep(10);
        // keep buffer alive so optimizer doesn't remove it
        if (secret_buffer[0] == 'X') {
            printf("Impossible\n");
        }
    }
    
    free(secret_buffer);
    return 0;
}
