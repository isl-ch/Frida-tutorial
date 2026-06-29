#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Flag obfuscated with XOR 42: "T3mp1e{libc_root_and_debug_bypass}"
static const unsigned char obfuscated_flag[] = {
    126, 25, 71, 90, 27, 79, 81, 70, 67, 72, 73, 117, 88, 69, 69, 94, 117, 75, 68, 78, 117, 78, 79, 72, 95, 77, 117, 72, 83, 90, 75, 89, 89, 87, 0
};

int is_rooted() {
    // Check for standard su location (simulated on Linux using /usr/bin/su)
    if (access("/usr/bin/su", F_OK) == 0) {
        return 1;
    }
    // Also check /system/bin/su to model Android
    if (access("/system/bin/su", F_OK) == 0) {
        return 1;
    }
    return 0;
}

int is_traced() {
    FILE *fp = fopen("/proc/self/status", "r");
    if (!fp) {
        return 0;
    }
    char line[128];
    int tracer_pid = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "TracerPid:", 10) == 0) {
            tracer_pid = atoi(&line[10]);
            break;
        }
    }
    fclose(fp);
    return tracer_pid != 0;
}

void print_flag() {
    char flag[35];
    for (int i = 0; i < 34; i++) {
        flag[i] = obfuscated_flag[i] ^ 42;
    }
    flag[34] = '\0';
    printf("\n[+] Verification passed! Flag: %s\n", flag);
}

int main() {
    printf("[*] Welcome to Advanced Challenge 7: Libc Anti-Debugging and Root Detection\n");
    printf("[*] Objective: Bypass root detection (access checks) and TracerPid check (file reading).\n\n");

    // Wait a bit to allow Frida to attach
    printf("[*] Performing security checks in 1 second...\n");
    sleep(1);

    if (is_rooted()) {
        printf("[-] Security Error: Root detected! (su binary exists)\n");
        return 1;
    }

    if (is_traced()) {
        printf("[-] Security Error: Tracer detected! (TracerPid is non-zero)\n");
        return 1;
    }

    print_flag();
    return 0;
}
