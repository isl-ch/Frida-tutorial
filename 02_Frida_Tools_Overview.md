# The Frida Toolkit Overview

Frida is more than just the `frida` CLI command. It comes with a suite of tools designed to make different aspects of reverse engineering easier.

## Core CLI Tools

### 1. `frida`
The standard REPL (Read-Eval-Print Loop) and scripting interface.
*   **Usage:** `frida -p <PID> -l script.js` or `frida -U -f com.example.app -l script.js`
*   **Purpose:** Injecting your custom JavaScript payloads into processes and interacting with them live.

### 2. `frida-trace`
A powerful tool for quickly generating boilerplate hooks for functions.
*   **Usage:** `frida-trace -i "recv*" -i "send*" -p <PID>`
*   **Purpose:** Instead of writing an `Interceptor.attach` script from scratch, `frida-trace` automatically generates JavaScript handler files for every function matching the wildcards you provide (e.g., all functions starting with `recv`). You can then edit those generated handlers on the fly.

### 3. `frida-ps`
A process lister that works identically across all platforms (Windows, Linux, macOS, Android, iOS).
*   **Usage:** 
    *   `frida-ps` (list local processes)
    *   `frida-ps -U` (list processes on USB device)
    *   `frida-ps -Ua` (list running applications on USB device)
    *   `frida-ps -Uai` (list installed applications on USB device)

### 4. `frida-kill`
A simple command-line tool to kill a process.
*   **Usage:** `frida-kill -U <PID>`

### 5. `frida-ls-devices`
Lists all attached devices that Frida can communicate with (Local, USB, Remote).

## Language Bindings

While the payload injected into the target is always JavaScript, the *client* communicating with that payload can be written in many languages:

*   **Python (`frida-python`):** The most popular. Great for automating dynamic analysis pipelines, fuzzing, or building custom GUI tools around Frida.
*   **Node.js (`frida-node`):** Excellent for web-based frontends or integrating with Electron apps.
*   **C / Swift:** Used when you need the absolute lowest latency or when building native macOS/iOS analysis tools.

## Ecosystem Tools (Community Built)

The Frida community has built massive frameworks on top of the core API:

*   **Objection:** A runtime mobile exploration toolkit built on Frida. It allows you to bypass SSL pinning, explore the filesystem, and dump memory without writing a single line of JavaScript.
*   **Passionfruit:** An iOS app analyzer with a web-based GUI.
*   **Brida:** A Burp Suite extension that acts as a bridge between Burp Suite and Frida, allowing you to encrypt/decrypt mobile app traffic on the fly for web penetration testing.
