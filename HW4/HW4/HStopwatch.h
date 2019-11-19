//////////////////////////////////////////////////////////////////////////////
//
//	file: HStopwatch.h
//	Date: 2002.3.2
//	Author: Kuk-Hyun Han
//	Description: stopwatch class
//
//////////////////////////////////////////////////////////////////////////////


#ifndef	_HSTOPWATCH_H_
#define	_HSTOPWATCH_H_

#include <iostream>
#include <sys/timeb.h>

//namespace hkh {				// hkh namespace beginning!

class HStopwatch {				// Stopwatch class template

	timeb	m_start;		// start time
	timeb	m_stop;			// stop time
	bool	m_flag;			// running flag (true if running)

	double dt(const timeb &s, const timeb &e) const;		// calculates the difference between to timeb times in ms

public:

	HStopwatch();					// The constructor

	// starts the timer if it's not running already
	void start() {					
		if (!m_flag) {
			ftime(&m_start);		// sets the starttime
			m_flag = true;
		}
	}
	
	// stops the timer if it's running
	void stop() {					
		if (m_flag) {
			ftime(&m_stop);			// sets the stop time
			m_flag = false;		
		}
	}

	// Sets the running flag to true to resume
	void resume() { m_flag = true; }		// resumes a timer as if it was never stopped

	// Gives out the current timer value in ms
	double getDT() const;

	// Let's you push the current timer value to std::ostream objects
	friend std::ostream& operator << (std::ostream& os, const HStopwatch& TM);
};


//}				// hkh namespace ending!

#endif
// !_HSTOPWATCH_H_
