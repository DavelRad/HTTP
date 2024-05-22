
#include "server.h"

TaskList taskList = {{{0}}, 0, PTHREAD_MUTEX_INITIALIZER};


int main(void) {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8080);

  printf("Binding...\n");
  if (bind(server_socket, (struct sockaddr *) &addr, addr_len) == -1) {
    perror("Failed to bind socket\n");
    exit(EXIT_FAILURE);
  }

  printf("Listening...\n");
  // Set max number of connections that we can handle
  if (listen(server_socket, MAX_NUM_THREADS) == -1) {
    perror("Failed to start accepting connections\n");
    exit(EXIT_FAILURE);
  }

  printf("Ready to accept connections...\n");
  // Accept connections
  while (true) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_socket;
    if ((client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len)) == -1) {
      perror("Failed to accept connection\n");
    }
    else {
      // Spawn thread to handle connection
      pthread_t thread;
      connection_args *args = (connection_args *) malloc(sizeof(connection_args));
      args->client_socket = client_socket;
      pthread_create(&thread, NULL, handleConnection, (void *) args);
    }
  }

  close(server_socket);
  return 0;
}


void *handleConnection(void *arg) {
  // Convert thread args
  connection_args *my_args = (connection_args *) arg;
  int client_socket = my_args->client_socket;
  free(my_args);

  // Read request from socket
  char buffer[1024];
  ssize_t bytes_read = recv(client_socket, buffer, 1024, 0);
  if (bytes_read < 0) {
    perror("Failed to read from socket");
    close(client_socket);
    pthread_exit(NULL);
  } 

  printf("Bytes read: %zd\n", bytes_read);
  if (bytes_read == 0) {
    printf("Debug: No data received or connection closed by client.\n\n");
    close(client_socket);
    pthread_exit(NULL);
  }
  
  buffer[bytes_read] = '\0';

  // Parse HTTP request
  HttpRequest request = parse_http_request(buffer);

  // Send Response
  char *response;
    if (strcmp(request.method, "GET") == 0) {
        // Handle GET request
        response = handleGetRequest(request);
    } else if (strcmp(request.method, "POST") == 0) {
        // Handle POST request
        response = handlePostRequest(request);
    } else if (strcmp(request.method, "DELETE") == 0) {
        // Handle DELETE request
        response = handleDeleteRequest(request);
    } else {
        response = create_http_response("405 Method Not Allowed", "Method Not Allowed");
    }

  ssize_t bytes_sent = send(client_socket, response, strlen(response), 0);
  if (bytes_sent < 0) {
      perror("send failed");
  } else if (bytes_sent != strlen(response)) {
      printf("Warning: Not all bytes were sent. Expected %zu, sent %zd\n", strlen(response), bytes_sent);
  } else {
      printf("Success: All bytes were sent correctly.\n");
  }

  free(response);

  // Exit Thread
  close(client_socket);
  printf("Thread ending...\n\n");
  pthread_exit(NULL);
}

int getHTMLFromFile(char **document, char *filePath) {
  FILE *file = fopen(filePath, "r");
  if (file == NULL) {
    perror("Failed to open file.\n");
    return 0;
  }

  int docSize = 2048;
  *document = (char *) calloc(docSize, sizeof(char));

  // Read from file line by line into document buffer
  char *line = NULL;
  size_t size = 0;
  int numBytesRead = 0;
  int totalBytesRead = 0;
  while ((numBytesRead = getline(&line, &size, file)) > 0) {
    totalBytesRead += numBytesRead + 1;

    // Increase size of document buffer if necessary
    while (totalBytesRead > docSize) {
      docSize *= 2;
      *document = realloc(*document, docSize);
    }

    // Fix carriage return on line from file to match HTML spec
    char *processedLine = malloc(strlen(line) + 2);
    strcpy(processedLine, line);

    int lastCharIdx = strlen(processedLine) - 1;
    processedLine[lastCharIdx] = '\r';
    processedLine[lastCharIdx+1] = '\n';
    processedLine[lastCharIdx+2] = '\0';

    // Add line 
    strcat(*document, processedLine);
    free(processedLine);
  }

  free(line);
  return totalBytesRead;
}

char *handleGetRequest(HttpRequest request) {
  // Serve HTML file for /pizza
  if (strcmp(request.uri, "/pizza") == 0) {
    char *document;
    int size = getHTMLFromFile(&document, "../frontend/pizza.html");
    if (size == 0) {
      return NULL;
    }

    char *response = create_http_response_HTML(document, size);

    free(document);
    return response;
  }
  // Otherwise default to todo list
  else {
    char response_content[4096] = "Current To-Do List:<br>";

    pthread_mutex_lock(&taskList.lock); // Lock the mutex
    // Add a script for the delete button to send HTTP DELETE request
    strcat(response_content, "<script>"
                          "function deleteTask(id) {"
                          "    var xhr = new XMLHttpRequest();"
                          "    var url = \"http://localhost:8080/delete?task=\" + id;"
                          "    xhr.open(\"DELETE\", url, true);"
                          "    xhr.onreadystatechange = function () {"
                          "        if (xhr.readyState === 4 && xhr.status === 200) {"
                          "            console.log(\"Task deleted successfully.\");"
                          "        } else {"
                          "            console.error(\"Failed to delete task.\");"
                          "        }"
                          "    };"
                          "    xhr.send();"
                          "    location.reload();"
                          "}"
                          "</script>");

    // Add li for each task in task list
    for (int i = 0; i < taskList.task_count; i++) {
      char li[50];
      sprintf(li, "<li id=\"%d\">", i + 1);
      strcat(response_content, li);
      strcat(response_content, taskList.tasks[i]);

      // Add a delete button for each task
      char deleteButton[100];
      sprintf(deleteButton, "<button onClick=\"deleteTask(%d)\">Delete</button>", i + 1);
      strcat(response_content, deleteButton);

      strcat(response_content, "</li>");
    }

    // Append form for adding new tasks
    strcat(response_content, "<form action=\"http://localhost:8080/post\" method=\"POST\">"
                          "<label for=\"task\">Enter Todo:</label><br>"
                          "<input type=\"text\" id=\"task\" name=\"task\"><br><br>"
                          "<input type=\"submit\" value=\"Submit\">"
                          "</form>");


    pthread_mutex_unlock(&taskList.lock); // Unlock the mutex
    return create_http_response("200 OK", response_content);
  }
}

char *handlePostRequest(HttpRequest request) {
  if (request.body == NULL) {
      return create_http_response("400 Bad Request", "No data in request body");
  }

  // Assume task is directly in the body; validate and add to list
  pthread_mutex_lock(&taskList.lock);
  if (taskList.task_count < MAX_TASKS) {
      strncpy(taskList.tasks[taskList.task_count], request.body, 255);
      taskList.tasks[taskList.task_count][255] = '\0'; // Ensure null-termination
      taskList.task_count++;
      pthread_mutex_unlock(&taskList.lock);

      char response_content[512];
      sprintf(response_content, "Task '%s' added successfully!", request.body);
      return create_http_response("200 OK", response_content);
  } else {
      pthread_mutex_unlock(&taskList.lock);
      return create_http_response("500 Internal Server Error", "Task list is full");
  }
}


char *handleDeleteRequest(HttpRequest request) {
    printf("Deleting\n");

    // Parse the task index from the URI
    int task_index;
    if (sscanf(request.uri, "/delete?task=%d", &task_index) != 1 || task_index < 1 || task_index > taskList.task_count) {
        return create_http_response("400 Bad Request", "Invalid task index");
    }

    // Adjust index for 0-based array access
    task_index--;

    pthread_mutex_lock(&taskList.lock);
    // Remove the task by shifting subsequent tasks down
    for (int i = task_index; i < taskList.task_count - 1; i++) {
        strcpy(taskList.tasks[i], taskList.tasks[i + 1]);
    }
    taskList.task_count--;
    pthread_mutex_unlock(&taskList.lock);

    return create_http_response("200 OK", "Task deleted successfully");
}

void add_task(const char* new_task) {
  pthread_mutex_lock(&taskList.lock); // Lock the mutex

  if (taskList.task_count < MAX_TASKS) {
      strncpy(taskList.tasks[taskList.task_count], new_task, 255);
      taskList.tasks[taskList.task_count][255] = '\0'; // Ensure null-termination
      taskList.task_count++;
  }

  pthread_mutex_unlock(&taskList.lock); // Unlock the mutex
}
