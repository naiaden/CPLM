/*
 * PLM.hpp
 *
 *  Created on: Apr 15, 2014
 *      Author: louis
 */

#ifndef PLM_HPP_
#define PLM_HPP_

#include <vector>

#include <boost/filesystem.hpp>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

class PLM {
public:

	virtual void fit(std::vector<boost::filesystem::path> input_files) = 0;
	virtual double background_prob(Pattern pattern) = 0;
	virtual double weighted_background_logprob(Pattern pattern) = 0;

protected:
	PLM(double interpolation_factor);
		virtual ~PLM() {};


	double _interpolation_factor;
	double _corpus_frequency;

};

class ColibriPLM: public PLM {
public:
	ColibriPLM(double interpolation_factor);
	virtual ~ColibriPLM();

	void fit(std::vector<boost::filesystem::path> input_files) override;
	double background_prob(Pattern pattern) override;
	double weighted_background_logprob(Pattern pattern) override;

private:
	ClassEncoder _class_encoder;
	ClassDecoder _class_decoder;
	PatternModel<uint32_t> _pattern_model;
};

#endif /* PLM_HPP_ */
