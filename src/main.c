#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int check_path(const char *command_name);

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  char command[1024];
  while(1){
    printf("$ ");

    // Handle Input & EOF (Ctrl+D)
    if (fgets(command, sizeof(command), stdin) == NULL){printf("\n"); break;} 

    // Remove trailing newline
    command[strcspn(command, "\n")] = '\0';

    // EXIT
    if (strcmp(command, "exit") == 0 || strcmp(command, "exit 0") == 0){
      return 0;
    }

    // ECHO
    else if (strncmp(command, "echo ", 5) == 0) {
      printf("%s\n", command + 5);
    }

    //--- TYPE COMMAND UPDATED ---
    else if (strncmp(command, "type ", 5) == 0){
      char *arg = command + 5;

      // 1. Check Builtins
      if (strncmp(arg ,"echo", 4)==0 ||strncmp(arg ,"type", 4)==0 ||strncmp(arg ,"exit", 4)==0){
        printf("%s is a shell builtin\n", arg);
      }
      // 2. Check PATH
      else {
        if (!check_path(arg)) {
          printf("%s: not found\n", arg);
        }
      }
    }


    // COMMAND NOT FOUND
    else{
      printf("%s: command not found\n", command);
    }
  }
  return 0;
}

int check_path(const char *command_name) {
  char *path_env = getenv("PATH");
  if (path_env == NULL) return 0;

  // Duplicate the PATH string because strtok modifies it
  char *path_copy = strdup(path_env);
  
  // Split the path by ':'
  char *directory = strtok(path_copy, ":");

  while (directory != NULL) {
    // Allocate enough space for "directory/command" + null terminator
    // (strlen(directory) + 1 slash + strlen(command_name) + 1 null)
    char full_path[1024]; 
      
    // Construct the full path: directory + "/" + command_name
    snprintf(full_path, sizeof(full_path), "%s/%s", directory, command_name);

    // check if file exists and is executable
    if (access(full_path, X_OK) == 0) {
      printf("%s is %s\n", command_name, full_path);
      free(path_copy); 
      return 1;
    }

    // Move to the next directory in PATH
    directory = strtok(NULL, ":");
  }

  free(path_copy);
  return 0;
}