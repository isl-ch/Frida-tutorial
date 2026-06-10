#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct User {
    char name[32];
    int is_admin;
};

void check_user(struct User *u) {
    printf("[-] Checking user: %s (Admin flag: %d)\n", u->name, u->is_admin);
    if (u->is_admin == 1) {
        printf("[+] Welcome Admin! The flag is HTB{medium_5_memory_struct_modification}\n");
        exit(0);
    } else {
        printf("[-] Access Denied. Regular users cannot see the flag.\n");
    }
}

int main() {
    struct User *current_user = malloc(sizeof(struct User));
    strcpy(current_user->name, "guest");
    current_user->is_admin = 0;

    printf("[*] Welcome to Medium Challenge 2: Struct Modification\n");
    printf("[*] My PID is %d\n", getpid());
    printf("[*] Objective: Hook check_user(struct User *u), read the pointer, and change is_admin to 1.\n\n");
    
    while(1) {
        printf("[-] Press Enter to attempt login as '%s'...\n", current_user->name);
        getchar();
        check_user(current_user);
    }
    
    free(current_user);
    return 0;
}
