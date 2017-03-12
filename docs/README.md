## With Dudley you can build fuzzers with ease

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

A few arguments are required to run dudley. These arguments include -i, -p, and 
-f to supply the target information and file path to the dudley file.
```
$ ./dudley -i 10.0.0.1 -p 80 -f /fuzzers/fuzzer.dud
```
