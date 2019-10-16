#include "spec.hpp"

using namespace std;

// Controller output
vector<double> flcOut;


void process_output_data(){
     fstream outputFile("matlab/flc.txt", ios_base::in);
     string line;
     string delimiter = ";";
     // Split up the values for each line and put them in a vector
     while(getline(outputFile, line)){
         vector<string> values;
         stringstream ss(line);
         string token;
         int pos = 0;
         while((pos = line.find(delimiter)) != string::npos){
             token = line.substr(0, pos);
             values.push_back(token);
             line.erase(0, pos + delimiter.length());
         }
         values.push_back(line); // Get the last value as well

         // Put the values in their respective vectors
         if (values[0] == "FLC") continue; // Do not want header
         else
         {
             flcOut.push_back(stod(values[0]));
         }
     }
}


double max_value(std::vector<double> output){
    double maxValue = SPEC_NINF;
    for (double value : output){
        if (value > maxValue){
            maxValue = value;
        }
    }
    return maxValue;
}


double percentage_overshoot(double maxVal, double stepValue){
    double po = (maxVal - stepValue) * 100.0 / stepValue;
    if (po < 0){
        po = 0;
    }
    return po;
}


double rising_time(std::vector<double> output, double stepValue){
    double yMin = STEPVALUE / 10.0;
    double yMax = STEPVALUE * 9 / 10.0;
    double startTime = SPEC_INF;
    double endTime = SPEC_INF;
    for(int time = 0; time < output.size(); time++){
        double value = output[time];
        if (value >= yMin && startTime == SPEC_INF){
            startTime = time;
        }
        if (value >= yMax && endTime == SPEC_INF){
            endTime = time;
        }
    }
    return endTime - startTime;
}


double settling_time(std::vector<double> output, double stepValue, double pctMargin){
    double yMin = stepValue - stepValue * pctMargin / 100.0;
    double yMax = stepValue + stepValue * pctMargin / 100.0;
    double settlingTime = SPEC_INF;
    for(int time = 0; time < output.size(); time++){
        double value = output[time];
        if(value >= yMin && value <= yMax && settlingTime == SPEC_INF){
            settlingTime = time;
        }
        if(value < yMin || value > yMax){
            settlingTime = SPEC_INF;
        }
    }
    return settlingTime;
}


std::string is_spec_met(bool isSpecMet){
    if(isSpecMet) return "Y"; // Check mark
    return "X";
}


void print_response_characteristics(std::vector<double> output, double stepValue, 
double pctSettling, double stThreshold, double rtThreshold, double poThreshold){

    double maxValue = max_value(output);
    double po = percentage_overshoot(maxValue, stepValue);
    double risingTime = rising_time(output, stepValue);
    double settlingTime = settling_time(output, stepValue, pctSettling);

    bool poSpec = (po <= poThreshold);
    bool risingTimeSpec = (risingTime <= rtThreshold);
    bool settlingTimeSpec = (settlingTime <= stThreshold);

    cout << fixed << setprecision(2);

    cout << "The percentage overshoot was " << po << "%: " << is_spec_met(poSpec) << endl;
    cout << "The rising time was " << static_cast<int>(risingTime) << " seconds: " << is_spec_met(risingTimeSpec) << endl;
    cout << "The settling time was " << static_cast<int>(settlingTime) << " seconds: " << is_spec_met(settlingTimeSpec) << endl;
    cout << endl;
}

