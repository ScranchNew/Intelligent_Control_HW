#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "fuzzy.hpp"
#include "control.hpp"
#include "spec.hpp"

int main()
{

	double const timeStep=0.05;
	double const modelOrder=4;
	double const numOfControllers = 4;
	double const target = STEPVALUE;

	std::vector<double> a = {1.0, -2.856 , 2.717, -0.861};
	std::vector<double> b = {0.0, 0.0, 2.3136e-4, 9.28e-7};

	// Write all membership functions to file
	std::fstream mfFile("matlab/mf.txt", std::ios::out);
	Fuzzy::write_MF_to_file(mfFile, Fuzzy::NB, "NB");
	Fuzzy::write_MF_to_file(mfFile, Fuzzy::NM, "NM");
	Fuzzy::write_MF_to_file(mfFile, Fuzzy::NS, "NS");
	Fuzzy::write_MF_to_file(mfFile, Fuzzy::ZO, "ZO");
	Fuzzy::write_MF_to_file(mfFile, Fuzzy::PS, "PS");
	Fuzzy::write_MF_to_file(mfFile, Fuzzy::PM, "PM");
	Fuzzy::write_MF_to_file(mfFile, Fuzzy::PB, "PB");
	mfFile.close();


	// Initialize controllers
	std::vector<Control> controllers(numOfControllers, Control(modelOrder, timeStep));

	for(int i = 0; i < numOfControllers; i++){
		Control control(modelOrder, timeStep);
		controllers[i].setA(a);
		controllers[i].setB(b);
		controllers[i].setTarget(target);
	}

	std::fstream controllersOut("matlab/output.txt", std::ios::out);
	controllersOut << "P;PID;FLC;FLC3" << std::endl;
	
	std::fstream controllersErr("matlab/err.txt", std::ios::out);
	controllersErr << "P;PID;FLC;FLC3" << std::endl;

	std::fstream controllersCtrl("matlab/ctrl.txt", std::ios::out);
	controllersCtrl << "P;PID;FLC;FLC3" << std::endl;

	// For P- and PID-controller
	double Kp = 1.2, Ki = 0.000024, Kd = 1.9;


	// For fuzzy-controller
	double Ke = 1.15, Kce = 0.95, Ku = 1.9;  // Ke and Ku make faster and more aggressive output. Kce make it more robust

	std::fstream flc_setting("matlab/flc_settings.txt", std::ios::out);
	flc_setting << "Ke, Kce, Ku" << std::endl;
	flc_setting << Ke << ";" << Kce << ";" << Ku << std::endl;
	flc_setting.close();

	int const maxTimeStep = 500;
	for (int t=0; t<maxTimeStep; t++) {

		std::vector <double> m(numOfControllers);
		std::vector <double> e(numOfControllers);

		controllers[0].constantK(1);
		controllers[1].PID(Kp, Ki, Kd);
		controllers[2].FLC(Ke, Kce, Ku);
	  	controllers[3].FLC(Ke, Kce, Ku, 3);

		for (size_t i = 0; i < numOfControllers; i++)
		{
			e[i] = controllers[i].getErr();
			m[i] = controllers[i].getCtrl();
			
			controllersErr << e[i];
			controllersCtrl << m[i];

			if (i < numOfControllers - 1){
				controllersErr << ";";
				controllersCtrl << ";";
			}
		}
		
		
		for(int i=0; i<numOfControllers; i++){
			double yout = controllers[i].motor();
			controllers[i].delay(yout);
			controllersOut << yout;

			if (i < numOfControllers - 1){
				controllersOut << ";";
			}
		}

		controllersErr << std::endl;
		controllersCtrl << std::endl;

		controllersOut << std::endl;

	}

	controllersErr.close();
	controllersCtrl.close();

	controllersOut.close();

	process_output_data();

	//std::cout << "P-controller" << std::endl;
	//print_response_characteristics(pOut, STEPVALUE, PCT_ST_THRESHOLD, ST_THRESHOLD, RT_THRESHOLD, PO_THERSHOLD);
	std::cout << "PID-controller" << std::endl;
	print_response_characteristics(pidOut, STEPVALUE, PCT_ST_THRESHOLD, ST_THRESHOLD, RT_THRESHOLD, PO_THERSHOLD);
	std::cout << "FLC-controller" << std::endl;
	print_response_characteristics(flcOut, STEPVALUE, PCT_ST_THRESHOLD, ST_THRESHOLD, RT_THRESHOLD, PO_THERSHOLD);
	//std::cout << "FLC3-controller" << std::endl;
	//print_response_characteristics(flc3Out, STEPVALUE, PCT_ST_THRESHOLD, ST_THRESHOLD, RT_THRESHOLD, PO_THERSHOLD);


	return 0;
}
