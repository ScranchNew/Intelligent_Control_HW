#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "fuzzy.hpp"
#include "control.hpp"

int control_num = 4;

int main()
{

	double timeStep=0.05;
	std::vector<Control> control(control_num, Control(4, timeStep));
	std::vector<double> a = {1.0, -2.856 , 2.717, -0.861};
	std::vector<double> b = {0.0, 0.0, 2.3136e-4, 9.28e-7};

	for (size_t i = 0; i < control_num; i++)
	{
		control[i].setA(a);
		control[i].setB(b);
	}
	

	std::fstream fileout("pid.txt", std::ios::out);

	double Kp = 0.927961, Ki = 0.000024, Kd = 1.130647;
	double Ke = 1.0, Kce = 0.95, Ku = 2.0;

	std::vector<double> yout(control_num, 0.0);
	double target = 1.0;
	for (size_t i = 0; i < control_num; i++)
	{
		control[i].setTarget(target);
	}

	int maxTimeStep = 500;
	for (int t=0; t<maxTimeStep; t++) {

		control[0].constantK(1);
		control[1].PID(Kp, Ki, Kd);
		control[2].FLC(Ke, Kce, Ku);
	  	control[3].FLC(Ke, Kce, Ku, 3);


		for (size_t i = 0; i < control_num; i++)
		{
			if ((i == 2) || (i == 3)) {
				yout[i] = control[i].motor();
				control[i].delay(yout[i]);

				std::cout << yout[i];
				fileout << yout[i];
				if (i < control_num-1)
				{
					std::cout << "; ";
					fileout << "; ";
				}
			}
			
		}
		std::cout << std::endl;
		fileout << std::endl;
	}

	fileout.close();


	return 0;
}
