#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>

void print_flag() {
    printf("[+] You found it! The flag is HTB{hard_7_ptrace_bypassed_and_hidden_call}\n");
    exit(0);
}

int main() {
    printf("[*] Welcome to Hard Challenge 1: Anti-Debugging & Redirection\n");
    printf("[*] My PID is %d\n", getpid());
    
    // Basic ptrace anti-debug
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        printf("[-] Debugger detected! Exiting...\n");
        return 1;
    }
    
    printf("[+] Passed anti-debug check.\n");
    printf("[-] But the flag is hidden. Good luck finding it!\n");
    
    while(1) {
        sleep(5);
    }
    
    return 0;
}
