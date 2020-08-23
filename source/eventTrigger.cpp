/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include <Bela.h>
#include "eventTrigger.h"

/*
  * @brief Constructor
*/
EventTrigger::EventTrigger()
{
	
}

/*
  * @brief Set the sample rate of objects instantiated with this class type
  * @param Sample rate
*/
void EventTrigger::setSampleRate(int sampleRate)
{
	sampleRate_ = sampleRate;
}

/*
  * @brief Return a random integer within a given range
  * @param Random number generator, in this case is mt19937
  * @param Generated random number range
*/
uint32_t EventTrigger::randomInt(rng_t_& rng_, uint16_t range)
{
    uint16_t x = rng_();
    uint32_t m = uint16_t(x) * uint16_t(range);
    return m >> 16;	
}

/*
  * @brief Return a random float between 0 and 1
  * @param Random number generator, in this case is mt19937
*/
float EventTrigger::randomFloat(rng_t_& rng_)
{
    uint16_t x = rng_();
    uint32_t m = uint16_t(x) * 100;
    float f = (static_cast<float>(m >> 16) * 0.01);
    return f;	
}

/*
  * @brief Compare the output of the altered sigmoid function with the a random number; if the sigmoid output is lower, trigger a random chime
  * @param Wind strength
*/
int EventTrigger::chimeTrigger(float windStrength)
{
	// Map wind strength to be between 0 and 4.59, which is the upper limit of the input of hte sigmoid function, before float precision becomes
	// inadequate and is rounded off to 1.0
	float windStrengthMapped = (0.01 + ((4.59 - 0.01) / (100 - 0)) * (windStrength - 0));
	
	// Sigmoid function based off Lukkari's and Välimäki's "Modal Synthesis of Wind Chime Sounds with Stochastic Event Triggering"
	sigmoidOutput = 1/(1 + (99 * exp(-2 * windStrengthMapped)));
	
	// Random float betwen 0 and 1 generated for point of comparison with sigmoid output
	comparativeProbability = randomFloat(rng_);

	if (comparativeProbability <= sigmoidOutput)
	{
		return randomInt(rng_, 3);
	}
	else
	{
		return 10;
	}
}

/*
  * @brief Destructor
*/
EventTrigger::~EventTrigger()
{
	
}
