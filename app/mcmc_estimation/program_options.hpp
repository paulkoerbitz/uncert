#ifndef __program_options__hpp__
#define __program_options__hpp__

#include <set>
#include <tr1/unordered_map>
#include <boost/program_options.hpp>

#include <ql_extensions.hpp>

namespace po = boost::program_options;

namespace qe = QuantLibExt;

typedef std::set<std::string> Set;
typedef std::tr1::unordered_map<std::string, unsigned> Map;

class ProgramOptions {
public:
	ProgramOptions(int ac, char** av, const std::string cfg_filename)
		: config("Configuration"), opts_valid(false)
	{
		add_options();

		cmdline_options.add(config);
		cfgfile_options.add(config);
		po::store(po::command_line_parser(ac, av).options(cmdline_options).run(), vm);

		ifstream cfg_file(cfg_filename.c_str());
		po::store(po::parse_config_file(cfg_file, cfgfile_options), vm);

		po::notify(vm);

		plength["BS"] = 2;
		plength["Cev"] = 3;
		plength["Vasicek"] = 3;
		plength["Ckls"] = 4;
		plength["BsVasicek"] = 6;
		plength["CevCkls"] = 8;

		for (Map::const_iterator i=plength.begin(); i!=plength.end(); ++i) {
			valid_models.insert(i->first);
		}

		check_options();
	}

	bool options_valid() const { return opts_valid; }
	void show_help() const { std::cout << config << std::endl; }

	void print_status() const {
		std::cout << "model        : " << model() << std::endl;
		std::cout << "algo         : " << algo() << std::endl;
		std::cout << "file         : " << file() << std::endl;
		std::cout << "outfile      : " << outfile() << std::endl;
		std::cout << "dt           : " << dt() << std::endl;
		std::cout << "N            : " << N() << std::endl;
		std::cout << "burn         : " << burn() << std::endl;
		std::cout << "tune         : " << tune() << std::endl;
		std::cout << "thin         : " << thin() << std::endl;
		std::cout << "start        : ";
		print_vector(start());
		std::cout << "lb           : ";
		print_vector(lb());
		std::cout << "ub           : ";
		print_vector(ub());

		if (vm.count("mean")) {
			std::cout << "mean         : ";
			print_vector(mean());
		}
		if (vm.count("hessian")) {
			std::cout << "hessian      : " << hessian() << std::endl;
		}
	}

	template <class T>
	static void print_vector(const std::vector<T>& v) {
		std::copy(v.begin(),v.end(), std::ostream_iterator<T>(std::cout, ", "));
		std::cout << std::endl;
	}

	void print_errors() const { std::cout << errors.str() << std::endl; }

	std::string model() const {
		return vm["model"].as<std::string>();
	}
	std::string file() const {
		return vm["file"].as<std::string>();
	}
	std::string outfile() const {
		return vm["outfile"].as<std::string>();
	}
	std::string algo() const {
		return vm["algo"].as<std::string>();
	}
	double dt() const {
		return vm["dt"].as<double>();
	}
	unsigned N() const {
		return vm["N"].as<unsigned>();
	}
	unsigned burn() const {
		return vm["burn"].as<unsigned>();
	}
	double tune() const {
		return vm["tune"].as<double>();
	}
	unsigned thin() const {
		return vm["thin"].as<unsigned>();
	}
	std::vector<double> start() const {
		return vm["start"].as< std::vector<double> >();
	}
	std::vector<double> mean() const {
		return vm["mean"].as< std::vector<double> >();
	}
	std::vector<double> lb() const {
		return vm["lb"].as< std::vector<double> >();
	}
	std::vector<double> ub() const {
		return vm["ub"].as< std::vector<double> >();
	}
	std::string hessian() const {
		return vm["hessian"].as<std::string>();
	}
	bool isSinglePathModel() const {
		return  model() == "BS"  || model() == "Vasicek"
			|| model() == "Cev" || model() == "Ckls";
	}
	
private:
	void add_options() {
		config.add_options()
			("help", "produce help message")
			("model", po::value<std::string>(), "the asset model to use")
			("algo", po::value<std::string>(), "MCMC algorithm to use (indep or rwalk)")
			("file", po::value<std::string>(), "file containing the time-series")
			("outfile", po::value<std::string>(), "file to which results are written")
			("dt", po::value<double>(), "time between observations in years")
			("N", po::value<unsigned>(), "number of scenarios to generate")
			("burn", po::value<unsigned>(), "number of burn-in scenarios to skip")
			("tune", po::value<double>(), "tuning parameter")
			("thin", po::value<unsigned>(), "every thin-th scenario will be kept")
			("start", po::value< std::vector<double> >(),
			 "start values for the parameters")
			("mean", po::value< std::vector<double> >(),
			 "mean value of proposal distribution, used for independent MH, can be omitted otherwise")
			("lb", po::value< std::vector<double> >(), "lower bound for parameters")
			("ub", po::value< std::vector<double> >(), "upper bound for parameters")
			("hessian", po::value<std::string>(),
			 "filename for file containing hessian matrix at maximum likelihood estimate (optional)")
			;
	}

	void check_options() {
		opts_valid = true;

		check_help()
			&& check_required_args()
			&& check_model_valid()
			&& check_files_exist()
			&& check_parameter_lengths()
			&& check_mean();
	}

	bool check_help() {
		if (vm.count("help")) {
			opts_valid = false;
			errors << config << std::endl;
		}
		return opts_valid;
	}

	bool required_args_given() {
		return  vm.count("model") && vm.count("algo") && vm.count("file") &&
			vm.count("outfile") &&
			vm.count("dt") && vm.count("N") && vm.count("burn") &&
			vm.count("tune") && vm.count("thin") && vm.count("start") &&
			vm.count("lb") && vm.count("ub");
	}

	bool check_required_args() {
		if ( not required_args_given() ) {
			opts_valid = false;
			errors << "USAGE:\n" << config
				   << "\nAll optiosn are required except where noted." << std::endl;
		} 
		return opts_valid;
	}

	bool check_model_valid() {
		if (valid_models.find(model()) == valid_models.end()) {
			opts_valid = false;
			errors << "Invalid model: " << model() << std::endl;
			errors << "Valid choices are: ";
			std::copy(valid_models.begin(), valid_models.end(),
					  std::ostream_iterator<std::string>(errors,", "));
			errors << std::endl;
		}
		return opts_valid;
	}
	
	bool check_files_exist() {
		if (not qe::file_exists(file())) {
			opts_valid = false;
			errors << "the file " << file() << " does not exist." << std::endl;
		}
		if (vm.count("hessian") && (not qe::file_exists(hessian()))) {
			opts_valid = false;
			errors << "the file " << hessian() << " does not exist." << std::endl;
		}
		return opts_valid;
	}

	bool param_length_equals(unsigned n) const {
		return (start().size() == n) && (lb().size() == n) && (ub().size() == n);
	}

	bool check_plength(std::string model, unsigned n) {
		if (not param_length_equals(n)) {
			opts_valid = false;
			errors << "parameters must be of length " << n << " for "
				   << model << "." << std::endl;
		}
		return opts_valid;
	}

	bool check_parameter_lengths() {
		return check_plength(model(), plength[model()]);
	}

	bool check_mean() {
		if (model() == "indep") {
			if (mean().size() != plength[model()]) {
				opts_valid = false;
				errors << "For the independent Metropolis Hastings algorithm  "
					   << "mean must be specified and the same length as "
					   << "start, lb and ub." << std::endl;
			}
		}
		return opts_valid;
	}

    po::options_description cmdline_options;
    po::options_description cfgfile_options;    
    po::options_description config;
    po::variables_map vm;

	bool opts_valid;
	std::stringstream errors;

	Set valid_models;
	Map plength;
};

#endif
