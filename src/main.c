#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  while(1){
    // Flush after every printf
    setbuf(stdout, NULL);
    printf("$ ");

    // Captures the user's command in the "command" variable
    char command[1024];
    fgets(command,sizeof(command), stdin);

    // Remove the trailing newline
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "exit") == 0){
      return 0;
    }

    // Check if command starts with "echo "
    else if (strncmp(command, "echo ", 5) == 0) {
      // Print everything starting from index 5 (after "echo ")
      printf("%s\n", command + 5);
    }

    // Check if command starts with type
    else if (strncmp(command, "type ", 5) == 0){
      if (strncmp(command+5 ,"echo", 4)==0 ||strncmp(command+5 ,"type", 4)==0 ||strncmp(command+5 ,"exit", 4)==0){
        printf("%s is a shell builtin\n", command+5);
      }
      else {
        printf("%s: not found\n", command+5);
      }
    }

    else{
      // Prints the "<command>: command not found" message
      printf("%s: command not found\n", command);
    }

  }
 

  return 0;
}
