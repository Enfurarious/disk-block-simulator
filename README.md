# Disk Block Simulator

This project is a C-based disk block simulator developed as part of an operating systems course. It simulates low-level disk behavior using block-based read and write operations, allowing testing of storage logic and data integrity.

## Features

- Simulates a block storage device using a file as the disk image  
- Implements block-level read and write operations  
- Handles memory allocation and data transfer between buffers  
- Includes randomized testing to validate correctness and edge cases  
- Detects and handles invalid read/write operations  

## Technologies Used

- C  
- CMake  
- Linux-based development environment  

## Project Structure
disk-block-simulator/
│
├── main.c
├── CMakeLists.txt
├── tests.txt
├── lib/
│ ├── libciblk.c
│ ├── libciblk.h
│ └── ciblk.h

## How to Build
mkdir build
cd build
cmake ..
make

## How to Run

Before running, create a disk image file:

dd if=/dev/zero of=ciblk.img bs=1024 count=1

Then run the program:

./simblock ciblk.img

## What This Project Demonstrates

- Understanding of low-level storage systems  
- Experience with memory management in C  
- Ability to design and test system-level software  
- Familiarity with Linux development tools and workflows  

## Notes

This project was developed as part of a university operating systems course. 
Core functionality and implementation were completed independently.
