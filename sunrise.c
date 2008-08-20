/* Sunrise and sunset computation utilities.
   Copyright (c) 2006, Mark S. Teel (mark@teel.ws)

   This source code is released for free distribution under the terms
   of the GNU General Public License.  */

#include <math.h>

/* Trigonometric functions in degrees.  */

#define DEG_RAD         (M_PI / 180.0)
#define RAD_DEG         (180.0 / M_PI)

#define sind(x)         sin((x) * DEG_RAD)
#define cosd(x)         cos((x) * DEG_RAD)
#define tand(x)         tan((x) * DEG_RAD)

#define atand(x)        (RAD_DEG * atan(x))
#define asind(x)        (RAD_DEG * asin(x))
#define acosd(x)        (RAD_DEG * acos(x))
#define atan2d(y,x)     (RAD_DEG * atan2(y,x))


/* This function reduces any angle to within the first revolution by subtracting
   or adding even multiples of 360.0 until the result is >= 0.0 and < 360.0.  */
static double
normalize (double x)
{
  return (x - 360.0 * floor (x / 360.0));
}

static double
normalize180 (double x)
{
  return (x - 360.0 * floor (x / 360.0 + 0.5));
}

/* Computes the Sun's ecliptic longitude and distance at an instant given in d,
   number of days since 1/1/2000. The Sun's ecliptic latitude is not computed,
   since it's always very near 0.  */
static void
calc_sun_position (double d, double *lon, double *r)
{
  double M,			/* Mean anomaly of the Sun */
    w,				/* Mean longitude of perihelion */
    /* Note: Sun's mean longitude = M + w */
    e,				/* Eccentricity of Earth's orbit */
    E,				/* Eccentric anomaly */
    x, y,			/* x, y coordinates in orbit */
    v;				/* True anomaly */

  /* Compute mean elements */
  M = normalize (356.0470 + 0.9856002585 * d);
  w = 282.9404 + 4.70935E-5 * d;
  e = 0.016709 - 1.151E-9 * d;

  /* Compute true longitude and radius vector */
  E = M + e * RAD_DEG * sind (M) * (1.0 + e * cosd (M));
  x = cosd (E) - e;
  y = sqrt (1.0 - e * e) * sind (E);
  *r = sqrt (x * x + y * y);	/* Solar distance */
  v = atan2d (y, x);		/* True anomaly */
  *lon = v + w;			/* True solar longitude */
  if (*lon >= 360.0)
    *lon -= 360.0;		/* Make it 0..360 degrees */
}

static void
calc_sun_ra_and_decl (double d, double *RA, double *sdec, double *cdec, double *r)
{
  double lon, obl_ecl, x, y, z;

  /* Compute Sun's ecliptical coordinates */
  calc_sun_position (d, &lon, r);

  /* Compute ecliptic rectangular coordinates (z=0) */
  x = cosd (lon);
  y = sind (lon);

  /* Compute obliquity of ecliptic (inclination of Earth's axis) */
  obl_ecl = 23.4393 - 3.563E-7 * d;

  /* Convert to equatorial rectangular coordinates - x is uchanged */
  z = y * sind (obl_ecl);
  y = y * cosd (obl_ecl);

  /* Convert to spherical coordinates */
  *RA = atan2d (y, x);
  *sdec = z;
  *cdec = sqrt (1.0 - z * z);
}

/* This function computes the number of days elapsed since 1/1/2000.  */
static inline int
days_this_millennium (int y, int m, int d)
{
  return 367L * y - 7 * (y + (m + 9) / 12) / 4 + 275 * m / 9 + d - 730530L;
}


/* This function computes GMST0, the Greenwhich Mean Sidereal Time at UTC. GMST
   is then the sidereal time at Greenwich at any time of the day.  */
static double
GMST0 (double d)
{
  double sidtim0;

  sidtim0 = normalize ((180.0 + 356.0470 + 282.9404) +
		       (0.9856002585 + 4.70935E-5) * d);
  return sidtim0;
}


int calc_sun_rise_set
  (int year,
   int month,
   int day,
   double lon,
   double lat, double altit, int upper_limb, double *trise, double *tset)
{
  double d,			/* Days since 2000 Jan 0.0 (negative before) */
    sr,				/* Solar distance, astronomical units */
    sRA,			/* Sun's Right Ascension */
    sdec, cdec,			/* Sun's declination sine and cosine */
    sradius,			/* Sun's apparent radius */
    t,				/* Diurnal arc */
    tsouth,			/* Time when Sun is at south */
    sidtime,			/* Local sidereal time */
    cost;

  int rc = 0;			/* Return code from function - usually 0 */

  /* Compute d of 12h local mean solar time */
  d = days_this_millennium (year, month, day) + 0.5 - lon / 360.0;

  /* Compute local sideral time of this moment */
  sidtime = normalize (GMST0 (d) + 180.0 + lon);

  /* Compute Sun's RA + Decl at this moment */
  calc_sun_ra_and_decl (d, &sRA, &sdec, &cdec, &sr);

  /* Compute time when Sun is at south - in hours UT */
  tsouth = 12.0 - normalize180 (sidtime - sRA) / 15.0;

  /* Compute the Sun's apparent radius, degrees */
  sradius = 0.2666 / sr;

  /* Do correction to upper limb, if necessary */
  if (upper_limb)
    altit -= sradius;

  /* Compute the diurnal arc that the Sun traverses to reach */
  /* the specified altitide altit: */
  cost = (sind (altit) - sind (lat) * sdec) / (cosd (lat) * cdec);
  if (cost >= 1.0)
    {
      rc = -1;
      t = 0.0;			/* Sun always below altit */
    }
  else if (cost <= -1.0)
    {
      rc = 1;
      t = 12.0;			/* Sun always above altit */
    }
  else
    {
      t = acosd (cost) / 15.0;	/* The diurnal arc, hours */
    }

  /* Store rise and set times - in hours UTC */
  *trise = tsouth - t;
  *tset = tsouth + t;

  return rc;
}

double calc_day_length
  (int year,
   int month, int day, double lon, double lat, double altit, int upper_limb)
{
  double d,			/* Days since 2000 Jan 0.0 (negative before) */
    obl_ecl,			/* Obliquity (inclination) of Earth's axis */
    sr,				/* Solar distance, astronomical units */
    slon,			/* True solar longitude */
    sin_sdecl,			/* Sine of Sun's declination */
    cos_sdecl,			/* Cosine of Sun's declination */
    sradius,			/* Sun's apparent radius */
    t,				/* Diurnal arc */
    cost;

  d = days_this_millennium (year, month, day) + 0.5 - lon / 360.0;

  /* Compute obliquity of ecliptic (inclination of Earth's axis) */
  obl_ecl = 23.4393 - 3.563E-7 * d;

  /* Compute Sun's position */
  calc_sun_position (d, &slon, &sr);

  /* Compute sine and cosine of Sun's declination */
  sin_sdecl = sind (obl_ecl) * sind (slon);
  cos_sdecl = sqrt (1.0 - sin_sdecl * sin_sdecl);

  /* Compute the Sun's apparent radius, degrees */
  sradius = 0.2666 / sr;

  /* Do correction to upper limb, if necessary */
  if (upper_limb)
    altit -= sradius;

  /* Compute the diurnal arc that the Sun traverses to reach */
  /* the specified altitide altit: */
  cost = (sind (altit) - sind (lat) * sin_sdecl) / (cosd (lat) * cos_sdecl);
  if (cost >= 1.0)
    t = 0.0;			/* Sun always below altit.  */
  else if (cost <= -1.0)
    t = 24.0;			/* Sun always above altit.  */
  else
    t = (2.0 / 15.0) * acosd (cost);	/* The diurnal arc, hours.  */

  return t;
}
