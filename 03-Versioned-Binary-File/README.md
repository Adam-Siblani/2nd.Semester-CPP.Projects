# Versioned Binary File

## Problem Description

This project implements an in-memory binary file simulator with support for file versioning, snapshots, and restoration of previous states.
The file behaves similarly to a real binary file, supporting reading, writing, seeking, truncation, and file size management. Additionally, the implementation allows storing unlimited historical versions of the file and restoring them later through an undo mechanism.
A major requirement of the assignment was efficient memory usage. Multiple file instances and versions may share unchanged data using reference counting and copy-on-write techniques.

## Key Features

* Binary file simulation in memory
* Read and write operations
* File seeking support
* File truncation
* Snapshot creation
* Multi-level undo functionality
* Deep copy semantics
* Copy constructor and assignment operator support
* Reference counting
* Copy-on-write optimization
* Chunk-based storage architecture

## Technical Implementation

The file contents are divided into fixed-size memory chunks.
Each chunk is reference counted and may be shared between multiple file instances and file versions. Whenever a modification is required, copy-on-write logic creates a private copy of the modified chunk while unchanged chunks remain shared.
File snapshots store references to previous file states rather than duplicating the entire file contents, significantly reducing memory consumption.
The implementation also supports independent copies of files while preserving all historical versions.

## Concepts Used

* Object-Oriented Programming (OOP)
* Dynamic Memory Management
* Reference Counting
* Copy-On-Write (COW)
* Deep Copy vs Shallow Copy
* Custom Memory Containers
* Version Control Concepts
* Binary Data Processing
* Resource Management
* Low-Level File Simulation

