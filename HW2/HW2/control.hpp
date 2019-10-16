#pragma once

#define A 1.0
#define B A/2.0
#define C B/2.0
#define D C/2.0
#define E D/2.0

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
		h.resize(numRule);
		weight.resize(numRule);
		std::fill(out.begin(), out.end(), std::vector<double>(3));
		std::fill(h.begin(), h.end(), std::vector<double>(3));
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

		rule[0] = Fuzzy::strength(error, Fuzzy::NB, d_error, Fuzzy::ZO); out[0] = Fuzzy::NB; h[0] = std::vector<double>{E,A,B};
		rule[1] = Fuzzy::strength(error, Fuzzy::NB, d_error, Fuzzy::PS); out[1] = Fuzzy::NM; h[1] = std::vector<double>{E,B,B};
		rule[2] = Fuzzy::strength(error, Fuzzy::NM, d_error, Fuzzy::ZO); out[2] = Fuzzy::NM; h[2] = std::vector<double>{E,B,B};
		rule[3] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::ZO); out[3] = Fuzzy::NS; h[3] = std::vector<double>{E,C,C};
		rule[4] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::PS); out[4] = Fuzzy::ZO; h[4] = std::vector<double>{D,D,D};
		rule[5] = Fuzzy::strength(error, Fuzzy::NS, d_error, Fuzzy::PB); out[5] = Fuzzy::PM; h[5] = std::vector<double>{E,D,B};
		rule[6] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NB); out[6] = Fuzzy::NB; h[6] = std::vector<double>{E,A,B};
		rule[7] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NM); out[7] = Fuzzy::NM; h[7] = std::vector<double>{E,B,B};
		rule[8] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::NS); out[8] = Fuzzy::NS; h[8] = std::vector<double>{E,C,C};
		rule[9] = Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::ZO); out[9] = Fuzzy::ZO; h[9] = std::vector<double>{D,D,D};
		rule[10]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PS); out[10]= Fuzzy::PS; h[10] = std::vector<double>{E,D,C};
		rule[11]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PM); out[11]= Fuzzy::PM; h[11] = std::vector<double>{E,D,B};
		rule[12]= Fuzzy::strength(error, Fuzzy::ZO, d_error, Fuzzy::PB); out[12]= Fuzzy::PB; h[12] = std::vector<double>{E,D,B};
		rule[13]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::NB); out[13]= Fuzzy::NM; h[13] = std::vector<double>{E,B,B};
		rule[14]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::NS); out[14]= Fuzzy::ZO; h[14] = std::vector<double>{D,D,D};
		rule[15]= Fuzzy::strength(error, Fuzzy::PS, d_error, Fuzzy::ZO); out[15]= Fuzzy::PS; h[15] = std::vector<double>{C,D,C};
		rule[16]= Fuzzy::strength(error, Fuzzy::PM, d_error, Fuzzy::ZO); out[16]= Fuzzy::PM; h[16] = std::vector<double>{B,D,B};
		rule[17]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::NS); out[17]= Fuzzy::PM; h[17] = std::vector<double>{B,D,B};
		rule[18]= Fuzzy::strength(error, Fuzzy::PB, d_error, Fuzzy::ZO); out[18]= Fuzzy::PB; h[18] = std::vector<double>{A,D,B};

		double s= std::pow(1-Fuzzy::xi,2)/std::pow(Fuzzy::xi,2);
		Fuzzy::getWeight_u(Fuzzy::preference, Fuzzy::weight_u);

		for (int i=0; i<weight.size(); i++)
			weight[i]=Fuzzy::choquet(s, h[i], Fuzzy::weight_u);

		m[0]=Ku*Fuzzy::defuzzy(rule, out, weight);
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
	std::vector<double> a;
	std::vector<double> b;
	std::vector<double> e;
	std::vector<double> m;
	std::vector<double> y;

private:
	int numRule;
	Fuzzy::Rule rule;
	Fuzzy::Out out;
	Fuzzy::Out h;
	Fuzzy::Weight weight;
};
