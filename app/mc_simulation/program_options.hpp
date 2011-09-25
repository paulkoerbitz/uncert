#ifndef command_line_parameters
#define command_line_parameters

#include <string>

#include <ql_extensions.hpp>

namespace qe = QuantLibExt;

class ProgramOptions {
  public:
    ProgramOptions() : didYouParseYet_(false) {}

    void parseCommandline(int ac, char** av) {
        model_ = std::string(av[1]);
        parameterFile_ = std::string(av[2]);
        nPaths_ = atoi(av[3]);
        nHedges_ = atoi(av[4]);
        nPathsInnerMC_ = atoi(av[5]);
        nPathPoints_ = atoi(av[6]);
        r0_ = atof(av[7]);
        s0_ = atof(av[8]);
        L0_ = atof(av[9]);
        contractMaturity_ = atoi(av[10]);
        rnStockVol_ = atof(av[11]);
        rnStockExp_ = atof(av[12]);
        rnIrSpeed_  = atof(av[13]);
        rnIrLevel_ = atof(av[14]);;
        rnIrVol_ = atof(av[15]);;
        rnIrExp_ = atof(av[16]);;
        rnCorrelation_ = atof(av[17]);;
        transCosts_ = atof(av[18]);; 
        outfilename_ = std::string(av[19]);
        seed_ = atoi(av[20]);

        doHedging_ = nHedges_ > 0;

        didYouParseYet_ = true;
    }

    boost::shared_ptr<MCMC_parameters> getRiskNeutralParameters() const {
        checkParsed();
        boost::shared_ptr<MCMC_parameters> p_RnParas;
        if (model_ == "BSVasicek") 
            p_RnParas.reset(new BlackScholesVasicek_MCMC_parameters(
                0.0,rnStockVol_,rnIrSpeed_
               ,rnIrLevel_,rnIrVol_,rnCorrelation_));
        else if (model_ == "CEV_CKLS")
            p_RnParas.reset(new CEV_CKLS_MCMC_parameters(
                0.0,rnStockVol_,rnStockExp_,rnIrSpeed_
               ,rnIrLevel_,rnIrVol_,rnIrExp_,rnCorrelation_));
        else 
            QL_FAIL("ProgramOptions: Don't know how to create parameters"
                   + std::string(" for model ") + model_);
        return p_RnParas;
    }

    std::string model() const {
        checkParsed();
        return model_;
    }

    std::string MCMC_parameter_filename() const {
        checkParsed();
        return parameterFile_;
    }

    std::string outputFilename() const {
        checkParsed();
        return outfilename_;
    }

    unsigned getNumberOfPaths() const {
        checkParsed();
        return nPaths_;
    }

    unsigned nPathsInitialMc() const {
        checkParsed();
        return nPaths_*10;
    }

    unsigned getSeed() const {
        return seed_;
    }

    qe::HedgeTraits getHedgeTraits() const {
        checkParsed();
        QL_REQUIRE(doHedging_, 
            "If you don't want to hedge don't ask for the hedgeTraits!");
        qe::HedgeTraits ht;
        ht.dt = qe::rational(contractMaturity_,nHedges_);
        ht.nSamplesInnerMC = nPathsInnerMC_;
        ht.offset_S = 0.005;
        ht.offset_r = 0.002;
        return ht;
    }

    qe::ContractTraits getContractTraits() const {
        checkParsed();
        qe::ContractTraits ct(0.035,0.5,0.9);
        ct.initialContractStates.L = L0_;
        ct.initialContractStates.Ap = 1.1*L0_;
        ct.T = qe::rational(contractMaturity_);
        return ct;
    }

    qe::AssetPathTraits getAssetPathTraits() const {
        checkParsed();
        qe::AssetPathTraits apt;
        apt.dt = qe::rational(contractMaturity_,nPathPoints_);
        apt.t0 = qe::rational();
        apt.T = qe::rational(contractMaturity_);
        apt.initialAssetValues.S = s0_;
        apt.initialAssetValues.r = r0_;
        apt.initialAssetValues.intR = 0.0;
        return apt;
    }

    bool doHedging() const {
        checkParsed();
        return doHedging_;
    }

    void debugPrint() const {
        std::cout << std::string(78,'-') << std::endl ;
        std::cout << "model            : " << model_ << std::endl;
        std::cout << "parameterFile    : " << parameterFile_ << std::endl;
        std::cout << "numHedges        : " << nHedges_ << std::endl;
        std::cout << "nPaths           : " << nPaths_ << std::endl;
        std::cout << "nPathsInnerMC    : " << nPathsInnerMC_ << std::endl;
        std::cout << "nPathPoints      : " << nPathPoints_ << std::endl;
        std::cout << "r0               : " << r0_ << std::endl ;
        std::cout << "s0               : " << s0_ << std::endl ;
        std::cout << "L0               : " << L0_ << std::endl ;
        std::cout << "contractMaturity : " << contractMaturity_ << std::endl ;
        std::cout << "rnStockVol       : " << rnStockVol_ << std::endl ;
        std::cout << "rnStockExp       : " << rnStockExp_ << std::endl ;
        std::cout << "rnIrSpeed        : " << rnIrSpeed_ << std::endl ;
        std::cout << "rnIrLevel        : " << rnIrLevel_ << std::endl ;
        std::cout << "rnIrVol          : " << rnIrVol_ << std::endl ;
        std::cout << "rnIrExp          : " << rnIrExp_ << std::endl ;
        std::cout << "rnCorrelation    : " << rnCorrelation_ << std::endl ;
        std::cout << "transCosts       : " << transCosts_ << std::endl ;
        std::cout << "outfilename      : " << outfilename_ << std::endl ;
        std::cout << "seed             : " << seed_ << std::endl ;
        std::cout << "doHedging        : " << doHedging_ << std::endl ;
        std::cout << std::string(78,'-') << std::endl ;
        if (doHedging_) {
          std::cout << "hedgeDt          : " << getHedgeTraits().dt << std::endl;
          std::cout << std::string(78,'-') << std::endl ;
        }

    }

  private:
    void checkCommandlineParameters(int ac, char** av) const {
        QL_REQUIRE(ac == 21, 
        "USAGE: model parameterFile nPaths nHedges nPathsInnerMC nPathPoints r0 S0 L0 contractMaturity rnStockVol rnStockExp rnIrSpeed rnIrLevel rnIrVol rnIrExp rnCorrelation transactionCosts ouputFilename seed");
    }

    void checkParsed() const {
        QL_REQUIRE(didYouParseYet_,"You have to parse the command line before accessing this option!");
    }

    bool didYouParseYet_;
    std::string model_;
    std::string parameterFile_;
    unsigned nHedges_;
    unsigned nPaths_;
    unsigned nPathsInnerMC_;
    unsigned nPathPoints_;
    double r0_;
    double s0_;
    double L0_;
    unsigned contractMaturity_;
    double rnStockVol_;
    double rnStockExp_;
    double rnIrSpeed_;
    double rnIrLevel_;
    double rnIrVol_;
    double rnIrExp_;
    double rnCorrelation_;
    double transCosts_; 
    std::string outfilename_;
    unsigned seed_;
    bool doHedging_;
};

#endif
