/* Map projection skeleton.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#ifndef PROJECT_H
#define PROJECT_H

static inline int project_long (double lon)
{
  lon += 180.0;
  while (lon > 360.0)
    lon -= 360.0;
  if (lon < 0.0)
    lon += 360.0;
  return lon;
}

static inline int project_lat (double lat)
{
  return lat;
}

static inline double project_x (int x)
{
  return x;
}

static inline double project_y (int y)
{
  return y;
}

#endif
