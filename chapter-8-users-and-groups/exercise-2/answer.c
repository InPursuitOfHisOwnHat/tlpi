/*  Implement getpwnam() using setpwent(), getpwent() and endpwent() */

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>

struct passwd* _getpwnam(const char * name) {

    struct passwd* p;
    setpwent();
    while((p = getpwent()) != NULL) {
        if ( strcmp(name, p->pw_name) == 0) {
            endpwent();
            return p;
        }
    }
    endpwent();
    return NULL;

}

// struct passwd {
//     char   *pw_name;       /* username */
//     char   *pw_passwd;     /* user password */
//     uid_t   pw_uid;        /* user ID */
//     gid_t   pw_gid;        /* group ID */
//     char   *pw_gecos;      /* user information */
//     char   *pw_dir;        /* home directory */
//     char   *pw_shell;      /* shell program */
// };

int main(int argc, char** argv) {

    struct passwd* p;
    p = _getpwnam(argv[1]);

    if (argc < 2) {
        printf("Usage: getpwnam <username>\n");
        exit(EXIT_FAILURE);
    }

    if (p != NULL) {
        printf("Retrieved:\n");
        printf("\tpw_name: %s\n", p->pw_name);
        printf("\tpw_passwd: %s\n", p->pw_passwd);
        printf("\tpw_uid: %d\n", p->pw_uid);
        printf("\tpw_gid: %d\n", p->pw_gid);
        printf("\tpw_gecos: %s\n", p->pw_gecos);
        printf("\tpw_dir: %s\n", p->pw_dir);
        printf("\tpw_shell: %s\n", p->pw_shell);
    }
    else {
        printf("Could not find user: %s\n", argv[1]);
    }

}