#include <iostream>
#include <thread>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <classencoder.h>

#include <tclap/CmdLine.h>

#include "utils.hpp"
#include "PLM.hpp"


const int number_of_threads = 10;

void call_from_thread(int thread_id) {
	std::cout << "Printed by thread " << thread_id << std::endl;
}

class ProgramOptions {
public:
	std::string _background_input_directory;
	std::string _foreground_input_directory;
	std::string _wp_output_directory;
	std::string _input_file_extension;

	double _weight;
	int _maxn;
	int _em_iterations;

	int _threads;

	int _verboseArgument;

	bool check_values() {

		assert(("" != _background_input_directory && "No background files input directory is specified"));

		assert((_weight >= 0 && _weight <= 1 && "The weight must be in the range [0.0,1.0]"));
		assert((_maxn >= 1 && "The max number of n cannot be less than 1"));
		assert((_em_iterations >= 1 && "The number of EM iterations cannot be less than 1"));

		assert((_threads >= 1 && "The number of threads cannot be less than 1"));

		return true;
	}

	ProgramOptions(int argc, char** argv) {
		try {
				TCLAP::CmdLine cmd("A C++ implementation of the Parsimonious Language Model", ' ', "0.00001");

				TCLAP::ValueArg<std::string> background_input_directory("b","background","the directory with background files",false,"","string", cmd);
				TCLAP::ValueArg<std::string> foreground_input_directory("f","foreground","the directory with foreground files",false,"","string", cmd);
				TCLAP::ValueArg<std::string> wp_output_directory("o","output","the directory to write the word probabilities per foreground file to",false,"","string", cmd);
				TCLAP::ValueArg<std::string> input_file_extension("e","extension","only use file with this extension (only works with -b)",false,".txt","string", cmd);


				TCLAP::ValueArg<double> weight("w","weight","the mixture parameter",false,0.5,"double", cmd);
				TCLAP::ValueArg<int> maxn("n","maxn","the max number of n for generating n-grams",false,3,"int", cmd);
				TCLAP::ValueArg<int> em_iterations("i","iterations","the number of iterations for EM",false,50,"int", cmd);



				TCLAP::ValueArg<int> threads("t","threads","number of threads", false, 1, "int", cmd);

				TCLAP::ValueArg<int> verboseArgument("V","verbose","print verbose", false, 0, "int", cmd);



				cmd.parse( argc, argv );




				_background_input_directory = background_input_directory.getValue();
				_foreground_input_directory = foreground_input_directory.getValue();
				_wp_output_directory = wp_output_directory.getValue();
				_input_file_extension = input_file_extension.getValue();

				_weight = weight.getValue();
				_maxn = maxn.getValue();
				_em_iterations = em_iterations.getValue();

				_threads = threads.getValue();

				_verboseArgument = verboseArgument.getValue();

				check_values();

			} catch (TCLAP::ArgException &e)  // catch any exceptions
			{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
	}


};

int main(int argc, char** argv) {


	ProgramOptions program_options(argc, argv);




	std::thread t[program_options._threads];

	for (int i : range(program_options._threads)) {
		t[i] = std::thread(call_from_thread, i);
	}

	std::cout << "Printed from the main thread" << std::endl;

	for (auto& thread : t) {
		thread.join();
	}

	/// BACKGROUND

	boost::filesystem::path background_dir(program_options._background_input_directory);
	boost::filesystem::directory_iterator bit(background_dir), beod;

	std::vector<boost::filesystem::path> background_input_files;
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(bit, beod))
	{
	    if(is_regular_file(p) && p.extension() == program_options._input_file_extension)
	    {
	    	background_input_files.push_back(p);
	    }
	}

	ColibriPLM plm = ColibriPLM(0.5);
	plm.create_background_model(background_input_files);

	/// FOREGROUND

	boost::filesystem::path foreground_dir("/tmp/vac1");
	boost::filesystem::directory_iterator fit(foreground_dir), feod;

	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, feod))
	{
	    if(is_regular_file(p) && p.extension() == program_options._input_file_extension)
	    {
	    	plm.create_document_model(p);
//	    	auto word_probs =
//	    	for (auto p : word_probs)
//	    	{
//	    		std::cout << p.second << std::endl;
//	    	}
	    }
	}

	return 0;
}
