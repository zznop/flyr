# Dudley allows you to build powerful fuzzers with ease!

## How does it work?
Dudley takes a block-based approach to fuzzer construction and makes use of a 
simple markup language to define data inputs for packet building. These dudley 
files are parsed by the Dudley command line interface and sent over the 
specified protocol to the target system. In its current state, Dudley supports 
fuzzing over TCP and UDP sessions.

## Dudley CLI Usage

To display dudley usage run the app with no arguments (or -h)
```
$ ./dudley 
Usage: dudley [options] -f [file]
-h         display this menu
-u         fuzz using UDP protocol (if not set TCP will be used)
-i IP      target service host address
-p PORT    target service port
-t SECS    send and receive timeout in seconds
-f FILE    path to Dudley fuzz file

```

A few arguments are required to run Dudley. These arguments include -i, -p, and 
-f to supply the target information and file path to the Dudley file.
```
$ ./dudley -i 10.0.0.1 -p 80 -f /fuzzers/fuzzer.dud
```

## Dudley File Building

Dudley files are composed of an extremely simple markup language. Each line 
contains a Dudley instruction for packet building and fuzzer control.

### d_string
Write a string into the packet buffer
```
(d_string "hello\r\n")
```

### d_string_repeat
Write a string n times into the packet buffer
```
(d_string_repeat "A" 1024)
```

### d_binary
Pack and write a hex string as binary into the packet buffer
```
(d_binary "de ad be ef")
```

### d_binary_repeat
Pack and write a hex string as binary n times into the packet buffer
```
(d_binary "fe ed fa ce" 64)
```

### d_send
Send packet buffer to target over communications session
```
(d_send)
```

### d_clear
Clear the packet buffer
```
(d_clear)
```

### d_hexdump
Display hex dump of the packet buffer. This function is handy for debugging.
```
(d_hexdump)
```
