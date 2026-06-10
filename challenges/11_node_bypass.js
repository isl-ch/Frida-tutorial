const readline = require('readline');

console.log("[*] Welcome to Advanced Challenge 2: Node.js Hooking");
console.log("[*] My PID is", process.pid);
console.log("[*] Objective: Hook the checkPassword function using Frida.\n");

function checkPassword(input) {
    const secret = "node_js_super_secret_123";
    if (input === secret) {
        return true;
    }
    return false;
}

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

function ask() {
    rl.question("[-] Enter password: ", (answer) => {
        if (checkPassword(answer)) {
            console.log("\n[+] Access Granted! The flag is HTB{node_js_v8_hooking_master}");
            process.exit(0);
        } else {
            console.log("[-] Access Denied.\n");
            ask();
        }
    });
}

ask();
