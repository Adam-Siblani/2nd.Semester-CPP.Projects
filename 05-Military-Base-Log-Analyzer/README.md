# Military Base Log Analyzer

## Problem Description

This project implements a military base visitor analysis system capable of processing access logs and determining which individuals may have visited specific zones within a secured facility.
The military base consists of interconnected zones separated by security checkpoints. Visitor movement is reconstructed from perimeter gate logs and internal base topology, allowing the system to estimate whether a person could have reached a particular location during a specified time interval.
The solution supports multiple log formats, visitor filtering, and efficient search operations across large datasets.

## Key Features

* Military base topology modeling
* Visitor log processing
* Zone access analysis
* Time interval filtering
* Visitor search queries
* Multi-zone traversal tracking
* Text and binary log support
* Date and time validation
* Efficient visitor lookup
* Security checkpoint simulation

## Technical Implementation

The base structure is represented as a graph where zones are vertices and security checkpoints are weighted connections.
Visitor activity is reconstructed from perimeter gate records and transformed into searchable visit intervals.
The implementation uses STL associative containers to efficiently store and retrieve visitor information. Query operations filter candidate visitors based on zone accessibility and temporal constraints.
The architecture separates military base configuration, visitor logs, and search filters into independent classes, improving modularity and maintainability.

## Concepts Used

* Object-Oriented Programming (OOP)
* Graph Modeling
* File Parsing
* Log Processing
* Date and Time Handling
* STL Maps
* STL Sets
* Search Algorithms
* Data Filtering
* Software Architecture
