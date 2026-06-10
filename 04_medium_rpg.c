#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int player_hp = 10;
int boss_hp = 9999;

void player_attack(int damage) {
    printf("[-] You attack the boss for %d damage!\n", damage);
    boss_hp -= damage;
    if (boss_hp < 0) boss_hp = 0;
    printf("[-] Boss HP: %d/9999\n", boss_hp);
}

void boss_attack() {
    int damage = 9;
    printf("[-] Boss attacks you for %d damage!\n", damage);
    player_hp -= damage;
    if (player_hp < 0) player_hp = 0;
    printf("[-] Your HP: %d/10\n", player_hp);
}

int main() {
    printf("[*] Welcome to Medium Challenge 1: The Impossible RPG\n");
    printf("[*] My PID is %d\n", getpid());
    printf("[*] Objective: Defeat the boss. Hook player_attack(int damage) and change the argument.\n\n");
    
    while(boss_hp > 0 && player_hp > 0) {
        printf("\n--- New Turn ---\n");
        printf("1. Attack\n");
        printf("2. Heal (does nothing yet)\n");
        printf("Choice: ");
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) { }
            continue;
        }
        
        if (choice == 1) {
            player_attack(1);
        } else {
            printf("[-] You skipped your turn.\n");
        }
        
        if (boss_hp > 0) {
            sleep(1);
            boss_attack();
        }
    }
    
    if (boss_hp == 0) {
        printf("\n[+] You defeated the boss! The flag is HTB{medium_4_argument_modification}\n");
    } else {
        printf("\n[-] You died! Game Over.\n");
    }
    
    return 0;
}
