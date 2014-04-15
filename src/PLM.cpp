/*
 * PLM.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: louis
 */

#include "PLM.hpp"

#include <iostream>



PLM::PLM(double interpolation_factor) : _interpolation_factor(interpolation_factor), _corpus_frequency(0) {

}

ColibriPLM::ColibriPLM(double interpolation_factor) : PLM(interpolation_factor) {
	_class_encoder = ClassEncoder();
	_class_decoder = ClassDecoder();
	_pattern_model = PatternModel<uint32_t>();
}

void ColibriPLM::fit(std::vector<boost::filesystem::path> input_files) {

	std::cout << "Found " << input_files.size() << " files" << std::endl;

	for(auto i : input_files)
	{
		_class_encoder.build(i.string());
	}

	_class_encoder.save("/tmp/tmpout/somefilename.colibri.cls");

	std::string dat_output_file = "/tmp/tmpout/somefilename.colibri.dat";

	for(auto f : input_files)
	{
		_class_encoder.encodefile(f.string(), dat_output_file, false, false, true);
	}

	_class_decoder.load("/tmp/tmpout/somefilename.colibri.cls");

	// See https://github.com/proycon/colibri-core/blob/master/src/patternmodeller.cpp#L212
	PatternModelOptions options = PatternModelOptions();
	options.MAXLENGTH = 1;
	options.DOSKIPGRAMS = false;
	options.DOREVERSEINDEX = false;
	options.QUIET = true;

	_pattern_model.train(dat_output_file, options, nullptr);
	_corpus_frequency = _pattern_model.tokens();

}

double ColibriPLM::background_prob(Pattern pattern)
{
	return _pattern_model.occurrencecount(pattern) / _corpus_frequency;
}

double ColibriPLM::weighted_background_logprob(Pattern pattern)
{
	return log(background_prob(pattern) * (1-_interpolation_factor));
}

void ColibriPLM::create_lm(std::vector<boost::filesystem::path> input_files)
{
	std::cout << "Found " << input_files.size() << " files" << std::endl;

	for(auto i : input_files)
	{
		_class_encoder.build(i.string());

//		_class_encoder.
	}
}

//void EM(Pattern pattern, Document document, int iterations, double epsilon)
//{
//	double pattern_freq = log(4.0);
//	double e_step = pattern_freq +
//	double m_step =
//}

ColibriPLM::~ColibriPLM() {
	// TODO Auto-generated destructor stub
}
