
CPPPATH = ['lib/ql_extensions',
           'lib/FLENS-lite',
           'lib/ool-0.2.0']

LIBPATH = ['lib/ql_extensions','lib/FLENS-lite']
LIBS = ['gsl','flens','QuantLib','ql_extensions','boost_program_options']

env = Environment(CC = 'gcc',
                  CCFLAGS = '-g -Wall',
                  CPPPATH = CPPPATH,
                  LIBPATH = LIBPATH,
                  LIBS = LIBS)

env.Library('lib/ql_extensions/ql_extensions',
            ['lib/ql_extensions/mcmc/mcmc_algorithms.cpp',
             'lib/ql_extensions/mcmc/mcmc_models.cpp',
             'lib/ql_extensions/utils/filereader.cpp',
             'lib/ql_extensions/utils/pathparser.cpp'])

env.Program('bin/mcmc_estimation',
            'app/mcmc_estimation/mcmc_estimation.cpp',
            CPPPATH = CPPPATH + ['app/mcmc_estimation'])
