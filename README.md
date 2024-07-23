## **HTTP Server and Client: Joe's Pizza**


### **Overview**

This project is a simple HTTP server implemented in C, designed to serve a static HTML page for a fictional pizza restaurant, Joe's Pizza. The server can handle multiple concurrent client requests using multi-threading and supports basic HTTP methods such as GET, POST, and DELETE.


### **Project Files**



* **Backend Files:**
    * `http.c` and `http.h`: Contains functions for handling HTTP requests and responses.
    * `server.c` and `server.h`: Implements the server logic, socket programming, and multi-threading.
    * `Makefile`: Build script for compiling the server code.
* **Frontend File:**
    * `pizza.html`: Static HTML page representing the menu of Joe's Pizza.
* **Project Report:**
    * `CS158a_FinalReport.pdf`: Detailed documentation of the project including the introduction, methodology, implementation, and conclusion.


### **Compilation and Running Instructions**


#### **Backend**



1. **Navigate to the backend directory:** 
`cd /path/to/backend/files`
2. **Compile the server code using the provided Makefile:** 
`make`
3. **Run the server executable:** 
`./server`

The server will start listening for connections on port 8080.


#### **Frontend**



1. **Locate the <code>pizza.html</code> file.</strong>
2. <strong>Open the <code>pizza.html</code> file in a web browser:</strong> 
    ```
    open pizza.html  # MacOS
    xdg-open pizza.html  # Linux
    start pizza.html  # Windows
    ```


3. You can access the server by navigating to `http://localhost:8080` in your web browser.


### **Project Structure**


#### **Backend**



* **HTTP Protocol:**
    * Utilizes HTTP 0.9 for simplicity.
    * Supports GET, POST, and DELETE methods.
    * Handles HTTP requests and generates appropriate responses.
* **Sockets and Multi-threading:**
    * Uses `sys/socket.h` for socket programming.
    * Implements multi-threading with `pthread.h` to handle multiple client connections concurrently.
    * Server binds to port 8080 and listens for incoming connections.


#### **Frontend**



* **HTML Page:**
    * Static HTML page for Joe's Pizza menu.
    * Simple and clean design using inline CSS for styling.


### **Methodology**



* **Programming Language:** C
* **Libraries Used:** Standard C libraries, `sys/socket.h`, and `pthread.h`.
* **Socket Programming:** Utilizes TCP for reliable data transmission.
* **Multi-threading:** Enables concurrent handling of multiple client requests.
* **HTTP Protocol Implementation:** Based on HTTP 0.9, handling basic HTTP methods.


### **Conclusion**

This project provided valuable insights into server-client communication over the internet, socket programming, and the use of different HTTP methods. The implementation of a simple web server in C enhanced our understanding of network programming and the foundational concepts of web communication.
