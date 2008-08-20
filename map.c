/* Map drawing module skeleton.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "map.h"

static cairo_pattern_t *map;

void
init_map (void)
{
  cairo_surface_t *png_map = cairo_image_surface_create_from_png ("map.png");
  map = cairo_pattern_create_for_surface (png_map);
}

void
do_map (cairo_t *cairo_context, SDL_Event *event)
{
  cairo_save (cairo_context);
  cairo_set_source (cairo_context, map);
  cairo_paint (cairo_context);
  cairo_restore (cairo_context);
}
