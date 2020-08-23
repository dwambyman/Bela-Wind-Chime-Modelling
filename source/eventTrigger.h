/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include "Bela.h"
#include <cmath>
#include <random>

typedef std::mt19937 rng_t_;

class EventTrigger {
	
public:
	
	EventTrigger();
	
	void setSampleRate(int sampleRate);
	uint32_t randomInt(rng_t_& rng_0, uint16_t range);
	float randomFloat(rng_t_& rng_0);
	
	int chimeTrigger(float windStrength);
	
	~EventTrigger();
	
	float sigmoidOutput;
	float comparativeProbability;
	
private:
	
	rng_t_ rng_;

	int sampleRate_;

};