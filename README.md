# Huffman Compress
A small program for compressing text data into a simple format. It uses the huffman
algorithm to compress text data and to store it in a simple format. Since this tool is so simple,
it has no external dependencies. This tool uses C++17 standard, so it needs a compiler that
supports it.

## Authors
  - Dan Sirbu (@darwin-s) Main Developer

## Installation
Use CMake to build a release version of this software, and then install it.
It is recommended to use a different directory for building. Here is an example installation, 
assuming you have downloaded the source in a folder named "hfm":

````Shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../hfm
cmake --build .
sudo cmake --install . 
````

## Usage
This software is very easy to use, even though it is command line only.
You write the name of the executable, the flags, and the name of the input
and output files. Here are the currently supported flags:

Flag | Description
-----|------------
-c   | Compress contents of input file into the output file
-d   | Decompress the contents of the input file into the output file
-h   | Display the help message
-i   | Display more information about this software

## License
The project is licensed under the [Apache License 2.0](https://choosealicense.com/licenses/apache-2.0/).
