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

int main(int args, char** argv) {
	std::thread t[number_of_threads];

	for (int i : range(10)) {
		t[i] = std::thread(call_from_thread, i);
	}

	std::cout << "Printed from the main thread" << std::endl;

	for (auto& thread : t) {
		thread.join();
	}

	/// BACKGROUND

	boost::filesystem::path background_dir("/tmp/vac");
	boost::filesystem::directory_iterator bit(background_dir), eod;

	std::vector<boost::filesystem::path> background_input_files;
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(bit, eod))
	{
	    if(is_regular_file(p) && p.extension() == ".txt")
	    {
	    	background_input_files.push_back(p);
	    }
	}

	ColibriPLM plm = ColibriPLM(0.5);
	plm.fit(background_input_files);

	/// FOREGROUND

	boost::filesystem::path foreground_dir("/tmp/vac");
	boost::filesystem::directory_iterator fit(foreground_dir), eod;

	std::vector<boost::filesystem::path> foreground_input_files;
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, eod))
	{
	    if(is_regular_file(p) && p.extension() == ".txt")
	    {
	    	foreground_input_files.push_back(p);
	    }
	}


	return 0;
}
