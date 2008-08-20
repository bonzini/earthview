/* Map drawing module.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "drawing.h"
#include "sunrise.h"
#include "project.h"
#include "map.h"
#include "anim.h"


/* Marching squares implementation.  */

enum dir {
  UP, LT, DN, RT
};

/* Bit 3 = top left
   bit 2 = top right
   bit 1 = bottom left
   bit 0 = bottom right.  */

static const int next_square[16] = {
  RT, /* | | | */		DN, /* | |.| */
  LT, /* |.| | */		LT, /* |.|.| */
  RT, /* | |'| */		DN, /* | |:| */
  LT, /* |.|'| */		LT, /* |.|:| */
  UP, /* |'| | */		UP, /* |'|.| */
  UP, /* |:| | */		UP, /* |:|.| */
  RT, /* |'|'| */		DN, /* |'|:| */
  RT, /* |:|'| */		RT  /* |:|:| */
};

static inline int shift (enum dir d, int x)
{
  switch (d)
    {
    case UP: return (x & 0xC) >> 2;
    case DN: return (x & 0x3) << 2;
    case LT: return (x & 0xA) >> 1;
    case RT: return (x & 0x5) << 1;
    default: abort ();
    }
}


typedef int sun_rise_set_fn (int, int, int, double, double, double *, double *);

static inline int has_daylight (int x, int y, void *data)
{
  sun_rise_set_fn *f = (sun_rise_set_fn *) data;
  double lon = project_x (x);
  double lat = project_y (y);
  double rise, set;
  double hm = cur_time.h + cur_time.m / 60.0;

  switch (f (cur_time.year, cur_time.month, cur_time.day, lon, lat, &rise, &set))
    {
    case -1:
      return 0;
    case 1:
      return 1;
    default:
      return ((hm > rise && hm < set)
	      || (hm + 24 > rise && hm + 24 < set)
	      || (hm - 24 > rise && hm - 24 < set));
    }
}

static int inside (int x, int y, void *data)
{
  if (y < -5 || y >= WIN_HEIGHT + 5 || x < -5 || x >= WIN_WIDTH + 5)
    return 0;

  if (y < 0)
    y = 0;
  if (y >= WIN_HEIGHT)
    y = WIN_HEIGHT - 1;

  if (x < 0)
    x = 0;
  if (x >= WIN_WIDTH)
    x = WIN_WIDTH - 1;

  return has_daylight (x, y, data);
}

#define BAD -151515151

static int marching_squares (cairo_t *cairo_context, int x, int y,
			     int (*fn) (int, int, void *), void *data)
{
  int unknown_points = 15;
  int inside_points = 0;
  int hit = 0, startx = BAD, starty = BAD;
  int went_inside = 0;

  for (;;)
    {
      enum dir dir;

      /* Bit 3 = top left
         bit 2 = top right
         bit 1 = bottom left
         bit 0 = bottom right.  */
      if ((unknown_points & 1) && fn (x + 1, y + 1, data))
	inside_points |= 1;
      if ((unknown_points & 2) && fn (x, y + 1, data))
	inside_points |= 2;
      if ((unknown_points & 4) && fn (x + 1, y, data))
	inside_points |= 4;
      if ((unknown_points & 8) && fn (x, y, data))
	inside_points |= 8;

      if (hit || (inside_points != 0 && inside_points != 15))
        {
	  if (x >= 0 && x < WIN_WIDTH && y >= 0 && y < WIN_HEIGHT)
	    went_inside = 1;

	  if (!hit)
	    {
	      startx = x, starty = y;
	      cairo_move_to (cairo_context, x, y);
	    }
	  else
	    {
	      if (startx == x && starty == y)
	        break;
	      cairo_line_to (cairo_context, x, y);
	    }
	  hit = 1;
	}

      dir = next_square[inside_points];
      switch (dir)
	{
	case UP: y--; break;
	case DN: y++; break;
	case LT: x--; break;
	case RT: x++; break;
	}

      unknown_points = shift (dir, 15) ^ 15;
      inside_points = shift (dir, inside_points);
    }

  return went_inside;
}


static cairo_pattern_t *map;

void
init_map (void)
{
  cairo_surface_t *png_map = cairo_image_surface_create_from_png ("map.png");
  map = cairo_pattern_create_for_surface (png_map);
}

void
render_map_marching_squares (cairo_t *cairo_context, sun_rise_set_fn *f,
			     double alpha)
{
  cairo_save (cairo_context);
  cairo_new_path (cairo_context);

  /* This is a little hackish.  Sometime the civil twilight's shape is
     a rectangle with a "hole" in it.  In this case, several interesting
     things happen:

     1) we know that the first marching squares pass will trace a rectangle,
        because it will just go around the borders

     2) however, we know that we can trace the hole, if we start *inside*
        the map (i.e.  inside the lit part!) and we don't consider the
        borders at all.

     3) we know that both the rectangle and the hole are traced in the
        same direction, so that the if we trace both the even/odd rule
        will indeed generate a rectangle with a hole:

                             .----.
                            |2222222|
                 .----------+-2-2-2-+----.
                 |1111111111|2222222|1111.
                 |1111111111|2222222|1111.
                 |1111111111'-------'1111.
                 |11111111111111111111111.
                 '-----------------------'

     So marching_squares returns false if it never enters the map, and in
     this case we call it again with a slightly different function that will
     trace the inside shape without regards for border.  */

  if (!marching_squares (cairo_context, -60, project_lat (0.0),
			 inside, (void *) f))
    marching_squares (cairo_context, 3, project_lat (0.0),
		      has_daylight, (void *) f);

  cairo_close_path (cairo_context);
  cairo_clip (cairo_context);
  cairo_paint_with_alpha (cairo_context, alpha);
  cairo_restore (cairo_context);
}

void
do_map (cairo_t *cairo_context, SDL_Event *event)
{
  cairo_save (cairo_context);
  cairo_set_source_rgba (cairo_context, 0.0, 0.0, 0.0, 1.0);
  cairo_paint (cairo_context);

  cairo_set_source (cairo_context, map);
  cairo_set_operator (cairo_context, CAIRO_OPERATOR_ADD);
  cairo_paint_with_alpha (cairo_context, 0.5);

  render_map_marching_squares (cairo_context, sun_rise_set, 0.25);
  render_map_marching_squares (cairo_context, civil_rise_set, 0.25);
  cairo_restore (cairo_context);
}
