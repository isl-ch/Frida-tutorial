#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void log_debug(const char *msg) {
    // This function supposedly logs to a file, but in this challenge it does nothing visible.
    // Hook it with Frida to see the argument!
}

void process_input(const char *input) {
    char secret_password[] = "Frid4_R0cks_S3cr3t";
    char log_msg[256];
    
    snprintf(log_msg, sizeof(log_msg), "User tried password: %s, Real password is: %s", input, secret_password);
    log_debug(log_msg);
    
    if (strcmp(input, secret_password) == 0) {
        printf("[+] Access Granted! The flag is HTB{easy_2_arguments_intercepted}\n");
        exit(0);
    } else {
        printf("[-] Access Denied.\n");
    }
}

int main() {
    char input[100];
    printf("[*] Welcome to Easy Challenge 2: Argument interception\n");
    printf("[*] My PID is %d\n", getpid());
    
    while(1) {
        printf("\n[-] Enter password to access the system: ");
        if (scanf("%99s", input) != 1) break;
        process_input(input);
    }
    return 0;
}
