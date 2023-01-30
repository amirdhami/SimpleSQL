/*util.h*/

//
// Project: utility functions for SimpleSQL
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once


//
// panic
//
// Outputs the given error message to stdout and then
// exits the program with error code of -1.
//
void  panic(char* msg);

//
// dupString
// 
// Duplicates the given string and returns a pointer
// to the copy.
// 
// NOTE: this function allocates memory for the copy,
// the caller takes ownership of the copy and must
// eventually free that memory.
//
char* dupString(char* s);

//
// dupStrings
// 
// Given 2 strings, makes a copy by concatenating 
// them together, and returns the copy.
// 
// NOTE: this function allocates memory for the copy,
// the caller takes ownership of the copy and must
// eventually free that memory.
//
char* dupStrings(char* s1, char* s2);

//
// dupAndStripEOLN
// 
// Duplicates the given string and returns a pointer
// to the copy; any EOLN characters (\n, \r, etc.)
// are also removed.
// 
// NOTE: this function allocates memory for the copy,
// the caller takes ownership of the copy and must
// eventually free that memory.
//
char* dupAndStripEOLN(char* s);

//
// icmpStrings
//
// case-insensitive comparison of strings s1 and s2.
// Like strcmp, returns 0 if s1 == s2 and returns a
// non-zero value if s1 != s2.
//
int   icmpStrings(char* s1, char* s2);
