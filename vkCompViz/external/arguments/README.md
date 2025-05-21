# Lightweight and simple argument parser 

Good for fast prototyping. Simply reads the params and provides a map with the parameter name and value. 

## Installation 
Include the *arguments.hpp* file. Example of the usage is in the *test.cpp*. \
The example can be run as: 
```
./test -e --lightsaber_power 3.1415 --nazghul_count 42 --spell "Brisingr!"
```
With the (expected) output:
```
I am here
I am 3.1415 m heigh!
I am scared 42 times!
Brisingr!
```
Check out commit [d6db766](https://github.com/ichlubna/arguments/tree/d6db7660a459b820b8d06b3c47787fca299080f9) for non-modularized standard header hpp version.

## Supported features 
- takes everything starting with "-" or "--", excluding numbers (anything that starts with "-[0-9]" or "--[0-9]"), as a parameter name
- allows implicit casting of the result to basic types with exceptions if not possible
- automatically detects "-h" and "--help" and prints the help string
- does not support predefined arguments, simply parses everything and ignores many possible mistakes

