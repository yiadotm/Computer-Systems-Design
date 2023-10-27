#Assignment 2 directory

This directory contains source code and other files for Assignment 2.

Use this README document to store notes about design, testing, and
questions you have while developing your assignment.

There are 6 files in this assignment
httpserver.c - the main execuble  
Makefile  
io.c         - file for I/O handlers  
io.h         - header file for I/O handlers  
Request.c    - file that holds request  
Request.h    - header for request

Questions asked from ChatGPT:  
how do I use regex  


can you tell me why this regex string isnt matching
"^([a-zA-Z]{1,8}) (//[a-zA-Z0-9.-]{1,63}) "
          "HTTP//([0-9]\\.[0-9])\r\n([a-zA-Z0-9.-]{1,128}: "
          "[\x20-\x7E]{1,128}\r\n)*\r\n$"

PUT /foo.txt HTTP/1.1\r\nContent-Length: 12\r\n\r\nHello World!
im doing this in c  

how do i get the last 4 characters in a string array in c  


assuming buf is a char array and total is the total amount of characters in the array, is this correct if i want to find the last 4 characters of the array?
buf + total - 3  

