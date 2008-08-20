/* Animation module skeleton.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "anim.h"
#include "drawing.h"

struct time cur_time;

void
init_anim (void)
{
  struct tm *tm;
  time_t t;

  t = time (NULL);
  tm = gmtime (&t);
  cur_time.year = tm->tm_year + 1900;
  cur_time.month = tm->tm_mon + 1;
  cur_time.day = tm->tm_mday;
  cur_time.h = tm->tm_hour;
  cur_time.m = tm->tm_min;
}

void
do_anim (cairo_t *cairo_context, SDL_Event *event)
{
}
