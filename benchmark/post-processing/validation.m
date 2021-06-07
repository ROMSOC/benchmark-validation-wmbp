clear all;
clc;

% Prerequisites:
% 1. FSI simulation finished --> output file benchmark_output.txt
% 2. Run python code extractFlowResults.py on benchmark_output.txt --> flowResults.txt
% 3. Check that files flowResults.txt and HQ_curve.mat are present in the working
% directory

%% PUT HERE THE TESTED HEAD PRESSURE, AS SET IN DATAFILE (default H=50 mmHg)
H = 50.0; % mmHg

%% LOAD HQ DATA
% string of the file with experimental HQ data
HQFile = "HQ_curve.mat";
load(HQFile);
HQdata = table2array(HQ_data);

h = HQdata(:,1); % head pressure data, mmHg
q = HQdata(:,2); % flow rate data, lpm

% we take the flow data with the pressure value closest to H in absolute
% value
Q_data = q(abs(h-H)==min(abs(h-H)))

%% LOAD FLOW RATE NUMERICAL RESULTS
% string of the file with flow data, see Point 2 of prerequisites
resultsFile = "flowResults.txt";
results = readtable(resultsFile)
outflow = results.outflow;

% time settings
T  = 0.025; % s
dt = 0.0002; % s
f = 120; % Hz
tau = 1/f;
nT = round(T/dt);
nTau = ceil(tau/dt);  

% compute Q_sim as average in time of Q results during last period of
% oscillation
% mid-point quadrature rule
Q_sim = mean(outflow(nT-ntau:nT)) % lpm
% alternative: trapezoidal quadrature rule
% Q_sim2 = 0.5*sum(dt*(outflow(nT-nTau:nT-1)+outflow(nT-nTau+1:nT)))/(tau)

%% PLOT HQ data curve + simulation result point
titleSize = 42;
labelSize = 36;
tickSize = 26;
lineSize = 5;
mksz = 50;
lsz = 8;

figure;
plot(q,h,'.k','Markersize',30)
hold on
plot(Q_sim,H,'rx','Linewidth',lsz,'Markersize',mksz)
grid on;
xlabel('Flow rate [l/min]','fontsize',labelSize,'fontweight','bold');
ylabel('Head pressure [mmHg]','fontsize',labelSize,'fontweight','bold');
set(gcf, 'Units', 'Normalized', 'OuterPosition', [0.2, 0.2, 0.35, 0.5]);
set(gca,'FontSize',tickSize);
xlim ([0,3.5]);
ylim ([35,70]);
lg = legend('  exp','  sim','Location','NorthEast');
set(lg,'FontSize',42);

%% VALIDATION ERROR

% absolute error
error_abs = abs(Q_sim-Q_data)
% relative error
error_rel = abs(Q_sim-Q_data)/abs(Q_data)
