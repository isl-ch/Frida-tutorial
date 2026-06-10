#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int check_pin(int user_pin, int actual_pin) {
    return user_pin == actual_pin;
}

int main() {
    srand(time(NULL));
    int actual_pin = rand() % 9000 + 1000;
    int user_pin = 0;

    printf("[*] Welcome to Easy Challenge 1: The PIN Check\n");
    printf("[*] My PID is %d\n", getpid());
    
    while(1) {
        printf("\n[-] Please enter the secret 4-digit PIN: ");
        if (scanf("%d", &user_pin) != 1) {
            // clear stdin
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            continue;
        }

        if (check_pin(user_pin, actual_pin)) {
            printf("[+] Correct! The flag is HTB{easy_1_hook_return_values}\n");
            break;
        } else {
            printf("[-] Wrong PIN! Try again.\n");
        }
    }

    return 0;
}
