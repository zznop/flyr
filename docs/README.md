# Introduction

fuzz light year (flyr) is a block-based software vulnerability fuzzing framework. It provides a engine that generates
fuzzed datasets from user-defined data models (JSON files). flyr data models are made up of three categories -
outputs, blocks, and mutations. outputs define the output method (file out, TCP stream, etc.). Blocks are individual data
members of a specific class and type that are used to build the template data buffer. Currently, there are three
classes of blocks - numbers, lengths, and hex strings. Numbers and hex strings are simply user-defined types of data
that are written to the template buffer. Lengths are reserved fields that are fixed up after the template buffer is built.
Blocks of all types can be assigned one or more length fields that are fixed up after the template buffer is built.
Mutations define the method used to modify the template data to create fuzzed inputs.

flyr is under rapid development. In its current state it is capable of building fuzzed datasets for file format fuzzing.
The end goal is a feature rich framework encompassing the mutation engine, a frontend GUI, and crash harnesses for both
Windows and Linux.

# Getting Started

To dive right in checkout the `examples/` directory which contains a flyr-compatible JSON fuzzing model capable of
generating mutated Pcap files containing a DHCP packet. After building flyr, simply run the following command:

```
$ ./build/release/flyr -f ./examples/pcap.json
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

This will generate multiple mutated Pcap files with a name of `*-lolwut.pcap` ready to be fed to a Pcap parsing
software application.

# Programming Guide

Below is documentation on flyr engine supported outputs, blocks, and mutations that can be included in flyr JSON files.
Note that JSON is a very picky file format. The file will fail to load if a comma is misplaced or the file does not
contain valid JSON formatted data. flyr uses the parson C JSON parsing library. parson supports C-style comments. Users
are encouraged to add comments to flyr fuzz files. Flyr fuzz files should start with the template below:
```
{
	"name" : "Descriptive Name Goes Here",

	/* Output method for generated fuzzed inputs */
    "output" : {
    },


    /* Individual members for building the template data buffer */
    "blocks" : {
    },


    /* Methods for mutation the template data buffer to create fuzzed inputs */
    "mutations" : {
    }
}
```

Reference `./examples/pcap.json` for an example flyr data model.

## Output

Output method for generated fuzzed inputs.

### file-out

Generates and writes fuzzed datasets to files in the specified directory.

#### Required Arguments

* **directory-path** - The directory to write the fuzzed files to
* **name-suffix** - Template name of generated fuzzed files.

#### Example

```
"output" : {
    "method" : "file-out",
    "directory-path" : "./",
    "name-suffix" : "lolwut.pcap"
},

```

## Blocks

Individual data members of a specific class and type that are used to build the template data buffer

### Numbers

Write a number to the dataset of specified size and endianess.

##### Required Arguments

* **value** - respective value in base 10 or base 16 format

##### Optional Arguments

* **endianess** - little or big endian (defaults to big endian if not supplied)
* **length-blocks** - length blocks that cover this qword block and need incremented

#### Classes

* **qword** - Write a 64-bit integer to the dataset
* **dword** - Write a 32-bit integer to the dataset
* **word** - Write a 16-bit integer to the dataset
* **byte** - Write a 8-bit integer to the dataset

##### Example

```
"ip_daddr" : {
	"class" : "number",
	"type" : "dword",
	"value" : "0xffffffff",
	"length-blocks" : [
        "ip_tot_len"
    ]
},
```

### Lengths

Length blocks (such as a IP header `tot_len` member) that are fixed up after the dataset is built.

#### Required Arguments

* **type** - data type of length block

##### Optional Arguments

* **endianess** - little or big endian (defaults to big endian if not supplied)

#### Types

* **qword** - Write a 64-bit integer to the dataset
* **dword** - Write a 32-bit integer to the dataset
* **word** - Write a 16-bit integer to the dataset
* **byte** - Write a 8-bit integer to the dataset

#### Example

```
"ip_tot_len" : {
	"class" : "length",
	"type" : "word",
	"endianess" : "little",
	"length-blocks" : [
        "ip_tot_len"
    ]
},
```

### Hex

Raw hex string to be converted to bytes and written to the dataset.

##### Required Arguments

* **value** - hex string value to be written to dataset

#### Optional Arguments
* **length-blocks** - length blocks that cover this hex buffer block and need incremented

##### Example

```

    	"aaaaaaa" : {
    		"class" : "hex",
    		"value" : "41414141414141414141414141414141",
    	    "length-blocks" : [
                "ip_tot_len"
            ]
    	}
```

## Mutations

Methods for mutation the template data buffer to create fuzzed inputs.

### Actions

Types of data mutation.

#### bitflip

Iterate through a specified range flipping a single bit at a time in the generated fuzzed input.


#### Required Arguments

* **start** - start offset into dataset to begin bitflip mutation
* **stop** - stop offset to terminate mutation