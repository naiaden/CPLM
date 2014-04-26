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
	_pattern_model_options.QUIET = false;
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

	_class_encoder.build(input_file_names);

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

//	std::vector<std::pair<Pattern, double>> word_probs = std::vector<std::pair<Pattern, double>>();
	std::shared_ptr<std::vector<std::pair<Pattern, double>>> pWord_probs(new std::vector<std::pair<Pattern, double>>());
	for(auto &p : document_model)
	{
		pWord_probs->push_back(std::pair<Pattern, double>(p.first, 1.0*document_model.occurrencecount(p.first)/document_model.tokens()));
	}

	// em
//	std::vector<std::pair<Pattern, double>> temp = std::vector<std::pair<Pattern, double>>();

	std::shared_ptr<std::vector<std::pair<Pattern, double>>> pTemp(new std::vector<std::pair<Pattern, double>>());

	for (int iterations : range(50))
	{
		double e_tot = 0.0;

		pTemp->clear();

		std::cerr << "ts:" << pTemp->size() << " wps:" << pWord_probs->size() << "\t";

//		for (int bla : range(pWord_probs->size()))
//				    std::cerr << "[" << (*pWord_probs)[bla].first.data << "|" << (*pWord_probs)[bla].second << ']';

		// e
		for (int i : range(pWord_probs->size()))
		{

			Pattern p = (*pWord_probs)[i].first;
			double word_prob = (*pWord_probs)[i].second;
//			std::cerr << "[" << word_prob << "|";



			double t1 = document_model.occurrencecount(p);
			double t2 = _interpolation_factor*word_prob;
			double t3 = background_prob(p + _interpolation_factor*word_prob);


			double t4 = t1 * t2;
			double t5 = (1-_interpolation_factor) * t3;

//			std::cerr << "1:" << t1 << ":2:" << t2 << ":3:" << t3 << ":4:" << t4 << ":5:" << t5 << "]";

//			std::cerr << "(" << t4 << "/" << t5 << "=" << t4/t5 << ")";

			double e_i = t4/t5;

//			double e_i = document_model.occurrencecount(p) * _interpolation_factor*word_prob / ((1-_interpolation_factor)*background_prob(p + _interpolation_factor*word_prob));
			e_tot += e_i;
//			std::cerr << "c";
//			std::pair<Pattern, double> zomaar = std::pair<Pattern, double>(p, 0.05);
			pTemp->push_back(std::make_pair(p, e_i));
//			pTemp->push_back(std::pair<Pattern, double>(p, e_i));
//			std::cerr << "d";
		}



		std::cerr << "e";

		pWord_probs->clear();

//		std::

		e_tot = 4.0;

//		// m
		for (int i : range(pTemp->size()))
		{
//			(*pTemp)[i] = std::pair<Pattern, double>((*pTemp)[i].first, 1.0*(*pTemp)[i].second/e_tot);
			std::cerr << "[" << (*pTemp)[i].second << "," << e_tot << "]";
			pWord_probs->push_back(std::make_pair((*pTemp)[i].first, 1.0*(*pTemp)[i].second/e_tot));

		}

		std::cerr << "\tts:" << pTemp->size() << " wps:" << pWord_probs->size() << std::endl;

//		pWord_probs.swap(pTemp);
	}

	return *pWord_probs;
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
