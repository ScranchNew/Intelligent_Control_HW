#pragma once

namespace Fuzzy {
#define INF 99999.0
#define DIV 100

typedef std::vector<double> Rule;
typedef std::vector<std::vector<double>> Out;

std::vector<double> NB = {-INF,   -1,   -2/3.};
std::vector<double> NM = {-1,   -2/3., -1/3.};
std::vector<double> NS = {-2/3., -1/3.,  0};
std::vector<double> ZO = {-1/3.,  0,    1/3.};
std::vector<double> PS = { 0,    1/3.,  2/3.};
std::vector<double> PM = { 1/3.,  2/3.,  1};
std::vector<double> PB = { 2/3.,  1,    INF};

double membership(double x, std::vector<double>& A) {
	double a, b; // y = ax+b
	if (A[0]==-INF && x<A[1])  return 1;
	if (A[2]== INF && x>=A[1]) return 1;
	if (x>=A[0] && x<A[1]) {
		a = 1/(A[1]-A[0]);
		b = -A[0]/(A[1]-A[0]);
		return a*x+b;
	}
	if (x>=A[1] && x<A[2]) {
		a = -1/(A[2]-A[1]);
		b = A[2]/(A[2]-A[1]);
		return a*x+b;
	}

	return 0;
}

double strength(double x1, std::vector<double>& A1, double x2, std::vector<double>& A2) {
	return std::min(membership(x1,A1), membership(x2,A2));
}

double defuzzy(std::vector<double>& rule, std::vector<std::vector<double>>& out, int mode) {
	double tmp1=0.0,tmp2=0.0;
	std::vector<double> member(DIV, 0);

	switch(mode) {

	case 1:
		for (int i=0; i< rule.size(); i++) {
			tmp1+=rule[i]*out[i][1];
			tmp2+=rule[i];
		}
		if (tmp2==0) return 0;
		return tmp1/tmp2;

	case 2:
		for (int i=0; i<rule.size(); i++) {
			for (int j=0; j<member.size(); j++) {
				tmp1 = membership(-1.0+2.0*j/(member.size()-1.0), out[i]);
				tmp1 = std::max(tmp1, rule[i]);
				member[j] = std::max(member[j], tmp1);
			}
		}

		tmp1=0.0;
		tmp2=0.0;

		for (int i=0; i<member.size(); i++) {
			tmp1 += member[i]*(-1.0+2.0*i/(member.size()-1.0));
			tmp2 += member[i];
		}

		if (tmp2=0.0) return 0.0;
		return tmp1/tmp2;

	case 3:
		for (int i=0; i<rule.size(); i++) {
			for (int j=0; j<member.size(); j++) {
				tmp1 = rule[i]*membership(-1.0+2.0*j/(member.size()-1.0), out[i]);
				member[j] = std::max(member[j], tmp1);
			}
		}

		tmp1=0.0;
		tmp2=0.0;

		for (int i=0; i<member.size(); i++) {
			tmp1 += member[i]*(-1.0+2.0*i/(member.size()-1.00));
			tmp2 += member[i];
		}

		if (tmp2 ==0.0) return 0.0;
		return tmp1/tmp2;
	default:
		std::cout << "Incorrect Defuzzification Error !!";
		assert(false);
		return 0.0;

	}
	return 0;
}

};
