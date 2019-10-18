#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "fuzzy.hpp"
#include "control.hpp"

int main()
{

	double timeStep=0.05;
	std::vector<Control> controlers = {Control(4, timeStep),
										Control(4, timeStep), 
										Control(4, timeStep), 
										Control(4, timeStep), 
										Control(4, timeStep), 
										Control(4, timeStep)};

	std::vector<double> a = {1.0, -2.856 , 2.717, -0.861};
	std::vector<double> b = {0.0, 0.0, 2.3136e-4, 9.28e-7};

	//////////////////////////////////////////////////////
	// Use plausibility measure xi=0.9, belief measure xi=0.1, respectively.

	std::vector<double> xi_s = {0.9, 0.1};
	std::vector<std::vector<double>> preference_s = {{1,10,2},
													 {2,1,10},
													 {10,2,1}};

	Fuzzy::xi = 0.1;
// 	Fuzzy::preference = {1,10,2};
//	Fuzzy::preference = {2,1,10};
//	Fuzzy::preference = {10,2,1};
	//////////////////////////////////////////////////////
	
	for (size_t i = 0; i < 6; i++)
	{
		controlers[i].setA(a);
		controlers[i].setB(b);
	}

	std::fstream fileout("matlab/flc.txt", std::ios::out);
	fileout << "Plaus_1; Plaus_2; Plaus_3; Belief_1; Belief_2; Belief_3" << std::endl;
	
	std::fstream controllersErr("matlab/err.txt", std::ios::out);
	controllersErr << "Plaus_1; Plaus_2; Plaus_3; Belief_1; Belief_2; Belief_3" << std::endl;

	std::fstream controllersCtrl("matlab/ctrl.txt", std::ios::out);
	controllersCtrl << "Plaus_1; Plaus_2; Plaus_3; Belief_1; Belief_2; Belief_3" << std::endl;

	//////////////////////////////////////////////////////
	// Use the parameters you used in HW#1.
	double Ke = 1.15, Kce = 0.94, Ku = 1.9;
	/////////////////////////////////////////////////////

	double yout = 0.0;
	double target = 1.0;

	for (size_t i = 0; i < 6; i++)
	{
		controlers[i].setTarget(target);
	}

	int maxTimeStep = 500;
	for (int t=0; t<maxTimeStep; t++) {

		for (size_t i = 0; i < 6; i++)
		{
			Fuzzy::xi = xi_s[ i > 2 ? 1 : 0 ];
			Fuzzy::preference = preference_s[i%3];

			controlers[i].FLC(Ke, Kce, Ku);


			yout = controlers[i].motor();
			controlers[i].delay(yout);

			controllersErr << controlers[i].getErr()<< "; ";
			controllersCtrl << controlers[i].getCtrl()<< "; ";
			fileout << yout << "; ";

			if (i > 4) {
				controllersErr <<  std::endl;
				controllersCtrl <<  std::endl;
				fileout <<  std::endl;
			}
			//std::cout << yout << std::endl;
		}

	}

	fileout.close();
	controllersErr.close();
	controllersCtrl.close();
	return 0;
}
