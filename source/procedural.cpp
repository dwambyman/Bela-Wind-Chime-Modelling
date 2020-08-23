/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include <Bela.h>
#include "procedural.h"

/*
  * @brief Constructor
*/
Procedural::Procedural()
{
	freqSeed = 0;
	qSeed = 0;
}

/*
  * @brief Calculate random frequency
  * @param Amount to add to randomly generated frequency
*/
float Procedural::calculateFrequency(float freqAdd)
{
	return freqSeed + freqAdd;
}

/*
  * @brief Calculate random Q
  * @param Amount to add to randomly generated Q
*/
float Procedural::calculateQ(float qScale, float qAdd)
{
	return  (qSeed * qScale) + qAdd;
}

/*
  * @brief Destructor
*/
Procedural::~Procedural()
{
	
}