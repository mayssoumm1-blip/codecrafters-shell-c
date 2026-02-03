#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Helper to find the full path of a command
char* get_command_path(const char *command_name);


int main() {
  setbuf(stdout, NULL);
  char input[1024];

  while (1) {
    printf("$ ");
    if (fgets(input, sizeof(input), stdin) == NULL) break;
    input[strcspn(input, "\n")] = '\0';

    // Tokenize input into arguments
    char *args[128];
    int i = 0;
    char *token = strtok(input, " ");
    while (token != NULL) {
      args[i++] = token;
      token = strtok(NULL, " ");
    }
    args[i] = NULL; // Array must be NULL-terminated for execv

    if (args[0] == NULL) continue; // Handle empty input

    // 2. Handle Builtins
    if (strcmp(args[0], "exit") == 0) return 0;
        
    else if (strcmp(args[0], "echo") == 0) {
      for (int j = 1; j < i; j++) printf("%s%s", args[j], (j == i - 1) ? "" : " ");
      printf("\n");
    } 
        
    // 3. Handle External Programs
    else {
      char *full_path = get_command_path(args[0]);
      if (full_path) {
        pid_t pid = fork(); // Create child process
        if (pid == 0) {
          // This is the CHILD process
          execv(full_path, args);
          // If execv returns, it failed
          perror("execv");
          exit(1);
        } 
        else {
          // This is the PARENT (your shell)
          wait(NULL); // Wait for the child to finish
          free(full_path);
        }
      } 
      else {
        printf("%s: command not found\n", args[0]);
      }
    }
  }
  return 0;
}

char* get_command_path(const char *command_name){
  char *path_env = getenv("PATH");
  if(!path_env) return NULL;

  char *path_copy = strdup(path_env);
  char *dir = strtok(path_copy, ":");

  while (dir != NULL){
    char *full_path = malloc(srtlen(dir) + strlen(command_name) + 2);
    sprintf(full_path, "%s/%s", dir, command_name );

    if (access(full_path, X_OK)==0){
      free(path_copy);
      return full_path;
    }
    free(full_path);
    dir = strtok(NULL, ":");
  }
  free(path_copy);
  return NULL;
}