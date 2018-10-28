# fuzz light year (flyr)

## Description

fuzz light year is a block-based software fuzzing framework

## Building flyr

flyr is built using SCons. You can install SCons and build on a
Linux machine (or Cygwin) using the following command:

```
$ scons -j 4
```

### Docker

Alternatively, this repository contains a Dockerfile and
[Scuba](https://github.com/JonathonReinhart/scuba) YAML file that allows you
to build the project using docker. To build the docker image run the
`build.sh` script. It will create an image tagged `flyr-build:latest`

A [Scuba]() YAML file has been included in the project for use with Docker.
To install Scuba, run the following command:

```
$ sudo apt-get install python-pip
$ sudo pip install scuba
```

Then, to build the project - simply run `scuba build` or `scuba scons -j 4`.
You can also run flyr using scuba and the build docker image by prefixing
your command with `scuba`.

## Getting Started

```
cat ./examples/pcap.json

{
    "name" : "Pcap Fuzzing Model",

    /* Generate fuzzed pcap files in the current working directory */
    "output" : {
        "method" : "file-out",
        "directory-path" : "./",
        "name-suffix" : "lolwut.pcap"
    },

    "blocks" : {

        /* Write the Pcap Header */

        "magic_number" : {
            "class" : "number",
            "type" : "dword",
            "value" : "0xd4c3b2a1"
        },

        "version_major" : {
            "class" : "number",
            "type" : "word",
            "value" : "0x0002",
            "endianess" : "little"
        },

        "version_minor" : {
            "class" : "number",
            "type" : "word",
            "value" : "0x0004",
            "endianess" : "little"
        },

        ...
```

```
$ ./build/release/flyr.exe -f examples/pcap.json
* Loading flyr file and validating the JSON schema
* examples/pcap.json (Pcap Fuzzing Model) loaded successfully!
* Applying build actions...
*   -- magic_number
*   -- version_major
*   -- version_minor
*   -- thiszone
*   -- sigfigs
*   -- snaplen
*   -- network
*   -- ts_sec
*   -- ts_usec
*   -- incl_len
*   -- orig_len
*   -- eth_dst
*   -- eth_src
*   -- eth_type
*   -- ip_tos
*   -- ip_diff_services
*   -- ip_tot_len
*   -- ip_id
*   -- ip_frag_off
*   -- ip_ttl
*   -- ip_protocol
*   -- ip_check
*   -- ip_saddr
*   -- ip_daddr
*   -- udp_header_and_payload
* Processing mutations...
* Done.
```

```
$ ls
...
00000032-lolwut.pcap  00000033-lolwut.pcap  00000034-lolwut.pcap
...

$ file *
...
00000032-lolwut.pcap: tcpdump capture file (little-endian) - version 3.4 (Ethernet, capture length 65535)
00000033-lolwut.pcap: tcpdump capture file (little-endian) - version 0.4 (Ethernet, capture length 65535)
00000034-lolwut.pcap: tcpdump capture file (little-endian) - version 6.4 (Ethernet, capture length 65535)
...
```