// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#ifndef LATERO_GRAPHICS_UNITS_H
#define LATERO_GRAPHICS_UNITS_H

#include <string>
#include <math.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace latero {
namespace graphics { 

namespace units {

static const std::string percent = "%";
static const std::string hz = "Hz";
static const std::string per_sec = "/s";
static const std::string ms = "ms";
static const std::string mm = "mm";
static const std::string mm_per_sec = mm+per_sec;
static const std::string degree = "\u00b0";
static const std::string degree_per_sec = degree+per_sec;
static const std::string rad = "rad";

// time
static const std::string day = "days";
static const std::string hour = "hours";
static const std::string min = "minutes";
static const std::string sec = "s";
static const std::string msec = "ms";
static const std::string usec = "us";
static const std::string nsec = "ns";

inline static double RadToDegree(double rad) { return rad*180/M_PI; }
inline static double DegreeToRad(double deg) { return deg*M_PI/180; }

inline static boost::posix_time::time_duration ConvertTime(long t, std::string units)
{
	if (units==nsec) return boost::posix_time::microseconds(t/1000);
	else if (units==usec) return boost::posix_time::microseconds(t);
	else if (units==msec) return boost::posix_time::milliseconds(t);
	else if (units==sec) return boost::posix_time::seconds(t);
	else if (units==min) return boost::posix_time::minutes(t);
	else if (units==hour) return boost::posix_time::hours(t);
	else if (units==day) return boost::posix_time::hours(t*24);
	else return boost::posix_time::seconds(0);
}

// Shift an angle in radians such that it fits within the range [min, min+2PI[.
inline static double RadShift(double rad, double min)
{
	while (rad < min) rad += 2*M_PI;
	while (rad >= min+2*M_PI) rad -= 2*M_PI;
	return rad;
}

// Shift an angle in degrees such that it fits within the range [min, min+360[.
inline static double DegreeShift(double degree, double min)
{
	while (degree < min) degree += 360;
	while (degree >= min+360) degree -= 360;
	return degree;
}

} // namespace units

} // namespace graphics
} // namespace latero

#endif
