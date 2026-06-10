#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// A custom decoding loop designed to be analyzed with Frida Stalker
// It generates the flag in registers and immediately clobbers it
void run_obfuscated_logic() {
    unsigned char key[] = {0x01, 0x05, 0x07, 0x0a, 0x04, 0x3e, 0x1a, 0x16, 0x15, 0x1b, 0x28, 0x0c, 0x16, 0x06, 0x11, 0x1c, 0x1b, 0x16, 0x10, 0x1a, 0x01, 0x05, 0x3d};
    unsigned char base = 0x49; // 'I' ^ 0x01 = 'H'
    
    printf("[-] Generating something...\n");
    
    // We want Stalker to capture the state inside this loop
    for (int i = 0; i < sizeof(key); i++) {
        // The real character is calculated here:
        volatile unsigned char actual_char = base ^ key[i];
        
        // And then we immediately overwrite the variable to make memory scanning harder
        actual_char = 0xFF;
    }
    printf("[-] Done.\n");
}

int main() {
    printf("[*] Welcome to Hard Challenge 3: Instruction Tracing (Stalker)\n");
    printf("[*] My PID is %d\n", getpid());
    printf("[*] Objective: Use Frida.Stalker to trace execution of run_obfuscated_logic()\n");
    printf("[*] and capture the decoded characters before they are destroyed.\n\n");
    
    while(1) {
        printf("[-] Press Enter to run the logic...\n");
        getchar();
        run_obfuscated_logic();
    }
    
    return 0;
}
