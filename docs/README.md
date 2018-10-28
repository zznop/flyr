# Introduction

fuzz light year (flyr) is a block-based software vulnerability fuzzing framework. It provides a engine that generates
fuzzed datasets from user-defined data models (JSON files). flyr data models are made up of three categories -
sinks, blocks, and mutations. Sinks define the output method (file out, TCP stream, etc.). Blocks are individual data
members of a specific class and type that are used to build the template data buffer. Mutations define the method used
to modify the template data to create fuzzed inputs.

# Programming Guide

Below is a list of flyr engine supported sinks, blocks, and mutations that can be included in flyr JSON files.

## Sinks

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

### Actions

Types of data mutation.

#### bitflip

Iterate through a specified range flipping a single bit at a time in the generated fuzzed input.


#### Required Arguments

* **start** - start offset into dataset to begin bitflip mutation
* **stop** - stop offset to terminate mutation