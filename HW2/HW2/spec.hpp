#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// Specifications
#define STEPVALUE 1
#define RT_THRESHOLD 20.0   // Rising time
#define PO_THERSHOLD 5.0    // Overshoot percentage
#define PCT_ST_THRESHOLD 2  // Settling time percentage intervall
#define ST_THRESHOLD 70.0   // Settling time
#define SPEC_NINF -99999.0
#define SPEC_INF 99999.0

std::vector<double> extern flcOut;


void process_output_data();
double max_value(std::vector<double> output);
double percentage_overshoot(double maxVal, double stepValue);
double rising_time(std::vector<double> output, double stepValue);
double settling_time(std::vector<double> output, double stepValue, double pctMargin);
std::string is_spec_met(bool isSpecMet);
void print_response_characteristics(std::vector<double> output, 
    double stepValue, double pctSettling, double stThreshold, 
    double rtThreshold, double poThreshold);

