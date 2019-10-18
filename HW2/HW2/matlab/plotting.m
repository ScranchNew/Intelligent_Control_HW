%% Output data from controllers
fidData = fopen('flc.txt');
tData = textscan(fidData, '%f%f%f%f%f%f', 'Delimiter', ';', 'headerlines', 1);
fclose(fidData);

fidData = fopen('err.txt');
eData = textscan(fidData, '%f%f%f%f%f%f', 'Delimiter', ';', 'headerlines', 1);
fclose(fidData);

fidData = fopen('ctrl.txt');
cData = textscan(fidData, '%f%f%f%f%f%f', 'Delimiter', ';', 'headerlines', 1);
fclose(fidData);

flcOut = zeros(500,6);

for i= 1:size(tData{1},1)
    for j = 1:size(tData,2)
        flcOut(i, j) = tData{j}(i);
    end
end

% Plot
%% Compare measures
for i = 1:3
    figure;
    x = 1:500;
    for j = 1:2
        hold on;
        plot(x, flcOut(:,i + (j-1)*3));
    end
    legend('Plaus', 'Belief');
    if i == 1
        title('Step response: {Tr,Os,Ts}:[1,10,2]');
    elseif i == 2
        title('Step response: {Tr,Os,Ts}:[2,1,10]');
    else
        title('Step response: {Tr,Os,Ts}:[10,2,1]');
    end
    xlabel('Time step [$s$]', 'Interpreter', 'latex');
    ylabel('Output', 'Interpreter', 'latex');
    grid on;
    hold off;
end

%% Compare preferences
for i = 1:2
    figure;
    x = 1:500;
    for j = 1:3
        hold on;
        plot(x, flcOut(:,(i-1)*3 + j));
    end
    legend('{Tr,Os,Ts}:[1,10,2]', '{Tr,Os,Ts}:[2,1,10]', '{Tr,Os,Ts}:[10,2,1]');
    if i == 1
        title('Step response: Plausability measure');
    else
        title('Step response: Belief measure');
    end
    xlabel('Time step [$s$]', 'Interpreter', 'latex');
    ylabel('Output', 'Interpreter', 'latex');
    grid on;
    hold off;
end

%% Error
% 
% flcErr = eData{1,1};
% 
% flc_dErr = zeros(500,1);
% 
% for i= 1:size(eData{1},1)
%     if i > 1
%         flc_dErr(i) = (eData{1,1}(i) - eData{1,1}(i-1))/0.05;
%     end
% end
% 
% %% Control
% flcCtrl = zeros(500,1);
% 
% for i= 1:size(cData{1},1)
%     flcCtrl(i) = cData{1,1}(i);
% end
% 
% % Plot
% figure(3);
% x = 1:500;
% hold on;
% plot(x, flcErr);
% plot(x, flc_dErr);
% plot(x, flcCtrl);
% axis([0 100 -2 2]);
% legend('Err', 'd_Err', 'Control');
% title('Control Signal');
% xlabel('Time step [$s$]', 'Interpreter', 'latex');
% ylabel('Output', 'Interpreter', 'latex');
% grid on;
% hold off;

uiwait(helpdlg('Examine the figures, then click OK to finish.'));
