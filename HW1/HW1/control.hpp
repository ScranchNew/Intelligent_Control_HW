#pragma once

#include<iostream>

class Control {

public:
	Control(int _order, double _timeStep=0.5): order(_order), timeStep(_timeStep) {

		a.resize(order);
		b.resize(order);

		e.resize(order);
		m.resize(order);
		y.resize(order);

		initialize();
	}

	void setA(std::vector<double>& _a) {
		a = _a;
	}
	void setB(std::vector<double>& _b) {
		b = _b;
	}
	void setTarget(int _target) {
		target = _target;
		e[0] = target-y[0];
	}

	void initialize() {

		// for PID control
		std::fill(e.begin(), e.end(), 0);
		std::fill(m.begin(), m.end(), 0);
		std::fill(y.begin(), y.end(), 0);

		// for Fuzzy control
		numRule=32;
		rule.resize(numRule);
		out.resize(numRule);
		std::fill(out.begin(), out.end(), std::vector<double>(3));
	}

	double motor() {
		double mtotal=0.0,ytotal=0.0;

		for(int i=0;i <order;i++){
			mtotal += b[i]*m[i];
			if(i > 0) ytotal += a[i]*y[i];
		}
		y[0] = mtotal-ytotal;
		return y[0];
	}

	void delay(double yout) {
		int i;
		for (i=y.size()-1; i>0; i--) {
			e[i] = e[i-1];
			m[i] = m[i-1];
			y[i] = y[i-1];
		}

		e[0] = target-yout;
	}

	void constantK(int k) {
		for (int i=0; i<order; i++) m[i]=k*e[i];
	}

	void PID(double Kp, double Ki, double Kd) {
		double a = Kp + Ki*timeStep/2.0+Kd/timeStep;
		double b = Ki*timeStep/2.0 - Kp - 2.0*Kd/timeStep;
		double c = Kd/timeStep;
		m[0] = m[1] + a*e[0] + b*e[1] + c*e[2];
	}

	void FLC(double Ke, double Kce, double Ku, int method=1) {

		double error, d_error;
		error = Ke*e[0];
		d_error = Kce*(e[0]-e[1])/timeStep;

		// rules for NM (NB does not happen)
		rule[0] = Fuzzy::strength(error, Fuzzy::NM, d_error, Fuzzy::NM); out[0] = Fuzzy::NM;
		rule[1] = Fuzzy::strength(error, Fuzzy::NM, d_error, Fuzzy::NS); out[1] = Fuzzy::NS;
		rule[2] = Fuzzy::strength(error, Fuzzy::NM, d_error, Fuzzy::ZO); out[2] = Fuzzy::NS;
		rule[3] = Fuzzy::strength(error, Fuzzy::NM, d_error, Fuzzy::PS); out[3] = Fuzzy::ZO;
		rule[4] = Fuzzy::strength(error, Fuzzy::NM, d_error, Fuzzy::PM); out[4] = Fuzzy::ZO;

		// rules for NS
		rule[5] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::NM); out[5] = Fuzzy::NM;
		rule[6] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::NS); out[6] = Fuzzy::NS;
		rule[7] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::ZO); out[7] = Fuzzy::NS;
		rule[8] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::PS); out[8] = Fuzzy::ZO;
		rule[9] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::PM); out[9] = Fuzzy::PS;

		// rules for ZO
		rule[10] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NB); out[10] = Fuzzy::NM;
		rule[11] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NM); out[11] = Fuzzy::NS;
		rule[12] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NS); out[12] = Fuzzy::NS;
		rule[13] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::ZO); out[13] = Fuzzy::ZO;
		rule[14]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PS); out[14]= Fuzzy::PS;
		rule[15]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PM); out[15]= Fuzzy::PS;
		rule[16]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PB); out[16]= Fuzzy::PM;

		// PS
		rule[17]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::NB); out[17]= Fuzzy::NB;
		rule[18]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::NM); out[18]= Fuzzy::NM;
		rule[19]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::NS); out[19]= Fuzzy::ZO;
		rule[20]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::ZO); out[20]= Fuzzy::PS;
		rule[21]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::PS); out[21]= Fuzzy::PM;

		// PM
		rule[22]= Fuzzy::strength(error, Fuzzy::PM, d_error, Fuzzy::NB); out[22]= Fuzzy::NB;
		rule[23]= Fuzzy::strength(error, Fuzzy::PM, d_error, Fuzzy::NM); out[23]= Fuzzy::NM;
		rule[24]= Fuzzy::strength(error, Fuzzy::PM, d_error, Fuzzy::NS); out[24]= Fuzzy::ZO;
		rule[25]= Fuzzy::strength(error, Fuzzy::PM, d_error, Fuzzy::ZO); out[25]= Fuzzy::PS;
		rule[26]= Fuzzy::strength(error, Fuzzy::PM, d_error, Fuzzy::PS); out[26]= Fuzzy::PM;


		// PB
		rule[27]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::NB); out[27]= Fuzzy::NB;
		rule[28]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::NM); out[28]= Fuzzy::PB;
		rule[29]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::NS); out[29]= Fuzzy::PB;
		rule[30]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::ZO); out[30]= Fuzzy::PB;
		rule[31]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::ZO); out[31]= Fuzzy::PB;

		m[0]=Ku*Fuzzy::defuzzy(rule, out, method);
	}

	double getErr() {
		return e[0];
	}

	double getCtrl() {
		return m[0];
	}

private:
	int order;
	double timeStep;
	double target;
	std::vector<double> a; // Define model of motor
	std::vector<double> b; // Define model of motor
	std::vector<double> e; // Error
	std::vector<double> m; // Output from controller
	std::vector<double> y; // Output from plant

private:
	int numRule;
	Fuzzy::Rule rule;
	Fuzzy::Out out;
};
