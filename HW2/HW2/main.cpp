#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "fuzzy.hpp"
#include "control.hpp"
#include "spec.hpp"

int main()
{

	double timeStep=0.05;
	Control control(4, timeStep);
	std::vector<double> a = {1.0, -2.856 , 2.717, -0.861};
	std::vector<double> b = {0.0, 0.0, 2.3136e-4, 9.28e-7};

	//////////////////////////////////////////////////////
	// Use plausibility measure xi=0.75, belief measure xi=0.25, respectively. 
	Fuzzy::xi = 0.8;
// 	Fuzzy::preference = {1,10,5};
//	Fuzzy::preference = {5,1,10};
	Fuzzy::preference = {10,5,1};
	//////////////////////////////////////////////////////
	
	control.setA(a);
	control.setB(b);

	std::fstream fileout("matlab/flc.txt", std::ios::out);
	fileout << "FLC" << std::endl;
	
	std::fstream controllersErr("matlab/err.txt", std::ios::out);
	controllersErr << "FLC" << std::endl;

	std::fstream controllersCtrl("matlab/ctrl.txt", std::ios::out);
	controllersCtrl << "FLC" << std::endl;

	//////////////////////////////////////////////////////
	// Use the parameters you used in HW#1.
	double Ke = 1.15, Kce = 0.94, Ku = 1.9;
	/////////////////////////////////////////////////////

	double yout = 0.0;
	double target = 1.0;
	control.setTarget(target);

	int maxTimeStep = 500;
	for (int t=0; t<maxTimeStep; t++) {

		control.FLC(Ke, Kce, Ku);

		controllersErr << control.getErr()<< std::endl;
		controllersCtrl << control.getCtrl()<< std::endl;

		yout = control.motor();
		control.delay(yout);

		//std::cout << yout << std::endl;
		fileout << yout << std::endl;
	}

	fileout.close();
	controllersErr.close();
	controllersCtrl.close();

	process_output_data();
	
	std::cout << "FLC-controller" << std::endl;
	print_response_characteristics(flcOut, STEPVALUE, PCT_ST_THRESHOLD, ST_THRESHOLD, RT_THRESHOLD, PO_THERSHOLD);

	return 0;
}
