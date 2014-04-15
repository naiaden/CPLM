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



//  cerr << "Options: -o outputprefix for class file" << endl;
//  cerr << " -d output directory, including trailing slash" << endl;
//  cerr << " -l read input filenames from list-file (one filename per line)" << endl;
//  cerr << " -u produce one unified encoded corpus (in case multiple corpora are specified)" << endl;
//  cerr << " -e extend specified class file with unseen classes" << endl;
//  cerr << " -U encode all unseen classes using one special unknown class" << endl;
//

	boost::filesystem::path targetDir("/tmp/vac");

	boost::filesystem::directory_iterator it(targetDir), eod;


	std::vector<boost::filesystem::path> background_input_files;
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
	{
	    if(is_regular_file(p) && p.extension() == ".txt")
	    {
	    	background_input_files.push_back(p);
	    }
	}

	ColibriPLM plm = ColibriPLM(0.5);
	plm.fit(background_input_files);


	return 0;
}
