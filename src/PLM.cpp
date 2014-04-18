/*
 * PLM.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: louis
 */

#include "PLM.hpp"

#include <iostream>
#include <algorithm>



PLM::PLM(double interpolation_factor) : _interpolation_factor(interpolation_factor), _corpus_frequency(0) {

}

ColibriPLM::ColibriPLM(double interpolation_factor) : PLM(interpolation_factor) {
	_class_encoder = ClassEncoder();
	_class_decoder = ClassDecoder();
	_pattern_model = PatternModel<uint32_t>();

	// See https://github.com/proycon/colibri-core/blob/master/src/patternmodeller.cpp#L212
	_pattern_model_options = PatternModelOptions();
	_pattern_model_options.MAXLENGTH = 1;
	_pattern_model_options.DOSKIPGRAMS = false;
	_pattern_model_options.DOREVERSEINDEX = false;
	_pattern_model_options.QUIET = false;

}

void ColibriPLM::create_background_model(std::vector<boost::filesystem::path> input_files) {

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

	_pattern_model.train(dat_output_file, _pattern_model_options, nullptr);
	_corpus_frequency = _pattern_model.tokens();

}

void ColibriPLM::create_document_model(boost::filesystem::path input_file)
{
//	std::cout << "Creating document model for " << input_file.string() << " ";

	std::ifstream input_stream;
	input_stream.open(input_file.string().c_str());

	std::stringstream through_stream;

	_class_encoder.encodefile((std::istream*) &input_stream, (std::ostream*) &through_stream, false, false, true);

	std::cout << ">>" << through_stream.str() << "<<" << std::endl;

	input_stream.close();

//	PatternModel<uint32_t> document_model = PatternModel<uint32_t>();
//
//	document_model.train((std::istream*) &through_stream, _pattern_model_options, nullptr);
//
//	std::cout << document_model.size() << std::endl;

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
