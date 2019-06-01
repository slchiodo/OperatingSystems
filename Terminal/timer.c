/* calculate running and crrent time */

#include "timer.h"
#define MAX_TIME 25

/* 
Purpose: get the current time
Input: current time string
Output: current time
*/
void get_time( char* *curr_time ) {
    struct timeval t;
    char s[MAX_TIME];
    gettimeofday (&t, NULL);
    struct tm *tm = localtime(&t.tv_sec);
    strftime(s, sizeof(s), "%I:%M%p", tm);
    strcpy(*curr_time, s);
}

/* 
Purpose: returns time in seconds
Input: current time
Output: current time in seconds
*/
double run_time(char* curr_time) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}



