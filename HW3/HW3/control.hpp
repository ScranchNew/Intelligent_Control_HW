#pragma once

#include <iostream>

#include "IT2FLS_HW.cpp"

class Control
{

public:
    Control(int _order, int sys_type = 1, double _timeStep = 0.5) : order(_order), timeStep(_timeStep)
    {

        a.resize(order);
        b.resize(order);

        e.resize(order);
        m.resize(order);
        y.resize(order);

        initialize(sys_type);
    }

    void setA(std::vector<double> &_a)
    {
        a = _a;
    }
    void setB(std::vector<double> &_b)
    {
        b = _b;
    }
    void setTarget(int _target)
    {
        target = _target;
        e[0] = target - y[0];
    }

    void initialize(int sys_type)
    {
        fuzzy_init(&fuzzy_system, sys_type);
    }

    double motor()
    {
        double mtotal = 0.0, ytotal = 0.0;

        for (int i = 0; i < order; i++)
        {
            mtotal += b[i] * m[i];
            if (i > 0)
                ytotal += a[i] * y[i];
        }
        y[0] = mtotal - ytotal;

        // Add the noise

        double noise = 0;//0.0001 * (((double)(std::rand()% 1000) / 500. ) - 1.);

        y[0] += noise;

        return y[0];
    }

    void delay(double yout)
    {
        int i;
        for (i = y.size() - 1; i > 0; i--)
        {
            e[i] = e[i - 1];
            m[i] = m[i - 1];
            y[i] = y[i - 1];
        }

        e[0] = target - yout;
    }

    void FLC(double Ke, double Kce, double Ku)
    {
        double error, d_error;

        // error
        error = Ke * e[0];
        // derivative of error
        d_error = Kce * (e[0] - e[1]) / timeStep;

		fuzzy_control(error, d_error, &fuzzy_system);

        m[0] = Ku * control_output();

		std::cout << "; " << y[0] << "; " << m[0] << "; " << std::endl;
    }

    double getErr()
    {
        return e[0];
    }

    double getCtrl()
    {
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

	FUZ_SYS fuzzy_system;  // The fuzzy system
};
