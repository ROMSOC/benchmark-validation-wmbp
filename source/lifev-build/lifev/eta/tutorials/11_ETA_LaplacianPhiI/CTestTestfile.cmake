# CMake generated Testfile for 
# Source directory: /home/lifev/lifev-src/lifev/eta/tutorials/11_ETA_LaplacianPhiI
# Build directory: /home/lifev/lifev-build/lifev/eta/tutorials/11_ETA_LaplacianPhiI
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ETA_11_ETA_LaplacianPhiI_MPI_2 "/usr/bin/mpiexec" "-np" "2" "/home/lifev/lifev-build/lifev/eta/tutorials/11_ETA_LaplacianPhiI/ETA_11_ETA_LaplacianPhiI.exe" "-c")
set_tests_properties(ETA_11_ETA_LaplacianPhiI_MPI_2 PROPERTIES  FAIL_REGULAR_EXPRESSION "The following Teuchos::RCPNode objects were created" LABELS "ETA" PROCESSORS "2")
