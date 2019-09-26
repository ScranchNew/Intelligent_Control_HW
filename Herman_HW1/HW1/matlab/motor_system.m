%% Define parameters and transfer functions
Kp = 1.797;
Ki = 0.05742/2;
Kd = 2.903;
Ts = 0.05;

a = Kp + Ki*Ts/2 + Kd/Ts;
b = -Kp + Ki*Ts/2 - 2*Kd/Ts;
c = Kd/Ts;

% Discrete transfer function for PID-controller (m/e)
z = tf('z', Ts);
D = (a + b*z^(-1) + c*z^(-2))/(1 - z^(-1));

% Parameters for motor model
numOfOrder = 4;

a0 = 1.0;
a1 = -2.856;
a2 = 2.717;
a3 = -0.861;
b0 = 0.0;
b1 = 0.0;
b2 = 2.3136e-4;
b3 = 9.28e-7;

% Discrete transfer function for motor model (y/m)
G = (b0 + b1*z^(-1) + b2*z^(-2) + b3*z^(-3))/(a0 + a1*z^(-1) + a2*z^(-2) + a3*z^(-3));

% Transfer function system (y/e)
H = D*G;
% Transfer function feedback (y/r)
sys = feedback(H, 1);

disp(G);
