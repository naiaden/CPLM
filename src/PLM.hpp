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

class PLM {
public:

	virtual void fit(std::vector<boost::filesystem::path> input_files) = 0;

protected:
	PLM(double interpolation_factor);
		virtual ~PLM() {};


	double _interpolation_factor;

};

class ColibriPLM: public PLM {
public:
	ColibriPLM(double interpolation_factor);
	virtual ~ColibriPLM();

	void fit(std::vector<boost::filesystem::path> input_files) override;
};

#endif /* PLM_HPP_ */
