import sys
import math
import numpy as np


filepath = sys.argv[1]
output_filename = 'flowResults.txt'

timeData_not_found = True

with open(filepath) as fp:
    line = fp.readline()
    cnt = 1
    while line and timeData_not_found:
        if line.split('=')[0].rstrip() == 'Initial time':
            print 'Line {}: {}'.format(cnt, line.strip())
            init_time = float(line.split('=')[1].strip())
        if line.split('=')[0].rstrip() == 'End time':
            print 'Line {}: {}'.format(cnt, line.strip())
            end_time = float(line.split('=')[1].strip())
        if line.split('=')[0].rstrip() == 'TimeStep':
            print 'Line {}: {}'.format(cnt, line.strip())
            dt = float(line.split('=')[1].strip())
        if 'end_time' in vars() and 'dt' in vars():
            timeData_not_found = False
        line = fp.readline()
        cnt += 1

    timeSize = int(round(end_time / dt))
    init = int(round(init_time / dt))
    print timeSize
    print init_time
    print init

    VAR_list = "iter, div , outflow,  p_out"
    ITER_list = np.zeros(timeSize)
    DIV_list = np.zeros(timeSize)
    OUT_list = np.zeros(timeSize)
    P_OUT_list = np.zeros(timeSize)
    
    t_i = -1 + init
    first = True

    solver = None
    lastIter = False
     
    
    while (line):
        if line.strip():
            
            # change of timestep            
            if ( line.lstrip()[0:2]=='--'):
                if ( (((line.strip()).split('-')[2]).strip()).split(' ')[0]=="At"  ):
                    print("Line {}: {}".format(cnt, line.strip()))
                    time = float((((line.strip()).split('-')[2]).strip()).split(' ')[2])
                    if (not first):
                        t_i = int(round(time/dt)-1)
                        print(t_i)
                    first = False
                    lastIter = False
                    it = None
            
            # iterations        
            if ( (line.split(' ')[0]).lstrip()=="Using" and (line.split(' ')[1]).lstrip()=="parameters" ):
                print("Line {}: {}".format(cnt, line.strip()))
                if ( ((line.split(' ')[3]).strip())[0] == "A" ): # for AtzecOO style file 
                    solver = "Aztec"
                elif ( ((line.split(' ')[3]).strip())[0] == "B" ): # for Belos style file 
                    solver = "Belos"
            if (solver == "Aztec"):                              
                if ((line.split(':')[0]).lstrip()=="total iterations" ):
                    print("Line {}: {} -- {}".format(cnt, line.strip(), t_i))
                    v = float((line.split(':')[1]).strip())
                    ITER_list[t_i] += v
            elif (solver == "Belos"):
                while ( line and lastIter == False):
                    if ( (line.split(' ')[0]).lstrip()=="Iter" ):                    
                        print( (line.split(',')[0]).split(' ')[-1] )
                        it = float( (line.split(',')[0]).split(' ')[-1].strip() )
                        print("Line {}: {}".format(cnt, line.strip()))
                    elif ( it is not None):
                        lastIter = True
                        ITER_list[t_i] = it   
                    line = fp.readline()
                    cnt += 1                             
            
            # divergence
            if ((line.split(' ')[0]).strip()=="int(Omega)" ):
                print("Line {}: {}".format(cnt, line.strip()))
                v = float(  ( ((line.split(':')[1]).split(' '))[1] ).strip()  )
                DIV_list[t_i] = v
            
            # outflow
            if ((line.split(' ')[0]).strip()=="int(Gamma_out)" and (line.split(' ')[1]).strip()=="u"):
                print("Line {}: {}".format(cnt, line.strip()))
                v = float((((line.split(':')[1]).split(' '))[1]).strip())
                OUT_list[t_i] = v
                											    
            # pressure at the outlet
            if ((line.split(' ')[0]).strip()=="int(Gamma_out)" and (line.split(' ')[1]).strip()=="p/area"):
                print("Line {}: {}".format(cnt, line.strip()))
                v = float((((line.split(':')[1]).split(' '))[1]).strip())
                P_OUT_list[t_i] = v
                                												
        line = fp.readline()
        cnt += 1
    fp.close()

# final output dataset
data = np.concatenate( (
						np.reshape(ITER_list,(timeSize,1)), 
						np.reshape(DIV_list, (timeSize,1)),
						np.reshape(OUT_list,(timeSize,1)),
						np.reshape(P_OUT_list, (timeSize,1))
						),axis=1 )
print "\n"
print(np.shape(data))
print(data)
np.savetxt (output_filename, data, delimiter=',', newline='\n',header=VAR_list)

