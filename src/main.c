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
    else{
      // Prints the "<command>: command not found" message
      printf("%s: command not found\n", command);
    }

  }
 

  return 0;
}
