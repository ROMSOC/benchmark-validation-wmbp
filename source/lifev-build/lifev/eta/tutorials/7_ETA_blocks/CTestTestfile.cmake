# CMake generated Testfile for 
# Source directory: /home/lifev/lifev-src/lifev/eta/tutorials/7_ETA_blocks
# Build directory: /home/lifev/lifev-build/lifev/eta/tutorials/7_ETA_blocks
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ETA_7_ETA_blocks_MPI_2 "/usr/bin/mpiexec" "-np" "2" "/home/lifev/lifev-build/lifev/eta/tutorials/7_ETA_blocks/ETA_7_ETA_blocks.exe" "-c")
set_tests_properties(ETA_7_ETA_blocks_MPI_2 PROPERTIES  FAIL_REGULAR_EXPRESSION "The following Teuchos::RCPNode objects were created" LABELS "ETA" PROCESSORS "2")
