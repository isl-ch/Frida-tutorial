const readline = require('readline');
const vm = require('vm');

console.log("[*] Welcome to Advanced Challenge 5: Node.js VM Hooking");
console.log("[*] My PID is", process.pid);
console.log("[*] Objective: Intercept the dynamically executed code inside the V8 VM sandbox.\n");

// Base64 encoded JavaScript code that contains the validation logic.
// Original code:
// function verify(input) {
//     const key = [84, 51, 109, 112, 49, 101, 123, 118, 56, 95, 115, 97, 110, 100, 98, 111, 120, 95, 101, 120, 101, 99, 117, 116, 105, 111, 110, 125]; // T3mp1e{v8_sandbox_execution}
//     let flag = key.map(c => String.fromCharCode(c)).join('');
//     if (input === "vm_secret_passphrase_2026") {
//         return { success: true, flag: flag };
//     }
//     return { success: false, flag: null };
// }
const obfuscatedCodeBase64 = "ZnVuY3Rpb24gdmVyaWZ5KGlucHV0KSB7CiAgICBjb25zdCBrZXkgPSBbODQsIDUxLCAxMDksIDExMiwgNDksIDEwMSwgMTIzLCAxMTgsIDU2LCA5NSwgMTE1LCA5NywgMTEwLCAxMDAsIDk4LCAxMTEsIDEyMCwgOTUsIDEwMSwgMTIwLCAxMDEsIDk5LCAxMTcsIDExNiwgMTA1LCAxMTEsIDExMCwgMTI1XTsgLy8gVDNtcDFle3Y4X3NhbmRib3hfZXhlY3V0aW9ufQogICAgbGV0IGZsYWcgPSBrZXkubWFwKGMgPT4gU3RyaW5nLmZyb21DaGFyQ29kZShjKSkuam9pbignJyk7CiAgICBpZiAoaW5wdXQgPT09ICJ2bV9zZWNyZXRfcGFzc3BocmFzZV8yMDI2IikgewogICAgICAgIHJldHVybiB7IHN1Y2Nlc3M6IHRydWUsIGZsYWc6IGZsYWcgfTsKICAgIH0KICAgIHJldHVybiB7IHN1Y2Nlc3M6IGZhbHNlLCBmbGFnOiBudWxsIH07Cn0=";

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

function ask() {
    rl.question("[-] Enter the sandbox bypass key: ", (answer) => {
        try {
            // Decrypt/decode the code at runtime
            const code = Buffer.from(obfuscatedCodeBase64, 'base64').toString('utf8');
            
            // Execute the code dynamically inside a VM context
            const sandbox = {};
            vm.createContext(sandbox);
            vm.runInNewContext(code, sandbox);
            
            // Call the dynamically loaded verify function
            const result = sandbox.verify(answer);
            
            if (result.success) {
                console.log(`\n[+] Access Granted! Flag: ${result.flag}`);
                process.exit(0);
            } else {
                console.log("[-] Sandboxed verification failed.\n");
                ask();
            }
        } catch (e) {
            console.log("[-] Error executing sandbox logic:", e.message);
            process.exit(1);
        }
    });
}

ask();
