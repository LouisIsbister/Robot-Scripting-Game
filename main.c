#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFF_SIZE 1000
#define MAX_SCRIPTS 20

char* scripts[MAX_SCRIPTS];

/**
 * load in script file names from the src/scripts directory
 * store the file names in the scripts array
 * 
 * @return the number of scripts loaded
 */
uint8_t load_scripts() {
    FILE *file;
    char buffer[BUFF_SIZE];

    file = popen("ls scripts", "r");
    if (file == NULL) {
        perror("Failed to list the scripts.");
        return 0;
    }

    uint8_t ptr = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && ptr < MAX_SCRIPTS) {
        scripts[ptr] = (char *) malloc(sizeof(buffer) + 1);
        if (scripts[ptr] == NULL) {
            perror("Bad memory alloction.");
            return 0;
        }
        
        strcpy(scripts[ptr], buffer);
        size_t len = strlen(scripts[ptr]);
        scripts[ptr][len - 1] = '\0';

        ptr++;
    }
    pclose(file);

    // display scripts to the user
    printf(" Scripts:\n---------- \n");
    for (int i = 0; i < ptr; i++) {
        printf("%s\n", scripts[i]);
    }

    return ptr;
}

/**
 * print all the contents of a script to the user if they have 
 * provided the cat command.
 * 
 * @param fname, the name of the script whose contents is being displayed
 */
void print_script_contents(const char* fname) {
    char command[100]; 
    sprintf(command, "cat scripts/%s", fname);

    FILE *fptr = popen(command, "r");
    if (fptr == NULL) {
        perror("Failed to 'cat' the desired script.");
        return;
    }

    printf("\nContents of '%s':\n", fname);
    char buffer[BUFF_SIZE];
    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        printf("%s", buffer);
    }
    pclose(fptr);
}

/**
 * check whether the user entered a valid script 
 * 
 * @param script, the string entered by user
 * @param len, the number of valid scripts
 * @return whether the script is within the scripts array 
 */
uint8_t valid_script(const char* script, uint8_t len) {
    for (int i = 0; i < len; i++) {
        if (strcmp(scripts[i], script) == 0) {
            return 0;
        }
    }
    return 1;
}

/**
 * ask the user to provide a script to execute, loop
 * until a valid one is provided.
 * 
 * @param len, the number of valid scripts
 * @return the script selected by the user
 */
char* get_user_script(uint8_t len) {
    printf("\nEnter a script to read from or execute.\n");
    printf("To read you can say 'cat <script name>'\n");
    printf("To execute just provide '<script name>'\n");

    char* buffer = (char *)malloc(100 * sizeof(char));
    while (1) {
        printf("\n > ");

        fgets(buffer, 100, stdin);
        size_t scr_len = strlen(buffer);
        if (buffer[scr_len - 1] == '\n') {    // remove the newline if present
            buffer[scr_len - 1] = '\0';
        }

        // if the user wants to view the script contents
        if (strncasecmp(buffer, "cat ", 4) == 0) {
            char* fname = buffer + 4;
            if (valid_script(fname, len) == 0) {
                print_script_contents(fname);
            } else {
                printf("cannot read from '%s' as it does not exist!\n", fname);
            }
        } else if (valid_script(buffer, len) == 0) {
            printf("Success! %s will be executed.\n", buffer); 
            break;
        } else {
            printf("Invalid script! You entered: '%s'\n", buffer);
        }
    }

    // scripts are no longer required
    for (int i = 0; i < len; i++) {
        free(scripts[i]);
    }

    return buffer;
}

int main(int argc, char** argv) {
    uint8_t num_scripts = load_scripts();
    char* user_script = get_user_script(num_scripts);

    // execute script to do

    free(user_script);
    return 0;
}
