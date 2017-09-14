# Secure_Server_Messaging
![C++](https://img.shields.io/badge/c%2B%2B-11%20-blue.svg)
![Java](https://img.shields.io/badge/java-1.8-orange.svg)
[![Build Status](https://img.shields.io/travis/bryanlng/Secure_Server_Messaging/master.svg)](https://travis-ci.org/bryanlng/Secure_Server_Messaging)
[![GitHub Issues](https://img.shields.io/github/issues/bryanlng/Secure_Server_Messaging.svg)](https://github.com/bryanlng/Secure_Server_Messaging/issues)

## Description
A small-scale messenging application built using C++ and Java.

## Features
The application consists of 2 parts:
1. Minimalistic android GUI
2. Multi-threaded C++ Server backend

## Note: This project is almost complete, there's still a few kinks that I have to fix

## Installation
### Requirements
This project runs on the following tools
1. <a href = "http://www.stack.nl/~dimitri/doxygen/index.html">Doxygen 1.183</a>
2. <a href = "https://github.com/google/googletest">GTest (coming soon!)</a>
3. <a href = "https://gcc.gnu.org/onlinedocs/gcc/Gcov.html">Gcov (coming soon!)</a>
4. <a href = "https://github.com/google/googletest/tree/master/googlemock">Google Mock (coming soon!)</a>
5. <a href = "http://valgrind.org/">Valgrind (coming soon!)</a>

On a Ubuntu environment, these requirements can be fulfilled using the following sudo apt-get commands:
1. Doxygen:	`sudo apt-get install doxygen`	
2. GTest:	`sudo apt-get install libgtest-dev`
3. GCov:	`sudo apt-get install gcovr`
4. GMock:	`sudo apt-get install google-mock`
5. Valgrind:	`sudo apt-get install valgrind`

### Build
1. Clone the repository by typing in `git clone https://github.com/bryanlng/Secure_Server_Messaging.git` 
2. `cd` into the mtserver directory by typing
2. Build the server application by typing `make`
3. Run the server application by typing ```./server <ports> <port #> <port_address>
   Example: ./server 5 9999 localhost```
4. Run the c++ client application by typing ```./client <port #> <port_address>
   Example: ./client 9999 localhost```
5. Run the android client by doing the following:
	1) Open up the project in Android studio
	2) Press the green arrow at the top that says "run app" to compile and run the application

## Documentation
### Forenote
As of now, there's only documentation for the server. Further documentation for the front-end Android application will come after I finish a few more crucial elements to it. That being said...

### Generating documentation for the server
1. `cd` into the mtserver directory
2. Build the server documentation by typing `make docs`
3. `cd` into the html directory
4. Open the file `index.html` in a browser.

## Upcoming features
1. Make front-end Android application fully functional.
2. Modify tcpsockets file to use SSL
3. Implement mocking for more in-depth testing of features

## Special acknowledgements:
All credit for the base code for the server's design (Multithreaded Work Queue Based Server) goes to Vic Hargrave.
I simply built upon the following code, re-engineering it to fit my purpose.

Base code can be found on the following sites.
1. <a href = "https://github.com/vichargrave/mtserver">Original mtserver implementation</a>
2. <a href = "https://github.com/vichargrave/threads">Original thread implementation</a>
3. <a href = "https://github.com/vichargrave/wqueue">Original work queue implementation</a>
4. <a href = "https://github.com/vichargrave/tcpsockets">Original TCP Socket implementation</a>
