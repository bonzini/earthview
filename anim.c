/* Animation module.
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
static int speed = 1;
static int set_to_localtime;

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
  set_to_localtime = 1;
}

void
do_anim (cairo_t *cairo_context, SDL_Event *event)
{
  if (event->type == SDL_KEYDOWN)
    {
      if (event->key.keysym.sym == SDLK_s)
        speed = (speed == 2 ? 0 : speed + 1);

      if (event->key.keysym.sym == SDLK_q)
        {
          event->type = SDL_QUIT;
          SDL_PushEvent (event);
        }

      if (event->key.keysym.sym == SDLK_EQUALS)
	init_anim ();
      if (event->key.keysym.sym == SDLK_RETURN)
	cur_time.day++;
    }

  switch (speed)
    {
    case 0:
      if (set_to_localtime)
        init_anim ();
      break;

    case 1:
      set_to_localtime = 0;
      cur_time.m++;
      if (cur_time.m == 60)
	cur_time.m = 0, cur_time.h++;
      if (cur_time.h == 24)
	cur_time.h = 0, cur_time.day++;
      break;

    case 2:
      set_to_localtime = 0;
      cur_time.day++;
      break;
    }
}
