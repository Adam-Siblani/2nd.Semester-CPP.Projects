# VAT Revenue Register

## Problem Description

This project implements a simplified VAT registration database used for managing tax records of registered companies.
Each company is uniquely identified by a tax ID and can also be referenced using a unique combination of company name and address. The system supports company registration, removal, invoice processing, auditing of company revenue, median invoice computation, and alphabetical iteration through registered companies.
The implementation was designed with strict performance requirements, where company lookups and invoice processing must remain efficient even for large datasets.

## Key Features

* Registration of new companies with uniqueness validation
* Company removal using either Tax ID or Name/Address
* Revenue tracking through invoice registration
* Company auditing and revenue reporting
* Case-insensitive company name and address handling
* Alphabetical company iteration
* Real-time median invoice computation
* Efficient binary-search based lookups

## Technical Implementation

The solution maintains two sorted collections:

* A company index sorted by company name and address
* A company index sorted by tax ID

Both structures allow efficient binary-search operations for frequent lookups.
To support fast median invoice calculation, the implementation uses a two-heap approach:

* Max-heap for the lower half of invoices
* Min-heap for the upper half of invoices

This allows median updates in logarithmic time after each successful invoice insertion.

## Concepts Used

* Object-Oriented Programming (OOP)
* STL Vectors
* Smart Pointers (`std::shared_ptr`)
* Binary Search (`std::lower_bound`)
* Heap Data Structures
* Custom Sorting
* Case-Insensitive String Processing
* Dynamic Memory Management
* Algorithm Optimization

