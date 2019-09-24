#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "fuzzy.hpp"
#include "control.hpp"

int main()
{

	double timeStep=0.05;
	Control control(4, timeStep);
	std::vector<double> a = {1.0, -2.856 , 2.717, -0.861};
	std::vector<double> b = {0.0, 0.0, 2.3136e-4, 9.28e-7};

	control.setA(a);
	control.setB(b);

	std::fstream fileout("flc3.txt", std::ios::out);

	double Kp = 0.927961, Ki = 0.000024, Kd = 1.130647;
	double Ke = 1.886226, Kce = 1.8875, Ku = 1.051013;

	double yout = 0.0;
	double target = 1.0;
	control.setTarget(target);

	int maxTimeStep = 500;
	for (int t=0; t<maxTimeStep; t++) {

//		control.constantK(1);
//		control.PID(Kp, Ki, Kd);
//		control.FLC(Ke, Kce, Ku);
//	  	control.FLC(Ke, Kce, Ku, 3);


		yout = control.motor();
		control.delay(yout);

		std::cout << yout << std::endl;
		fileout << yout << std::endl;
	}

	fileout.close();


	return 0;
}
