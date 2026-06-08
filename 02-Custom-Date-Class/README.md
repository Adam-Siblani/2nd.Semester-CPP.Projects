# Custom Date Class

## Problem Description

This project implements a fully featured custom date class following the Gregorian calendar system.
The class supports date arithmetic, comparison operations, stream input/output, validation of calendar dates, and configurable date formatting. Invalid dates are detected and reported through custom exceptions.
The implementation provides functionality similar to standard date libraries while demonstrating advanced operator overloading and object-oriented programming techniques.

## Key Features

* Gregorian calendar date representation
* Date validation with exception handling
* Addition and subtraction of days
* Difference calculation between two dates
* Prefix and postfix increment/decrement operators
* Full comparison operator support
* Stream input and output operators
* Custom date formatting manipulator
* Leap year handling
* ISO 8601 date support

## Technical Implementation

Dates are internally stored as a continuous day count representation, allowing efficient date arithmetic and comparisons.
The implementation converts between calendar dates and day-count values using mathematical calendar algorithms rather than iterative date traversal.
A custom stream manipulator was implemented to support arbitrary input and output date formats while preserving stream-specific formatting settings.

## Concepts Used

* Object-Oriented Programming (OOP)
* Operator Overloading
* Exception Handling
* Stream Operators
* Custom Stream Manipulators
* Gregorian Calendar Algorithms
* Date Arithmetic
* Input Validation
* Dynamic Stream Formatting
* STL Streams

