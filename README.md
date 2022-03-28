<img src="images/romsoclogo-logo.png" alt="ROMSOC logo"  width="150"/>

# Model validation in Wave Membrane Blood Pumps 
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5171806.svg)](https://doi.org/10.5281/zenodo.5171806) [![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/ROMSOC/benchmark-validation-wmbp/HEAD?labpath=Benchmark.ipynb)

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

├── dockerFile used to build Docker image for LIFEV enviroment

├── runLife.sh to run LIFEV container to run the simulation 

├── CITATION
├── CONTRIBUTORS
├── CHANGELOG
├── LICENSE
├── README
└── .gitignore
```

## Run Jupyter notebooks
The entire benchmark repository can be executed in a provided Docker container where a full installation of LIFEV is available. Once you have clone or downloaded this repository, to build the container just type
```bash
docker build -t benchmark-validation-wmbp . 
```
and for running it locally:
```bash
docker run -it --rm -p 8888:8888 benchmark-validation-wmbp jupyter-lab --ip=0.0.0.0 --port=8888 --allow-root
```

Alternatively, user-friendly Jupyter Notebooks could be used to run different benchmarks on the cloud. For instance, the benchmark is available at:
[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/ROMSOC/benchmark-validation-wmbp/HEAD?labpath=Benchmark.ipynb). Please, notice that mybinder cloud computations are limited to 2GB of RAM memory.

## Disclaimer
In downloading this SOFTWARE you are deemed to have read and agreed to the following terms:
This SOFTWARE has been designed with an exclusive focus on civil applications. It is not to be used
for any illegal, deceptive, misleading or unethical purpose or in any military applications. This includes ANY APPLICATION WHERE THE USE OF THE SOFTWARE MAY RESULT IN DEATH,
PERSONAL INJURY OR SEVERE PHYSICAL OR ENVIRONMENTAL DAMAGE. Any redistribution of the software must retain this disclaimer. BY INSTALLING, COPYING, OR OTHERWISE
USING THE SOFTWARE, YOU AGREE TO THE TERMS ABOVE. IF YOU DO NOT AGREE TO
THESE TERMS, DO NOT INSTALL OR USE THE SOFTWARE

## Acknowledgments
<img src="images/EU_Flag.png" alt="EU Flag"  width="150" height="100" />
The ROMSOC project has received funding from the European Union’s Horizon 2020 research and innovation programme under the Marie Skłodowska-Curie Grant Agreement No. 765374.
This repository reflects the views of the author(s) and does not necessarily reflect the views or policy of the European Commission. The REA cannot be held responsible for any use that may be made of the information this repository contains.
