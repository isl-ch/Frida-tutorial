#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void win() {
    printf("\n[+] How did you get here?! The flag is HTB{easy_3_calling_hidden_functions}\n");
    exit(0);
}

int main() {
    printf("[*] Welcome to Easy Challenge 3: Call hidden functions\n");
    printf("[*] My PID is %d\n", getpid());
    printf("[-] I am an unbreakable program. I will never print the flag.\n");
    
    while(1) {
        printf("[-] Zzz...\n");
        sleep(5);
    }
    
    return 0;
}
