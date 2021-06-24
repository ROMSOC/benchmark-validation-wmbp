# Model validation in Wave Membrane Blood Pumps

## Summary
This benchmark provides a test case for model validation in Wave Membrane Blood Pumps (WMBPs), developed at CorWave SA.

## Description
The aim of this benchmark consists in running Fluid-Structure Interaction (FSI) simulations using the proposed Extended Finite Element Method (XFEM) and compare the predicted hydraulic output of WMBPs against real experimental flow-pressure data, said HQ curve.

## Directory structure
This repository is organized as follows:
```
├── benchmark
    ├── executable WMBP.exe
    ├── paramsFile: folder that contains dataFile and solverFile 
    ├── post-processing: Pythonsource files for post-processing
    ├── data: csv dataset for validation
    ├── restart: dataFile and other files to restart simulation
    └── post-processing: source files for post-processing  

├── source
    ├──  C++ main file used for compilation of the benchmark

├── documentation: 
    ├── introduction to benchmark, step-by-step procedure, computer requirements.

├── dockerFile to build image for LIFEV enviroment

├── runLife.sh to run LIFEV container to run the simulation 

├── CITATION
├── CONTRIBUTORS
├── CHANGELOG
├── LICENSE
├── README
└── .gitignore
```

## Disclaimer
In downloading this SOFTWARE you are deemed to have read and agreed to the following terms:
This SOFTWARE has been designed with an exclusive focus on civil applications. It is not to be used
for any illegal, deceptive, misleading or unethical purpose or in any military applications. This includes ANY APPLICATION WHERE THE USE OF THE SOFTWARE MAY RESULT IN DEATH,
PERSONAL INJURY OR SEVERE PHYSICAL OR ENVIRONMENTAL DAMAGE. Any redistribution of the software must retain this disclaimer. BY INSTALLING, COPYING, OR OTHERWISE
USING THE SOFTWARE, YOU AGREE TO THE TERMS ABOVE. IF YOU DO NOT AGREE TO
THESE TERMS, DO NOT INSTALL OR USE THE SOFTWARE

## Acknowledgments
<img src="/images/EU_Flag.png" alt="EU Flag"  width="150" height="100" />
The ROMSOC project has received funding from the European Union’s Horizon 2020 research and innovation programme under the Marie Skłodowska-Curie Grant Agreement No. 765374.
This repository reflects the views of the author(s) and does not necessarily reflect the views or policy of the European Commission. The REA cannot be held responsible for any use that may be made of the information this repository contains.
