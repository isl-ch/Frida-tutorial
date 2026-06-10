#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void secure_function() {
    printf("[+] Secure function executed. The flag is HTB{hard_8_integrity_check_bypassed}\n");
    exit(0);
}

int check_integrity(void *func_ptr) {
    unsigned char *ptr = (unsigned char *)func_ptr;
    // Frida inline hooks often start with an unconditional jump (0xE9) on x86/x64
    // or INT3 (0xCC) for breakpoints.
    if (ptr[0] == 0xE9 || ptr[0] == 0xCC) {
        return 0; // Modified
    }
    return 1; // Intact
}

int main() {
    printf("[*] Welcome to Hard Challenge 2: Hook Detection\n");
    printf("[*] My PID is %d\n", getpid());
    
    printf("[-] This program checks if secure_function() is hooked before calling it.\n");
    printf("[-] Objective: Hook secure_function() or bypass the check_integrity() function!\n\n");
    
    while(1) {
        printf("[-] Press Enter to execute secure_function()...\n");
        getchar();
        
        if (check_integrity(&secure_function)) {
            printf("[+] Integrity check passed. Calling function...\n");
            secure_function();
        } else {
            printf("[-] WARNING: Code modification detected (hook/breakpoint). Execution aborted!\n");
        }
    }
    
    return 0;
}
