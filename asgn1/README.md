#Assignment 1 directory

This directory contains source code and other files for Assignment 1.

Use this README document to store notes about design, testing, and
questions you have while developing your assignment.



Makefile is referenced from an assignment from CSE13s.  

Assignment Details:  

- User passes in string arguments with either 'get' or 'set' commands.  
- '\n' is used to separate words.  

- 'get' command has one parameter:  
    <\location> - output file is set to location (if it exists)  
        - catch error if it does not exist  

- 'set' command has 3 parameter:  
    <\location> - place where contents are located (if it exists)  
        catch error  
    <\content_length> -  number of bytes  
    <\contents> - name of file  