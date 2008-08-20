/* Sunrise and sunset computation utilities.
   Paolo Bonzini, August 2008.

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#ifndef SUNRISE_H
#define SUNRISE_H

/* Eastern longitude positive, Western longitude negative.
   Northern latitude positive, Southern latitude negative.  */

extern double calc_day_length (int, int, int, double, double, double, int);
extern int calc_sun_rise_set (int, int, int, double, double, double, int,
			      double *, double *);

/* This function returns whether it is spring or summer in the northern
   (if return value is 1) or southern (if return value is 0) hemisphere
   at the given time.  */

static inline int north_emisphere_summer (int y, int m, int d)
{
  return calc_day_length (y, m, d, 0.0, 50.0, 0.0, 0) > 12;
}

/* This function computes the length of the day, from sunrise to sunset.
   Sunrise/set is considered to occur when the Sun's upper limb is
   35 arc minutes below the horizon (this accounts for the refraction
   of the Earth's atmosphere).  */
static inline double
day_length (int year, int month, int day, double lon, double lat)
{
  return calc_day_length (year, month, day, lon, lat, -35.0/60.0, 1);
}

/* This function computes the length of the day, including civil twilight.
   Civil twilight starts/ends when the Sun's center is 6 degrees below
   the horizon.  */
static inline double
day_length_civil (int year, int month, int day, double lon, double lat)
{
  return calc_day_length (year, month, day, lon, lat, -6.0, 0);
}

/* This function computes the length of the day, including astronomical
   twilight.  Astronomical twilight starts/ends when the Sun's center is
   18 degrees below the horizon.  */
static inline double
day_length_astro (int year, int month, int day, double lon, double lat)
{
  return calc_day_length (year, month, day, lon, lat, -18.0, 0);
}

/* This function computes times for sunrise/sunset.
   Return value is 0 usually, i.e. if sun rises/sets; times are stored at
   *RISE and *SET.

   A return value of 1 means the sun is above the specified "horizon" all
   24 hours. The values stored in *RISE and *SET are *not* 0 and 24!!
   *RISE is set to the time when the sun is at south, minus 12 hours while
   *SET is set to the south time plus 12 hours.

   -1 instead means the sun is below the specified "horizon" 24 hours;
   *RISE and *SET are both set to the time when the sun is at south.

   Sunrise/set is considered to occur when the Sun's upper limb is
   35 arc minutes below the horizon (this accounts for the refraction
   of the Earth's atmosphere).  */
static inline int
sun_rise_set (int year, int month, int day, double lon, double lat,
	      double *rise, double *set)
{
  return calc_sun_rise_set (year, month, day, lon, lat, -35.0/60.0, 1,
			    rise, set);
}

/* This function computes the start and end times of civil twilight.
   Civil twilight starts/ends when the Sun's center is 6 degrees below
   the horizon.  */
static inline int
civil_rise_set (int year, int month, int day, double lon, double lat,
	        double *rise, double *set)
{
  return calc_sun_rise_set (year, month, day, lon, lat, -6.0, 0,
			    rise, set);
}

/* This function computes the start and end times of astronomical twilight.
   Astronomical twilight starts/ends when the Sun's center is 18 degrees
   below the horizon.  */
static inline int
astro_rise_set (int year, int month, int day, double lon, double lat,
	        double *rise, double *set)
{
  return calc_sun_rise_set (year, month, day, lon, lat, -18.0, 0,
			    rise, set);
}


#endif
