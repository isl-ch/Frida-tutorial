# Frida Dynamic Instrumentation Course

Welcome to the Frida CTF Course! This tutorial will teach you the core concepts of Frida, providing examples and techniques that you can then apply to solve the 9 provided challenges. 

## 1. Introduction to Frida

Frida is a dynamic instrumentation toolkit for developers, reverse engineers, and security researchers. It allows you to inject snippets of JavaScript or your own library into native apps on Windows, macOS, GNU/Linux, iOS, Android, and QNX.

### Basic Setup
1. **Install Frida**: `pip install frida-tools`
2. **Writing a Script**: You write your logic in a `.js` file.
3. **Attaching**: 
   - By Process ID (PID) of an already running program: `frida -p 1234 -l script.js`
   - By spawning a binary (useful for catching startup logic): `frida -f ./my_binary -l script.js`
   - Note: Modern Frida auto-resumes the spawned process. Use `--pause` if you need to stop at entry point.

### The Frida Javascript API
Frida exposes a massive JavaScript API. The most important objects we'll use are:
- `Interceptor`: For hooking and intercepting functions.
- `Module`: For finding bases and exports of loaded libraries/binaries.
- `Memory`: For reading, writing, and scanning raw memory.
- `NativeFunction` / `NativePointer`: For interacting with native C functions and pointers.

---

## 2. Function Hooking (`Interceptor`)

The bread and butter of Frida is hooking functions. This means intercepting execution just before a function runs (`onEnter`) and just after it finishes (`onLeave`).

### Finding the Function
If the binary is not stripped, you can find a function by its name. If it's stripped, you need its address (offset from base).
```javascript
// 1. Find a LIBRARY function by its exported name (e.g., libc's strcmp)
//    getExportByName only works for functions in the ELF *export table*
var strcmpPtr = Module.getExportByName(null, "strcmp");

// 2. Find an INTERNAL function by its debug symbol name
//    This works as long as the binary is NOT stripped.
//    Use this for functions like check_pin(), win(), etc.
var myFuncPtr = DebugSymbol.fromName("my_custom_function").address;

// 3. If stripped, calculate address using base + offset (found via Ghidra/objdump)
var baseAddr = Module.getBaseAddress("my_binary");
var strippedFuncPtr = baseAddr.add(0x114A);

// IMPORTANT: Use findExportByName (returns null) over getExportByName (throws)
//            when you are not sure if a symbol is exported.
var safePtr = Module.findExportByName(null, "some_func");
if (safePtr === null) { console.log("Not found in exports!"); }
```

### Hooking with `Interceptor.attach`
Once you have the pointer, you can attach to it.

```javascript
Interceptor.attach(myFuncPtr, {
    onEnter: function(args) {
        // args is an array of NativePointers representing the arguments
        // E.g., for int my_func(int a, char *b):
        var arg1 = args[0].toInt32(); // Read as integer
        var arg2 = args[1].readUtf8String(); // Read pointer as string
        
        console.log("Called with: " + arg1 + " and " + arg2);
        
        // You can MODIFY arguments!
        args[0] = ptr(9999); // Change the first argument to 9999
    },
    onLeave: function(retval) {
        // retval is a NativePointer representing the return value
        console.log("Original return value: " + retval.toInt32());
        
        // You can MODIFY the return value!
        retval.replace(1); // Force the function to return 1
    }
});
```
*Skills learned here will help you solve: 01_easy_return, 02_easy_args, 04_medium_rpg.*

---

## 3. Calling Native Functions

Sometimes you don't want to hook a function, but rather call a hidden function yourself. You can define a C function signature in JavaScript using `NativeFunction`.

### Example: Calling a hidden `print_secret()`
Assume a function `void print_secret(int key)` exists at a known address.

```javascript
// print_secret is an internal function, NOT an export, so use DebugSymbol
var funcPtr = DebugSymbol.fromName("print_secret").address;

// Define the signature: NativeFunction(address, returnType, [argTypes...])
var printSecret = new NativeFunction(funcPtr, 'void', ['int']);

// Call it from Javascript!
console.log("Calling hidden function...");
printSecret(42);
```
*Skills learned here will help you solve: 03_easy_call.*

---

## 4. Reading and Writing Memory (`Memory`)

Often, data is passed as pointers to structs or buffers. You'll need to read from and write to these memory locations directly.

### Interacting with Pointers
```javascript
// Assume args[0] is a pointer to `struct User { char name[32]; int age;  }`
var structPtr = args[0];

// Read the string at offset 0
var name = structPtr.readUtf8String();

// Read the integer at offset 32 (size of char array)
var agePtr = structPtr.add(32);
var age = agePtr.readInt();

// Write to memory
agePtr.writeInt(99); // Change age to 99
```

### Scanning Memory
If a flag or key is generated dynamically but not printed, it might be sitting in memory.

```javascript
// Scan all readable/writable memory ranges
var ranges = Process.enumerateRanges({protection: 'rw-', coalesce: true});

ranges.forEach(function(range) {
    // Search for the hex pattern of "FLAG{" (46 4c 41 47 7b)
    try {
        var matches = Memory.scanSync(range.base, range.size, "46 4c 41 47 7b");
        matches.forEach(function(match) {
            console.log("Found pattern at: " + match.address);
            console.log("Data: " + match.address.readUtf8String());
        });
    } catch(e) { /* ignore unreadable ranges */ }
});
```
*Skills learned here will help you solve: 05_medium_struct, 06_medium_memscan.*

---

## 5. Bypassing Anti-Debugging and Hook Detection

Many challenges employ anti-debugging tricks. Since Frida is a dynamic toolkit, it often triggers anti-debug checks.

### Bypassing `ptrace`
A common Linux anti-debug check is calling `ptrace(PTRACE_TRACEME)`. If it returns `-1`, a debugger is present.

To bypass this, you simply hook the libc `ptrace` function and force it to return `0`!
```javascript
var ptracePtr = Module.getExportByName(null, "ptrace");
Interceptor.attach(ptracePtr, {
    onLeave: function(retval) {
        if (retval.toInt32() === -1) {
            console.log("Bypassing ptrace check!");
            retval.replace(0);
        }
    }
});
```
*(Remember to use `frida -f ./binary` to spawn and catch early startup checks. Modern Frida auto-resumes by default.)*

### Evading Hook Detection
If a program checks its own code for modifications (looking for `JMP` or `INT3` instructions that Frida inserts), you can either:
1. Hook the function doing the checking and force it to return true.
2. Intercept the execution before the check happens and fake the memory state.
*Skills learned here will help you solve: 07_hard_antidebug, 08_hard_integrity.*

---

## 6. Advanced Instruction Tracing (`Frida.Stalker`)

Sometimes, standard function hooks aren't enough—especially against heavily obfuscated code or custom cryptography loops where the values are stored only in CPU registers and quickly erased.

`Stalker` is Frida's code tracing engine. It acts like a dynamic recompilation engine, following threads at the instruction level.

### Basic Stalker Usage
You generally wrap a function call with Stalker.
```javascript
Interceptor.attach(targetFunc, {
    onEnter: function(args) {
        console.log("Starting trace...");
        Stalker.follow({
            events: {
                call: false, // Don't trace calls
                ret: false,  // Don't trace returns
                exec: true   // Trace every single instruction executed
            },
            onReceive: function (events) {
                // Parse the raw instruction events here
                // Very advanced - usually requires external tools to parse
            }
        });
    },
    onLeave: function(retval) {
        Stalker.unfollow();
        console.log("Trace finished.");
    }
});
```
For most reverse engineering CTFs involving Stalker, you might instead use `Stalker.addCallout` to run Javascript before specific instructions, or simply resort to standard `Interceptor.attach(INSTRUCTION_ADDRESS)` if you know the exact offset of the XOR/computation logic from analyzing the binary in Ghidra/objdump.
*Skills learned here will help you solve: 09_hard_stalker.*

---

## Conclusion
You now have the theoretical knowledge required to solve all 9 challenges in this curriculum! You will likely need to reference the official Frida Javascript API documentation as you build your solution scripts. Good luck!
