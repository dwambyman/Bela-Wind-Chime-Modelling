/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include "Bela.h"
#include <cstdlib>


class Procedural {
	
public:
	
	Procedural();
	
	float calculateFrequency(float freqAdd);
	float calculateQ(float qScale, float qAdd);
	
	~Procedural();
	
	float freqSeed;
	float qSeed;
	
private:

};