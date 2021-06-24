# Prerequisites:
# 1. FSI simulation finished --> output file benchmark_output.txt
# 2. Run python code extractFlowResults.py on benchmark_output.txt --> flowResults.txt
# 3. Check that files flowResults.txt and HQ_curve.mat are present in the working
# directory
# N.B. If the simulation was restarted, combine the flowResults.txt obtained from the differnt output file benchmark_output.txt in a unique file

import numpy as np
import math
import matplotlib.pyplot as plt

## PUT HERE THE TESTED HEAD PRESSURE, AS SET IN DATAFILE (default H=50 mmHg)
H = 50.0 # mmHg
print("Head pressure H: ",H, "mmHg")

## LOAD HQ DATA
# string of the file with experimental HQ data
HQFile = open("../data/HQ_curve.csv")
HQdata = np.loadtxt(HQFile, delimiter=",",skiprows=1)
# print(HQdata)

h = HQdata[:,0] # head pressure data, mmHg
q = HQdata[:,1] # flow rate data, lpm

# we take the flow data with the pressure value closest to H in absolute
# value
Q_data = q[abs(h-H)==min(abs(h-H))]
print("Experimental flow: ",Q_data[0], "lpm")

## LOAD FLOW RATE NUMERICAL RESULTS
# string of the file with flow data, see Point 2 of prerequisites
resultsFile = open("flowResults.txt")
results = np.loadtxt(resultsFile, delimiter=",",skiprows=1)
outflow = results[:,2]

# time settings
T  = 0.025 # s
dt = 0.0002 # s
f = 120. # Hz
tau = 1./f
nT = int(round(T/dt))
nTau = int(math.ceil(tau/dt))

# compute Q_sim as average in time of Q results during last period of
# oscillation
# mid-point quadrature rule
Q_sim = np.mean(outflow[-nTau:]) # lpm
print("Predicted flow: ",Q_sim, "lpm")
# alternative: trapezoidal quadrature rule
#Q_sim2 = 0.5*sum(dt*(outflow[-nTau:-1]+outflow[-nTau+1:]))/(tau)
#pprint("Predicted flow =  ",Q_sim2, "lpm")


## VALIDATION ERROR

# absolute error
error_abs = abs(Q_sim-Q_data)
print("Absolute error: ",error_abs[0], " lpm")

# relative error
error_rel = abs(Q_sim-Q_data)/abs(Q_data)
print('Relative error: ',error_rel[0])

## PLOT HQ CURVE + NUMERICAL RESULT


markerData='o'
plt.plot(q,h, markerData,label="Experimental".format(markerData),color='gray',
         markersize=15, linewidth=5, markerfacecolor='gray',
         markeredgecolor='black', markeredgewidth=2)
markerResult='x'
plt.plot(Q_sim,H, markerResult,label="Simulation".format(markerResult),color='red',
         markersize=15, linewidth=16, markerfacecolor='red',
         markeredgecolor='red', markeredgewidth=2)

plt.legend(numpoints=1,fontsize=18)
plt.xlim(0, 4.0)
plt.ylim(35,70)
plt.xlabel('Flow rate [l/min]',fontsize=18)
plt.ylabel('Head pressure [mmHg]',fontsize=18)
plt.show();
