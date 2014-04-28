/*
 * PLM.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: louis
 */

#include "PLM.hpp"

#include <iostream>
#include <algorithm>

#include <memory>

#include "pattern.h"
#include "utils.hpp"

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
	_pattern_model_options.QUIET = true;
	_pattern_model_options.MINTOKENS = 1;

}

void ColibriPLM::create_background_model(std::vector<boost::filesystem::path> input_files) {

	std::cout << "Found " << input_files.size() << " files" << std::endl;

	std::vector<std::string> input_file_names = std::accumulate(input_files.begin(), input_files.end(),
	 std::vector<std::string>(),
	 [](std::vector<std::string>& v, boost::filesystem::path path)
	   {
	    v.push_back(path.string());
	    return v;
	   });

	_class_encoder.build(input_file_names, true);

	_class_encoder.save("/tmp/tmpout/somefilename.colibri.cls");

	std::string dat_output_file = "/tmp/tmpout/somefilename.colibri.dat";

	for(auto i : input_file_names)
	{
		_class_encoder.encodefile(i, dat_output_file, false, false, true, true);
	}

	_class_decoder.load("/tmp/tmpout/somefilename.colibri.cls");

	_pattern_model.train(dat_output_file, _pattern_model_options, nullptr);
	_corpus_frequency = _pattern_model.tokens();

}

std::vector<std::pair<Pattern, double>> ColibriPLM::create_document_model(boost::filesystem::path input_file)
{
	std::ifstream input_stream;
	input_stream.open(input_file.string().c_str());

	std::stringstream through_stream;

	_class_encoder.encodefile((std::istream*) &input_stream, (std::ostream*) &through_stream, true, false, true);

	input_stream.close();

	PatternModel<uint32_t> document_model = PatternModel<uint32_t>();

	document_model.train((std::istream*) &through_stream, _pattern_model_options, nullptr);


//	std::vector<double> word_probs = std::accumulate(document_model.begin(), document_model.end(),
//			std::vector<double>(), [=](std::vector<double>& dd, PatternModel<uint32_t>::iterator p) {dd.push_back(document_model.occurrencecount(p->first)); return dd;} );

	std::vector<std::pair<Pattern, double>> word_probs = std::vector<std::pair<Pattern, double>>();
	for(auto &p : document_model)
	{
		word_probs.push_back(std::pair<Pattern, double>(p.first, 1.0*document_model.occurrencecount(p.first)/document_model.tokens()));
	}

	// em
	std::vector<std::pair<Pattern, double>> temp = std::vector<std::pair<Pattern, double>>();

//	double epsilon = 0.05;
	int iterations = 100;

	for (int iteration : range(iterations))
	{
		double e_tot = 0.0;

		temp.clear();

		// e
		for (int i : range(word_probs.size()))
		{

//			double e_i = log(document_model.occurrencecount(p)) + log(_interpolation_factor) + log(word_prob) - log(background_prob(p) * (1-_interpolation_factor) + _interpolation_factor*word_prob);

			Pattern p = word_probs[i].first;
			double word_prob = word_probs[i].second;

			double e_i = document_model.occurrencecount(p) * _interpolation_factor*word_prob / ((1-_interpolation_factor)*background_prob(p) + _interpolation_factor*word_prob);
			e_tot += e_i;
			temp.push_back(std::make_pair(p, e_i));
		}

		word_probs.clear();

//		// m
		for (int i : range(temp.size()))
		{
			word_probs.push_back(std::make_pair(temp[i].first, 1.0*temp[i].second/e_tot));

		}

	}


	for(auto a : word_probs) std::cout << a.first.tostring(_class_decoder) << ":" << a.second << std::endl;

	return word_probs;
}

double ColibriPLM::background_prob(Pattern pattern)
{
	return _pattern_model.occurrencecount(pattern) / _corpus_frequency;
}

double ColibriPLM::weighted_background_logprob(Pattern pattern)
{
	return log(background_prob(pattern) * (1-_interpolation_factor));
}

ColibriPLM::~ColibriPLM() {
	// TODO Auto-generated destructor stub
}
