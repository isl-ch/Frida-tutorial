# Solutions for Frida CTF Challenges

Here are the solution scripts for the 9 challenges if you get stuck.

## Easy 1: `solve_01.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "check_pin"), {
    onLeave: function(retval) {
        retval.replace(1);
        console.log("[+] Replaced return value with 1!");
    }
});
```

## Easy 2: `solve_02.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "log_debug"), {
    onEnter: function(args) {
        var msg = args[0].readUtf8String();
        console.log("[+] Intercepted log_debug msg: " + msg);
    }
});
```

## Easy 3: `solve_03.js`
```javascript
var win_addr = Module.getExportByName(null, "win");
var win_func = new NativeFunction(win_addr, 'void', []);
win_func();
```

## Medium 1: `solve_04.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "player_attack"), {
    onEnter: function(args) {
        args[0] = ptr(9999);
        console.log("[+] Modified damage to 9999!");
    }
});
```

## Medium 2: `solve_05.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "check_user"), {
    onEnter: function(args) {
        var user_ptr = args[0];
        var is_admin_ptr = user_ptr.add(32); // offset
        is_admin_ptr.writeInt(1);
    }
});
```

## Medium 3: `solve_06.js`
```javascript
var ranges = Process.enumerateRanges({protection: 'rw-', coalesce: true});
ranges.forEach(function(range) {
    try {
        var matches = Memory.scanSync(range.base, range.size, "48 54 42 7B"); // HTB{
        matches.forEach(function(match) {
            console.log("[+] Found flag at: " + match.address);
            console.log("[+] Value: " + match.address.readUtf8String());
        });
    } catch(e) {}
});
```

## Hard 1: `solve_07.js`
```javascript
// Run with: frida -f ./07_hard_antidebug -l solve.js --no-pause
Interceptor.attach(Module.getExportByName(null, "ptrace"), {
    onLeave: function(retval) {
        if (retval.toInt32() === -1) {
            retval.replace(0);
        }
    }
});

setTimeout(function() {
    var print_flag = new NativeFunction(Module.getExportByName(null, "print_flag"), 'void', []);
    print_flag();
}, 500);
```

## Hard 2: `solve_08.js`
```javascript
Interceptor.attach(Module.getExportByName(null, "check_integrity"), {
    onLeave: function(retval) {
        retval.replace(1); // Force it to return true
    }
});

Interceptor.attach(Module.getExportByName(null, "secure_function"), {
    onEnter: function() {
        console.log("[+] successfully hooked!");
    }
});
```

## Hard 3: `solve_09.js`
```javascript
// A simple bypass is hooking the XOR instruction directly using base + offset
var baseAddr = Module.getBaseAddress('09_hard_stalker');

// Note: 0x1247 is a placeholder. You must find the real offset using objdump -d 09_hard_stalker
// It is the instruction immediately after the XOR.
var instruction_offset = 0x1247; 

Interceptor.attach(baseAddr.add(instruction_offset), {
    onEnter: function(args) {
        // Read the AL/EAX register which contains the computed byte
        // console.log("Char:", this.context.rax);
    }
});
```
