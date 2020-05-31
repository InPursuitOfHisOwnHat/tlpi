/*  Write a program to see what happens if we try to longjmp() into a function
    that has already returned

    There are two calls, here. One is normal and succeeds the other fails (as
    per the question.

    The answer is: It goes nuts and corrupts the stack frame but we already knew
    that :) */

#include <stdio.h>
#include <setjmp.h>


static jmp_buf env;

int func2() {

    printf("Arrived in func2()\n");
    longjmp(env,1);
    printf("Still in func2() - this shouldn't happen\n");
    return 1;

}

int func() {

    if (setjmp(env) > 0) {
        printf("Arrived back from a lngjmp() call\n");
        return 0;
    }
    else {
        printf("setjmp() called for the first time\n");
    }

    func2();
    return 0;
}


int func_fail() {

    if (setjmp(env) > 0) {
        printf("Arrived back from a lngjmp() call\n");
        return 0;
    }
    else {
        printf("setjmp() called for the first time\n");
    }
    return 0;
}

int main() {

    printf("Working jmp calls.");
    func();
    printf("Now failing jmp calls.");
    func_fail();
    longjmp(env,1);
}