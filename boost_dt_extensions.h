/* Some helpful extensions to the Boost date_time library. */

#ifndef BOOST_DT_EXTENSIONS
#define BOOST_DT_EXTENSIONS

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

/* Converts ptime objects into strings */
inline std::string date_time_to_string(boost::posix_time::ptime p) {
	std::stringstream ss;
	boost::posix_time::time_duration t = p.time_of_day();
	boost::gregorian::date d = p.date();

	ss << d.year();

	ss << "-";

	if (d.month() < 10) {
		ss<< "0"<<d.month().as_number();
	}
	else {
		ss<<d.month().as_number();
	}

	ss << "-";

	if (d.day() < 10) {
		ss<< "0"<<d.day();
	}
	else {
		ss<<d.day();
	}

	ss << " ";

	if (t.hours() < 10) {
		ss<< "0"<<t.hours();
	}
	else {
		ss<<t.hours();
	}

	ss << ":";

	if (t.minutes() < 10) {
		ss<< "0"<<t.minutes();
	}
	else {
		ss<<t.minutes();
	}

	ss << ":";

	if (t.seconds() < 10) {
		ss<< "0"<<t.seconds();
	}
	else {
		ss<<t.seconds();
	}

	return ss.str();
}

/* Returns true if the given date_time is a weekend */
inline bool is_weekend(boost::posix_time::ptime p) {
	boost::gregorian::date d = p.date();

	int day = d.day_of_week();

	return (day == 6 || day == 0);

}

/* Modulous operator for ptime dates */
inline boost::posix_time::time_duration ptime_mod(boost::posix_time::time_duration p1, boost::posix_time::time_duration p2) {
	boost::posix_time::time_duration td = p1 - p2;

	if (td.is_negative()) {
		return td.invert_sign();
	}

	else {
		while(!((td - p2).is_negative())) {
			td = td - p2;
		}

		return td;
	}
}

#endif








