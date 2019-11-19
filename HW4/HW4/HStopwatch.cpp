//////////////////////////////////////////////////////////////////////////////
//
//	file: HStopwatch.cpp
//	Date: 2002.3.2
//	Author: Kuk-Hyun Han
//	Description: stopwatch class
//
//////////////////////////////////////////////////////////////////////////////
	

#include <time.h>
#include "HStopwatch.h"


//namespace hkh {				// hkh namespace beginning!

//using namespace hkh;			// hkh namespace usage. 


// Stopwatch class constructor
HStopwatch::HStopwatch()
{
	m_flag = false;
	ftime(&m_start);
	m_stop = m_start;
}


// returns the time difference between two timeb times
double HStopwatch::dt(const timeb &s, const timeb &e) const
{
	double delta;
	delta = 1.0e3 * difftime(e.time, s.time);
	delta += (double)(e.millitm - s.millitm);
	return delta;
}



// Returns the timer output in ms
double HStopwatch::getDT() const
{ 
	if (m_flag) {						// If the timer is stopped: returns m_stop-m_start
		timeb tmp;
		ftime(&tmp);
		return dt(m_start, tmp);
	}
	else								// else: returns the current time - m_start
		return dt(m_start, m_stop);
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// pushes the current timer value into an std::ostream object

std::ostream& operator << (std::ostream& os, const HStopwatch& TM)
{
	//os << TM.dt(TM.m_start, TM.m_stop) << "msec" << std::endl;
	os << TM.getDT() << "msec" << std::endl;
	return os;
}


//};						// hkh namespace ending!
