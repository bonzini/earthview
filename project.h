/* Map projection.
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
  return lon / 360.0 * WIN_WIDTH;
}

static inline int project_lat (double lat)
{
  if (lat > 90.0)
    lat = 90.0;
  if (lat < -90.0)
    lat = -90.0;
  return (90.0 - lat) / 180.0 * WIN_HEIGHT;
}

static inline double project_x (int x)
{
  double lon = x * 360.0 / WIN_WIDTH - 180.0;
  if (lon < 0.0)
    lon += 360.0;
  return lon;
}

static inline double project_y (int y)
{
  return 90.0 - y * 180.0 / WIN_HEIGHT;
}

#endif
