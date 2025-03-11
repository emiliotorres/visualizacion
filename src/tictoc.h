#ifndef TICTOC_H
#define TICTOC_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <R.h>

// gcc -o timer timer.c -lrt


void tic();
double toc();
int tic_named(const char* timer_name);
double toc_timer(int timer_id);
void toc_print_timer(int timer_id);

#endif
