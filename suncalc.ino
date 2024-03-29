#include <Servo.h>

#define AZIMUTH_PIN 9

#define REMMERS_LAT 53.181634d
#define REMMERS_LONG 6.541645d

#define TIJD 1481632030

#include <math.h>

#define PI (M_PI)
#define DEG2RAD (PI / 180)
#define RAD2DEG (180 / PI)
#define SECONDS_IN_DAY ((double) (24 * 3600))

struct sun_pos {
	double azimuth;
	double altitude;
};

int J1970 = 2440588,
	J2000 = 2451545;

double M0 = 357.5291 * DEG2RAD,
	M1 = 0.98560028 * DEG2RAD,
	J0 = 0.0009,
	J1 = 0.0053,
	J2 = -0.0069,
	C1 = 1.9148 * DEG2RAD,
	C2 = 0.0200 * DEG2RAD,
	C3 = 0.0003 * DEG2RAD,
	P = 102.9372 * DEG2RAD,
	e = 23.45 * DEG2RAD,
	th0 = 280.1600 * DEG2RAD,
	th1 = 360.9856235 * DEG2RAD,
	h0 = -0.83 * DEG2RAD, //sunset angle
	d0 = 0.53 * DEG2RAD, //sun diameter
	h1 = -6 * DEG2RAD, //nautical twilight angle
	h2 = -12 * DEG2RAD, //astronomical twilight angle
	h3 = -18 * DEG2RAD; //darkness angle


double timestampToJulianDate(int timestamp) {
	return timestamp / SECONDS_IN_DAY + J1970 - 0.5;
}

double getSolarMeanAnomaly(double Js) {
	return M0 + M1 * (Js - J2000);
}

double getEquationOfCenter(double M) {
	return C1 * sin(M) + C2 * sin(2 * M) + C3 * sin(3 * M);
}

double getEclipticLongtitude(double M, double C) {
	return M + P + C + PI;
}

double getSunDeclination(double Lsun) {
	return asin(sin(Lsun) * sin(e));
}

double getRightAscension(double Lsun) {
	return atan2(sin(Lsun) * cos(e), cos(Lsun));
}

double getSiderealTime(double J, double lw) {
	return th0 + th1 * (J - J2000) - lw;
}

double getAzimuth(double th, double a, double phi, double d) {
	double H = th - a;
	return atan2(sin(H), cos(H) * sin(phi) - tan(d) * cos(phi));
}

double getAltitude(double th, double a, double phi, double d) {
	double H = th - a;
	return asin(sin(phi) * sin(d) + cos(phi) * cos(d) * cos(H));
}

int getSunPosition(struct sun_pos* pos, int timestamp, double latitude, double longtitude) {
	if (pos == 0) return -1;

	double J = timestampToJulianDate(timestamp);
	double phi = latitude * DEG2RAD;
	double lw = longtitude * -DEG2RAD;

	double M = getSolarMeanAnomaly(J);
	double C = getEquationOfCenter(M);
	double Lsun = getEclipticLongtitude(M, C);
	double d = getSunDeclination(Lsun);
	double a = getRightAscension(Lsun);
	double th = getSiderealTime(J, lw);

	// TODO
	pos->azimuth = getAzimuth(th, a, phi, d) * RAD2DEG + 180;
	pos->altitude = getAltitude(th, a, phi, d) * RAD2DEG;

	return 0;
}


Servo azimuth;
struct sun_pos pos;

int time = TIJD;
 
 
void setup() {
  azimuth.attach(AZIMUTH_PIN);
  
  azimuth.write(0);
    
  delay(1000);
}
 
void loop() { 
  if(getSunPosition(&pos, TIJD, REMMERS_LAT, REMMERS_LONG) != 0)
    return;
  
  azimuth.write(abs(pos.azimuth - 180));
  
  delay(1000);
  
  time++;
} 
