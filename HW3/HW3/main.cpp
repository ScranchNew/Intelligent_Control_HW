#include <fstream>
#include <vector>
#include <iostream>

#include <cstdlib>
#include <ctime>

#include "control.hpp"

int main()
{
    std::srand(std::time(nullptr)); // initialize the random noise generator

    double timeStep = 0.05;
	double const modelOrder=4;
	double const numOfControllers = 2;

	// Initialize controllers
	std::vector<Control> controllers = {Control(modelOrder, 1, timeStep), 
                                        Control(modelOrder, 2, timeStep)};

	//std::vector<Control> controllers = {Control(modelOrder, 1, timeStep)};

    //
    std::vector<double> a = {1.0, -2.856, 2.717, -0.861};
    std::vector<double> b = {0.0, 0.0, 2.3136e-4, 9.28e-7};

	std::fstream fileout("matlab/flc.txt", std::ios::out);
	fileout << "Type_1;Type_2" << std::endl;
	
	std::fstream controllersErr("matlab/err.txt", std::ios::out);
	controllersErr << "Type_1;Type_2" << std::endl;

	std::fstream controllersCtrl("matlab/ctrl.txt", std::ios::out);
	controllersCtrl << "Type_1;Type_2" << std::endl;

	//////////////////////////////////////////////////////
	// Use the parameters you used in HW#1.
	//double Ke = 1.15, Kce = 0.94, Ku = 1.9;
    double Ke = 1., Kce = 1., Ku = 1.25;
	/////////////////////////////////////////////////////

	double yout = 0.0;
	double target = 1.0;

    for(int i = 0; i < numOfControllers; i++){
		controllers[i].setA(a);
		controllers[i].setB(b);
		controllers[i].setTarget(target);
	}

    int maxTimeStep = 500;
    for (int t = 0; t < maxTimeStep; t++)
    {
        for (size_t i = 0; i < numOfControllers; i++)
        {
            std::cout << t << ": ";
            controllers[i].FLC(Ke, Kce, Ku);

			yout = controllers[i].motor();
			controllers[i].delay(yout);

			controllersErr << controllers[i].getErr()<< "; ";
			controllersCtrl << controllers[i].getCtrl()<< "; ";
			fileout << yout << "; ";

			if (i >= numOfControllers - 1) {
				controllersErr <<  std::endl;
				controllersCtrl <<  std::endl;
				fileout <<  std::endl;
			}
			//std::cout << yout << std::endl;
        }
    }

    fileout.close();

    return 0;
}