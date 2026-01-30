## Lossless Data Compression Tool

### Overview

This project is a lightweight, command-line utility for lossless data compression and decompression, implemented in C++. It demonstrates low-level memory management and algorithmic efficiency in handling string data.

The tool focuses on reducing the memory footprint of text data while ensuring that the original data can be perfectly reconstructed without any loss of information.

### Key Features

Lossless Compression: Guarantees 100% data integrity upon decompression.

Memory Efficiency: Optimized to minimize runtime memory usage using C++ pointers and references.

Run-Length Encoding (RLE) / Custom Algorithm: (Not: Kullandığın spesifik algoritmayı buraya yazabilirsin, örneğin: Huffman Coding, LZW veya basit RLE) efficient algorithms for repetitive data patterns.

CLI Interface: Easy-to-use command-line interface for compressing and decompressing files or strings.

### Technical Stack

Language: C++

Concepts: Data Structures, Memory Management (Pointers), File I/O, Compression Algorithms.

### Usage

Compile the code:

g++ -o compressor Source.cpp


Run the tool:

./compressor input.txt compressed.bin


Decompress:

./compressor -d compressed.bin output.txt


### License

This project is open-source and available under the MIT License.

Created by Yusufcan Demirkapı
