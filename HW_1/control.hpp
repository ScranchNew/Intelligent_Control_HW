#pragma once

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
		numRule=19;
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

		rule[0] = Fuzzy::strength(error, Fuzzy::NB, d_error, Fuzzy::ZO); out[0] = Fuzzy::NB;
		rule[1] = Fuzzy::strength(error, Fuzzy::NB, d_error, Fuzzy::PS); out[1] = Fuzzy::NM;
		rule[2] = Fuzzy::strength(error, Fuzzy::NM, d_error, Fuzzy::ZO); out[2] = Fuzzy::NM;
		rule[3] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::ZO); out[3] = Fuzzy::NS;
		rule[4] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::PS); out[4] = Fuzzy::ZO;
		rule[5] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::PB); out[5] = Fuzzy::PM;
		rule[6] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NB); out[6] = Fuzzy::NB;
		rule[7] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NM); out[7] = Fuzzy::NM;
		rule[8] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NS); out[8] = Fuzzy::NS;
		rule[9] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::ZO); out[9] = Fuzzy::ZO;
		rule[10]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PS); out[10]= Fuzzy::PS;
		rule[11]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PM); out[11]= Fuzzy::PM;
		rule[12]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PB); out[12]= Fuzzy::PB;
		rule[13]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::NB); out[13]= Fuzzy::NM;
		rule[14]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::NS); out[14]= Fuzzy::ZO;
		rule[15]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::ZO); out[15]= Fuzzy::PS;
		rule[16]= Fuzzy::strength(error, Fuzzy::PM, d_error, Fuzzy::ZO); out[16]= Fuzzy::PM;
		rule[17]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::NS); out[17]= Fuzzy::PM;
		rule[18]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::ZO); out[18]= Fuzzy::PB;

		m[0]=Ku*Fuzzy::defuzzy(rule, out, method);
	}

private:
	int order;
	double timeStep;
	double target;
	std::vector<double> a;
	std::vector<double> b;
	std::vector<double> e;
	std::vector<double> m;
	std::vector<double> y;

private:
	int numRule;
	Fuzzy::Rule rule;
	Fuzzy::Out out;
};
