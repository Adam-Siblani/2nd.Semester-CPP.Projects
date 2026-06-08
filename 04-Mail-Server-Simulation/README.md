# Mail Server Simulation

## Problem Description

This project implements a simplified mail server mailbox system capable of storing, organizing, and retrieving e-mails.
The system supports multiple folders, e-mail delivery, attachment handling, mailbox organization, mail transfers between folders, and efficient querying of messages within specified time intervals.
The implementation focuses on efficient access to e-mails based on timestamps while minimizing unnecessary copying when moving messages between folders.

## Key Features

* E-mail representation using custom classes
* Mailbox folder management
* Mail delivery simulation
* Attachment support with reference counting
* Folder creation and organization
* Moving e-mails between folders
* Time-based e-mail filtering
* Sender address extraction
* Automatic mailbox initialization with inbox folder
* Efficient timestamp indexing

## Technical Implementation

The mailbox stores folders using STL associative containers.
Each folder maintains e-mails grouped by timestamps, allowing efficient retrieval using ordered map operations. Date range queries utilize binary-search-based lookups through lower_bound and upper_bound operations.
Attachments are managed through reference counting to avoid unnecessary duplication of attachment data between e-mail instances.
Move semantics are used when transferring e-mails between folders to minimize copying overhead and improve performance.

## Concepts Used

* Object-Oriented Programming (OOP)
* STL Maps
* STL Sets
* STL Vectors
* STL Lists
* Smart Resource Management
* Reference Counting
* Move Semantics
* Date Range Queries
* Efficient Data Organization

