# Secure_Server_Messaging

# Build
1. cd into the mtserver directory
2. Build the server application by typing 'make'
3. Run the server application by typing './server #ports port# port_address'
   Example: ./server 5 9999 localhost
4. Run the c++ client application by typing './client #ports port# port_address'
   Example: ./client 5 9999 localhost
5. Run the android client by doing the following:
	1) Open up the project in Android studio
	2) Press the green arrow at the top that says "run app" to compile and run the application
	
#Special acknowledgements:
All credit for the base code for the server's design (Multithreaded Work Queue Based Server) goes to Vic Hargrave.
I simply built upon the following code, re-engineering it to fit my purpose.

Base code can be found on the following webstes.

1. https://github.com/vichargrave/mtserver
2. https://github.com/vichargrave/threads
3. https://github.com/vichargrave/wqueue
4. https://github.com/vichargrave/tcpsockets