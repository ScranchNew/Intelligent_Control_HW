%% Output data from controllers
fidData = fopen('output.txt');
tData = textscan(fidData, '%f%f%f%f', 'Delimiter', ';', 'headerlines', 1);
fclose(fidData);

fidData = fopen('err.txt');
eData = textscan(fidData, '%f%f%f%f', 'Delimiter', ';', 'headerlines', 1);
fclose(fidData);

fidData = fopen('ctrl.txt');
cData = textscan(fidData, '%f%f%f%f', 'Delimiter', ';', 'headerlines', 1);
fclose(fidData);

fidData = fopen('flc_settings.txt');
flcSet = textscan(fidData, '%f%f%f%f', 'Delimiter', ';', 'headerlines', 1);
fclose(fidData);

pOut = zeros(500,1);
pidOut = zeros(500,1);
flcOut = zeros(500,1);
flc3Out = zeros(500,1);

for i= 1:size(tData{1},1)
    pOut(i) = tData{1,1}(i);
    pidOut(i) = tData{1,2}(i);
    flcOut(i) = tData{1,3}(i);
    flc3Out(i) = tData{1,4}(i);
end

% Plot
figure(1);
x = 1:500;
%plot(x, pOut);
hold on;
plot(x, pidOut);
plot(x, flcOut);
%plot(x, flc3Out);
legend('PID', 'FLC');
title('Step response');
xlabel('Time step [$s$]', 'Interpreter', 'latex');
ylabel('Output', 'Interpreter', 'latex');
grid on;
hold off;


%% Error

pErr = eData{1,1};
pidErr = eData{1,2};
flcErr = eData{1,3}; %% * flcSet{1,1};
flc3Err = eData{1,4};

p_dErr = zeros(500,1);
pid_dErr = zeros(500,1);
flc_dErr = zeros(500,1);
flc3_dErr = zeros(500,1);

for i= 1:size(eData{1},1)
    if i > 1
        p_dErr(i) = (eData{1,1}(i) - eData{1,1}(i-1))/0.05;
        pid_dErr(i) = (eData{1,2}(i) - eData{1,2}(i-1))/0.05;
        flc_dErr(i) = (eData{1,3}(i) - eData{1,3}(i-1))/0.05; %% * flcSet{1,2};
        flc3_dErr(i) = (eData{1,4}(i) - eData{1,4}(i-1))/0.05;
    end
end

%% Control
pCtrl = zeros(500,1);
pidCtrl = zeros(500,1);
flcCtrl = zeros(500,1);
flc3Ctrl = zeros(500,1);

for i= 1:size(cData{1},1)
    pCtrl(i) = cData{1,1}(i);
    pidCtrl(i) = cData{1,2}(i);
    flcCtrl(i) = cData{1,3}(i); %% / flcSet{1,3};
    flc3Ctrl(i) = cData{1,4}(i);
end

% Plot
figure(2);
x = 1:500;
hold on;
plot(x, pidErr);
plot(x, pid_dErr);
plot(x, pidCtrl);
axis([0 100 -2 2]);
legend('Err', 'd_Err', 'Control');
title('Control Signal');
xlabel('Time step [$s$]', 'Interpreter', 'latex');
ylabel('Output', 'Interpreter', 'latex');
grid on;
hold off;

% Plot
figure(3);
x = 1:500;
hold on;
plot(x, flcErr);
plot(x, flc_dErr);
plot(x, flcCtrl);
axis([0 100 -2 2]);
legend('Err', 'd_Err', 'Control');
title('Control Signal');
xlabel('Time step [$s$]', 'Interpreter', 'latex');
ylabel('Output', 'Interpreter', 'latex');
grid on;
hold off;


%% Membership functions
fidMf = fopen('mf.txt');
tMf = textscan(fidMf, '%s%f%f%f', 'Delimiter', ';');
fclose(fidMf);

NB = zeros(3,1);
NM = zeros(3,1);
NS = zeros(3,1);
ZO = zeros(3,1);
PS = zeros(3,1);
PM = zeros(3,1);
PB = zeros(3,1);

for i= 1:size(tMf,2)-1
   NB(i) = tMf{i+1}(1);
   NM(i) = tMf{i+1}(2);
   NS(i) = tMf{i+1}(3);
   ZO(i) = tMf{i+1}(4);
   PS(i) = tMf{i+1}(5);
   PM(i) = tMf{i+1}(6);
   PB(i) = tMf{i+1}(7);
end

% Plot
figure(4);
fis = mamfis;
fis = addInput(fis, [-1.5 1.5], 'Name', 'Input');
fis = addMF(fis,'Input','trimf',NB,'Name','NB');
fis = addMF(fis,'Input','trimf',NM,'Name','NM');
fis = addMF(fis,'Input','trimf',NS,'Name','NS');
fis = addMF(fis,'Input','trimf',ZO,'Name','ZO');
fis = addMF(fis,'Input','trimf',PS,'Name','PS');
fis = addMF(fis,'Input','trimf',PM,'Name','PM');
fis = addMF(fis,'Input','trimf',PB,'Name','PB');
plotmf(fis, 'input', 1);


uiwait(helpdlg('Examine the figures, then click OK to finish.'));
