/* Sunrise and sunset computation utilities - test program.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "sunrise.h"

int
main (void)
{
  int year, month, day;
  double lon, lat;
  double len, rise, set, cur_time;
  int rc;
  char ch;
  time_t t;
  struct tm *tm;

  t = time (NULL);
  tm = gmtime (&t);
  lon = 0;

  sun_rise_set (tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		lon, 0.0, &rise, &set);
  cur_time = tm->tm_hour + tm->tm_min / 60.0;
  lon += (rise - cur_time) * 15.0;
  if (lon > 180.0)
    lon -= 360.0;
  if (lon < -180.0)
    lon += 360.0;

  printf ("==> Sun is rising at longitude %5.2fE, latitude 0.00N\n", lon);

  printf ("Longitude (+ is east) and latitude (+ is north), or * for Lugano: ");
  ch = getchar ();
  if (ch == '*')
    lon = 8.0 + 57.0/60.0, lat = 46.0;
  else
    {
      ungetc (ch, stdin);
      while (scanf ("%lf %lf", &lon, &lat) < 2)
	;
    }

  tm = localtime (&t);
  year = tm->tm_year + 1900;
  month = tm->tm_mon + 1;
  day = tm->tm_mday;
  do
    {
      len = day_length (year, month, day, lon, lat);
      printf ("==> Day length: %5.2f hours\n", len);

      rc = sun_rise_set (year, month, day, lon, lat, &rise, &set);
      if (rc == 1)
	printf ("==> Sun never sets!\n");
      else if (rc == -1)
	printf ("==> Sun never rises!\n");
      else
	{
	  printf ("==> Midday: %5.2f GMT\n", (rise + set) / 2);
	  printf ("==> Sunrise: %5.2f, Sunset: %5.2f GMT\n", rise, set);
	}

      rc = civil_rise_set (year, month, day, lon, lat, &rise, &set);
      if (rc == 1)
	printf ("==> Sun never reaches civil dusk!\n");
      else if (rc == -1)
	printf ("==> Sun never reaches civil dawn!\n");
      else
	printf ("==> Civil sunrise: %5.2f, Sunset: %5.2f GMT\n", rise, set);

      printf ("Date (yyyy mm dd) (ctrl-C exits): ");
      while (scanf ("%d %d %d", &year, &month, &day) < 3 && !feof (stdin))
	;
    }
  while (!feof (stdin));
  return 0;
}
