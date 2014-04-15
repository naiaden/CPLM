/*
 * PLM.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: louis
 */

#include "PLM.hpp"

#include <iostream>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

PLM::PLM(double interpolation_factor) : _interpolation_factor(interpolation_factor) {

}

ColibriPLM::ColibriPLM(double interpolation_factor) : PLM(interpolation_factor) {

}

void ColibriPLM::fit(std::vector<boost::filesystem::path> input_files) {

	std::vector<std::string> file_names = std::vector<std::string>();

	for(auto i : input_files)
	{
		file_names.push_back(i.string());
	}

	std::cout << "Found " << file_names.size() << " files" << std::endl;

	ClassEncoder class_encoder = ClassEncoder();


	class_encoder.build(file_names);
	class_encoder.save("/tmp/tmpout/somefilename.colibri.cls");

	std::string dat_output_file = "/tmp/tmpout/somefilename.colibri.dat";

	for(auto f : file_names)
	{
		class_encoder.encodefile(f, dat_output_file, false, false, true);
	}

	///

	ClassDecoder class_decoder = ClassDecoder("/tmp/tmpout/somefilename.colibri.cls");

	///

	// See https://github.com/proycon/colibri-core/blob/master/src/patternmodeller.cpp#L212
	PatternModelOptions options = PatternModelOptions();
	options.MAXLENGTH = 1;
	options.DOSKIPGRAMS = false;
	options.DOREVERSEINDEX = false;
	options.QUIET = true;

	PatternModel<uint32_t> output_model = PatternModel<uint32_t>();
	output_model.train(dat_output_file, options, nullptr);

	for(auto &p : output_model)
	{
		std::cout << p.first.tostring(class_decoder) << "," << output_model.occurrencecount(p.first) << std::endl;
	}
}

ColibriPLM::~ColibriPLM() {
	// TODO Auto-generated destructor stub
}
