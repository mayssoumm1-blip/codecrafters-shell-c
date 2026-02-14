#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

// Helper to find the full path of a command
char* get_command_path(const char *command_name);


int main() {
  setbuf(stdout, NULL);
  char input[1024];

  while (1) {
    printf("$ ");
    if (fgets(input, sizeof(input), stdin) == NULL) break;
    input[strcspn(input, "\n")] = '\0';

    char *args[128];
    int arg_count = 0;

    int n = strlen(input);
    for(int i = 0; i<n; i++){
      while(i<n && isspace(input[i])) i++;
      if (i >= n) break;

      char *arg = malloc(1024);
      int k = 0;
      int in_single_quotes = 0;

      while (i<n){
        if (input[i] == '\''){
          in_single_quotes = !in_single_quotes; // Toggle state
        } else if (!in_single_quotes && isspace(input[i])){
          break;  // Ende of argument
        } else {
          arg[k++] = input[i];  // Literal character
        }
        i++;
      }
      arg[k] = '\0';
      args[arg_count] = arg;
    }
    args[arg_count] = NULL;
   
    if (arg_count == 0) continue; // Handle empty input

    // -- BUILTINS --

    // --EXIT--
    if (strcmp(args[0], "exit") == 0) return 0;

    // --ECHO--
    else if (strcmp(args[0], "echo") == 0) {
      for (int j = 1; j < arg_count; j++) printf("%s%s", args[j], (j == arg_count - 1) ? "" : " ");
      printf("\n");
    } 

    // --PWD--
    else if(strcmp(args[0], "pwd") == 0){
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s\n", cwd);
      }
      else{
        perror("pwd error");
      }
    }

    // --CD BUILTIN--
    else if(strcmp(args[0], "cd") == 0){
      char *path = (arg_count > 1) ? args[1] : NULL;
        if (path && strcmp(path, "~") == 0) path = getenv("HOME");
        if (path && chdir(path) != 0) printf("cd: %s: No such file or directory\n", args[1]);
    }

    // --TYPE--
    else if (strcmp(args[0], "type") == 0) {                       
      if (arg_count < 2) continue;                                                  
      char *t = args[1];                                  
      if (strcmp(t, "exit") == 0 || 
          strcmp(t, "echo") == 0 ||
          strcmp(t, "type") == 0 ||
          strcmp(t, "pwd") == 0 ||
          strcmp(t, "cd") == 0) {                          
        printf("%s is a shell builtin\n", t);                
      } else {                                                     
        // Check if it's an external command 
        char *path = get_command_path(t); 
        if (path) { printf("%ss is %s\n", t, path); free(path);}  
        else printf("%s: not found \n", t);                                                       
      }                                                            
    }    
        
    // -- EXTERNAL PROGRAMS --
    else {
      char *full_path = get_command_path(args[0]);
      if (full_path) {
        if (full_path) {
          if(fork() == 0){
            execv(full_path, args);
            exit(1);
          }else{
            wait(NULL);
            free(full_path);
          }
        } else {
          printf("%s: command not found\n", args[0]);
        }
      }
      // Clean up memory for arguments
      for (int j = 0; j < arg_count; j++) free(args[j]);
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
    char *full_path = malloc(strlen(dir) + strlen(command_name) + 2);
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