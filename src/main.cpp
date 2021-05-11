// Copyright 2021 Sirbu Dan
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <Version.hpp>
#include <iostream>
#include <cstring>

void printHelp() {
    std::cout << "Program usage: huffman [flags] input_file output_file\n";
    std::cout << "Currently supported flags:\n";
    std::cout << "\t-c Compress contents of input_file into output_file\n";
    std::cout << "\t-d Decompress contents of output_file into input_file\n";
    std::cout << "\t-h Display this help message\n";
    std::cout << "\t-i Show info about the program" << std::endl;
}

void printInfo() {
    std::cout << "About huffman-compress:\n";
    std::cout << "\tAuthor: Dan Sirbu (@darwin-s)\n";
    std::cout << "\tCreation date: 11 May 2021\n";
    std::cout << "\tVersion: " << HFM_VER_MAJOR << "." << HFM_VER_MINOR <<
                 "." << HFM_VER_PATCH << "." << HFM_VER_TWEAK << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return -1;
    }

    if (std::strcmp(argv[1], "-c") == 0) { // Compression
        if (argc != 4) {
            printHelp();
            return -1;
        }
    } else if (std::strcmp(argv[1], "-d") == 0) { // Decompression
        if (argc != 4) {
            printHelp();
            return -1;
        }
    } else if (std::strcmp(argv[1], "-h") == 0) { // Help
        printHelp();
        return 0;
    } else if (std::strcmp(argv[1], "-i") == 0) { // Info
        printInfo();
        return 0;
    } else { // Not defined
        printHelp();
        return -1;
    }

    return 0;
}