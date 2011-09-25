Install
=======
The following packages are required (Ubuntu)

libatlas3gf-base libatlas-base-dev liblapack-dev  
scons
libgsl0-dev
libquantlib0-dev
libboost-program-options1.40-dev
libboost1.40-dev


FLENS
======
get it from http://flens.sourceforge.net and add it under lib/FLENS-lite

You have to add it your LD_LIBRARY_PATH. In the FLENS home directory do:
$ export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH


You should be able to compile with
$ scons


Overview
========

The main program is in

app/mcmc_estimation/mcmc_estimation.cpp

It uses the classes in lib/ql_extensions/mcmc/mcmc_algorithms.hpp and mcmc_models.hpp.

Command line parameters are evaluated with the class ProgramOptions which is in app/mcmc_estimation/program_settings.hpp .
