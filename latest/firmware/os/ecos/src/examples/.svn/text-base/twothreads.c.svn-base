#include <cyg/kernel/kapi.h>
#include <cyg/io/devtab.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* now declare (and allocate space for) some kernel objects,
   like the two threads we will use */
cyg_thread thread_s[2];		/* space for two thread objects */

char stack[2][4096];		/* space for two 4K stacks */

/* now the handles for the threads */
cyg_handle_t simple_threadA, simple_threadB;

/* and now variables for the procedure which is the thread */
cyg_thread_entry_t simple_program;

/* and now a mutex to protect calls to the C library */
cyg_mutex_t cliblock;

/* we install our own startup routine which sets up threads */
void cyg_user_start(void)
{
  watchdogInt();

  cyg_mutex_init(&cliblock);

  cyg_thread_create(4, simple_program, (cyg_addrword_t) 0,
		    "Thread A", (void *) stack[0], 4096,
		    &simple_threadA, &thread_s[0]);
  cyg_thread_create(4, simple_program, (cyg_addrword_t) 1,
		    "Thread B", (void *) stack[1], 4096,
		    &simple_threadB, &thread_s[1]);

  cyg_thread_resume(simple_threadA);
  //cyg_thread_resume(simple_threadB);
}

char string[2][80] = {"88888888888888888888888888888888888888888",
		 "99999999999999999999999999999999999999999"};
char line_buffer[81];

/* this is a simple program which runs in a thread */
void simple_program(cyg_addrword_t data)
{
  static int counters[2] = {1, 2};
  int message = (int) data;
  int delay, i = 0;

  printf("Beginning execution; thread data is %d\n", message);

  //cyg_thread_delay(200);

  for (;;) {
#if 0
    counters[message]++;
    delay = 200 + (rand() % 50);

    /* note: printf() must be protected by a
       call to cyg_mutex_lock() */
      cyg_mutex_lock(&cliblock); {
	printf("Thread %d: and now a delay of %d clock ticks\n",
		    message, delay);
	printf("%s\n", string[message]);
      }
      cyg_mutex_unlock(&cliblock);
      cyg_thread_delay(delay);
#endif
      delay = 200 + (rand() % 50);
      fgets(line_buffer, 80, stdin);
      printf("%s%i@%d > ", line_buffer, i++, cyg_current_time());
  }
}

cyg_interrupt  wd_interrupt;
cyg_handle_t   wd_interrupt_handle;

static cyg_uint32 wd_ISR(cyg_vector_t vector, cyg_addrword_t data) {

    //cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_WATCHDOG);
    //cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_WATCHDOG);
    *((unsigned long*)0xbf000004) = 0x5678; //hack, kick the wd

    return CYG_ISR_CALL_DSR;
}

static void wd_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
    //cyg_drv_interrupt_unmask(vector);
}

void 
watchdogInt(void) {
        cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_WATCHDOG,
                                 99,
                                 (cyg_addrword_t)0,
                                 wd_ISR,
                                 wd_DSR,
                                 &wd_interrupt_handle,
                                 &wd_interrupt);
        cyg_drv_interrupt_attach(wd_interrupt_handle);
        cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_WATCHDOG);
}
