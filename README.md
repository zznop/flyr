# Dudley (WIP)

## Description

Dudley is a automated software vulnerability analysis and fuzzing framework

## Building Dudley

Dudley is built using SCons. You can install SCons onto a Linux machine and
build using the following command:

```
$ scons -j 4
```

### Docker

Alternatively, this repository contains a Dockerfile and
[Scuba](https://github.com/JonathonReinhart/scuba) YAML file that allows you
to build the project using docker. To build the docker image run the
`build.sh` script. It will create an image tagged `dudley-build:latest`

A [Scuba]() YAML file has been included in the project for use with Docker.
To install Scuba, run the following command:

```
$ sudo apt-get install python-pip
$ sudo pip install scuba
```

Then, to build the project - simply run `scuba build` or `scuba scons -j 4`.
You can also run dudley using scuba and the build docker image by prefixing
your command with `scuba`.

## Getting Started

```
$ cat test/test.json
{
    "name" : "Example Dudley File",
    "output" : {
        "method" : "file-out",
        "directory-path" : "./",
        "name-suffix" : "example.bin"
    },
    "actions" : {
        "input all the things" : {
            "action" : "consume",
            "type" : "hex",
            "data" : "deadbeeffeedface"
        }
    },
    "mutations" : {
        "mutate all the things" : {
            "action" : "bitflip",
            "start" : "0x0",
            "stop" : "0x10"
        }
    }
}

```

```
$ ./build/release/dudley -f test/test.json 
* Loading dudley file and validating the JSON schema
* test/test.json (Example Dudley File) loaded successfully!
* Applying build actions...
* Processing mutations...
* Done.
```

```
$ ls
1538199747_585456_example.bin  1538199747_586280_example.bin  1538199747_586846_example.bin
1538199747_585789_example.bin  1538199747_586326_example.bin  1538199747_586891_example.bin
...
```