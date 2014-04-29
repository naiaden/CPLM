/*
 * utils.hpp
 *
 *  Created on: Apr 15, 2014
 *      Author: louis
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <tclap/CmdLine.h>

#include <fstream>

#include <boost/filesystem.hpp>

/*
 * Code for C++1y generator taken from
 * http://en.wikipedia.org/wiki/Generator_%28computer_programming%29#C.2B.2B
 */
class range
{
    private:
    int last;
    int iter;

    public:
    range(int begin, int end):
        last(end),
        iter(begin)
    {}
    range(int end):
        last(end),
        iter(0)
    {}

    // Iterable functions
    const range& begin() const { return *this; }
    const range& end() const { return *this; }

    // Iterator functions
    bool operator!=(const range&) const { return iter < last; }
    void operator++() { ++iter; }
    int operator*() const { return iter; }
};

class ProgramOptions {
	public:
	std::string _background_input_directory;
	std::string _foreground_input_directory;
	std::string _generated_output_directory;
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

				TCLAP::ValueArg<std::string> background_input_directory("b","background","the directory with background files",true,"","string", cmd);
				TCLAP::ValueArg<std::string> foreground_input_directory("f","foreground","the directory with foreground files",false,"","string", cmd);
				TCLAP::ValueArg<std::string> generated_output_directory("G","generated","the directory to save intermediate (generated) files",false,".","string", cmd);
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
				_generated_output_directory = generated_output_directory.getValue();
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



#endif /* UTILS_HPP_ */
