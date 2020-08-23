/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include "Bela.h"
#include <cmath>


class LPFilter {

public:
	
	LPFilter();
	
	void setSampleRate(int rate);
	void setFrequency(float frequency);
	void setQ(float q);

	~LPFilter();
	
	float b0;
	float b1;
	float b2;
	
	float a0;
	float a1;
	float a2;

private:

	void calculateCoefficients(float frequency, float q);


	float sampleRate_;
	float frequency_;
	float q_;
	
};