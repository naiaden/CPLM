#include <iostream>
#include <thread>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <classencoder.h>

#include "utils.hpp"
#include "PLM.hpp"


const int number_of_threads = 10;

void call_from_thread(int thread_id) {
	std::cout << "Printed by thread " << thread_id << std::endl;
}

void write_probs_to_file(std::vector<std::pair<Pattern, double>> word_probs, ClassDecoder decoder, boost::filesystem::path file_name) {
	std::ofstream output_file(file_name.string());
	if( output_file ) {
		for(auto p : word_probs) {
			output_file << p.first.tostring(decoder) << "\t" << p.second << '\n';
		}
		output_file.flush();
		output_file.close();
	} else {
		std::cerr << "Cannot open " << file_name.string() << " to write word probs to!" << std::endl;
	}
}

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

	ColibriPLM plm = ColibriPLM(program_options);
	plm.create_background_model(background_input_files);

	/// FOREGROUND

	if(program_options._foreground_input_directory != "") {
		boost::filesystem::path foreground_dir(program_options._foreground_input_directory);
		boost::filesystem::directory_iterator fit(foreground_dir), feod;

		BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, feod))
		{
			if(is_regular_file(p) && p.extension() == program_options._input_file_extension)
			{
				std::vector<std::pair<Pattern, double>> document_word_probs = plm.create_document_model(p);

				boost::filesystem::path output_file_name = boost::filesystem::path(program_options._generated_output_directory + "/" + p.stem().string() + ".dwp");

				write_probs_to_file(document_word_probs, plm.getDecoder(), output_file_name);
			}
		}
	}

	return 0;
}
