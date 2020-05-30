/*  Implement setenv() and unsetenv() using getenv(), putenv() and, where
    necessary code that directly modifies environ. Your version of unsetenv()
    should check to see whether there are multiple definitions of an
    environment variable, and remove them all (which is what the glibc version
    of unsetenv() does). */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char** environ;

char * get_left_side(char * variable) {

    // This function always assumes there will be a '=' in the string. No
    // checking.

    int i;
    for (i=0; variable[i] != '='; i++)
    ;

    // Account for zero-based index
    i++;

    char * left_side = malloc(i);
    snprintf(left_side, i, "%s", variable);
    return left_side;
}

char * get_right_side(char * variable) {

    int i;
    for (i=0; variable[i] != '='; i++)
    ;

    int j;
    for (j=i; variable[j] != '\0'; j++)
    ;

    i++;
    j++;

    int len = j-i;
    char * right_side = malloc(len+1);
    snprintf(right_side, len, "%s", variable+i);

    return right_side;

}

char * _getenv(const char * name) {

    for(int i=0; environ[i] != NULL; i++) {

        char * lhs = get_left_side(environ[i]);
        if (strcmp(name, lhs) == 0) {
            return get_right_side(environ[i]);
        }
    }
    return NULL;
}


int _unsetenv(const char * name) {

    char** environ_copy = NULL;
    int environ_copy_len = 0;

    for(int i=0; environ[i] != NULL; i++) {

        char * lhs = get_left_side(environ[i]);
        if (!(strcmp(name, lhs) == 0)) {
            //printf("Copying %s to new environment list\n", lhs);
            environ_copy = realloc(environ_copy, 
                                        (environ_copy_len+1) * sizeof(char*));
            environ_copy[environ_copy_len] = environ[i];
            environ_copy_len++;
        }
        else {
            //printf("NOT copying %s to new environment list\n", lhs);
        }
    }

    // Put final NULL in
    environ_copy = realloc(environ_copy, (environ_copy_len+1) * sizeof(char*));
    environ_copy[environ_copy_len] = NULL;
    environ = environ_copy;
    return 0;
}

void print_env() {

    for (int i=0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
}

int _setenv(const char *name, const char *value, int overwrite) {

    int name_copy_len = strlen(name);
    int value_copy_len = strlen(value);

    // Add 2 for the '=' and NULL
    int putenv_param_len = name_copy_len + value_copy_len + 2;

    char * putenv_param = malloc(putenv_param_len);
    int ret = snprintf(putenv_param, putenv_param_len, "%s=%s", name, value);
    if (ret < putenv_param_len -1 ) {
        perror("snprintf()");
        return 1;
    }

    if (_getenv(name) != NULL) {

        if (overwrite == 0) {
            // Already exists but overwrite = 0 means don't do anything
            return 0;
        }
        else {
            _unsetenv(name);
        }

    }

    ret = putenv(putenv_param);
    if (ret != 0) {
        perror("putenv()");
    }

    return 0;
}

int main() {

    printf("1. Setting variable MYVARIABLE1=MYVALUE1\n");
    _setenv("MYVARIABLE", "MYVALUE", 0);
    print_env();

    printf("================================================================\n");


    printf("2. Getting variable MYVARIABLE1=MYVALUE1 from environment list\n");
    char* v = _getenv("MYVARIABLE");
    printf("%s\n",v);

    printf("================================================================\n");

    printf("3. Setting variable MYVARIABLE1=MYNEWVALUE WITHOUT overwrite.\n");
    _setenv("MYVARIABLE", "MYNEWVALUE", 0);
    print_env();

    printf("================================================================\n");

    printf("4. Setting variable MYVARIABLE1=MYNEWVALUE WITH overwrite.\n");
    _setenv("MYVARIABLE", "MYNEWVALUE", 1);
    print_env();

    printf("================================================================\n");

    printf("5. Unsetting variable MYVARIABLE.\n");
    _unsetenv("MYVARIABLE");
    print_env();

    printf("================================================================\n");

}