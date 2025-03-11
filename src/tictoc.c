/*
  TITLE: 
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-03-05 18:05 emilio on emilio-XPS-15-9570>
*/

#include "tictoc.h"
// gcc -o timer timer.c -lrt

#define MAX_TIMERS 1000

typedef struct {
  struct timespec start;
  char name[50];
  int active;
} Timer;

static Timer timers[MAX_TIMERS];
static int timer_tic0 =0;

// Initialize a new timer
int tic_named(const char* timer_name) {

  int num_timers1=0;
  while(num_timers1 < MAX_TIMERS && timers[num_timers1].active!=0){
	num_timers1++;
  }
  if (num_timers1 >= MAX_TIMERS) {
	Rf_warning("Error: Maximum number of timers reached (%d)\n",MAX_TIMERS);
	return -1;
  }

  int num_timers = num_timers1;
  strncpy(timers[num_timers].name, timer_name, 49);
  timers[num_timers].name[49] = '\0';
  timers[num_timers].active = 1;
  clock_gettime(CLOCK_MONOTONIC, &timers[num_timers].start);
   return num_timers;
}

// Simple tic without name
void tic() {
  timer_tic0 = tic_named("default");
}

// Get elapsed time for a specific timer
double toc_timer(int timer_id) {
    if (timer_id < 0 || timer_id >= MAX_TIMERS || !timers[timer_id].active) {
	Rf_warning("Error: Invalid timer ID (%d)\n",timer_id);
	return -1.0;
  }

  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);

  double elapsed = (end.tv_sec - timers[timer_id].start.tv_sec) +
	(end.tv_nsec - timers[timer_id].start.tv_nsec) / 1e9;


  //timers[timer_id].name[0] = '\0';
  timers[timer_id].active = 0;
  
  return elapsed;
}

// Print elapsed time for a specific timer

void toc_print_timer(int timer_id) {
  double elapsed = toc_timer(timer_id);
  if (elapsed >= 0) {
	if (elapsed >= 3600.0) {
	  // Show in hours, minutes, and seconds if >= 1 hour
	  int hours = (int)(elapsed / 3600.0);
	  int minutes = (int)((elapsed - (hours * 3600.0)) / 60.0);
	  double seconds = elapsed - (hours * 3600.0) - (minutes * 60.0);
	  Rprintf("Timer '%s' elapsed time: %dh %dm %.3fs\n", 
			 timers[timer_id].name, hours, minutes, seconds);
	}
	else if (elapsed >= 60.0) {
	  // Show in minutes and seconds if >= 1 minute
	  int minutes = (int)(elapsed / 60.0);
	  double seconds = elapsed - (minutes * 60.0);
	  Rprintf("Timer '%s' elapsed time: %dm %.3fs\n", 
			 timers[timer_id].name, minutes, seconds);
	}
	else if (elapsed >= 1.0) {
	  // Show in seconds if >= 1 second
	  Rprintf("Timer '%s' elapsed time: %.3fs\n", 
			 timers[timer_id].name, elapsed);
	}
	else if (elapsed >= 0.001) {
	  // Show in milliseconds if >= 1 millisecond
	  Rprintf("Timer '%s' elapsed time: %.2fms\n", 
			 timers[timer_id].name, elapsed * 1000.0);
	}
	else {
	  // Show in microseconds if < 1 millisecond
	  Rprintf("Timer '%s' elapsed time: %.2fµs\n", 
			 timers[timer_id].name, elapsed * 1000000.0);
	}
  }
}

// Simple toc for default timer
double toc() {
    return toc_timer(timer_tic0);
}

// Example usage
int maintictoc() {
  int i;

  // Using default timer
  tic();
  for(i = 0; i < 100000000; i++) { /* waste time */ }
  Rprintf("Default timer: %.6f seconds\n", toc());

  // Using named timers
  int timer1 = tic_named("Timer 1");
  for(i = 0; i < 100000000; i++) { /* waste time */ }
  toc_print_timer(timer1);

  int timer2 = tic_named("Timer 2");
  for(i = 0; i < 200000000; i++) { /* waste time */ }
  toc_print_timer(timer2);

  return 0;
}

/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
