/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include <Bela.h>
#include "chime.h"

/*
  * @brief Constructor
*/
Chime::Chime()
{
	minimumLevel_ = 0.0000001;
	sampleRate_ = 0;
	readPointer_ = 0;
	chimeSum_ = 0;
	totalLength = 0;
	isPlaying_ = false;

	for (int i = 0; i < NUMBER_OF_MODES; i++)
	{
		currentState_[i] = envelopeStateOff;
		currentLevel_[i] = minimumLevel_;
		currentSampleIndex_[i] = 0;
		nextStateSampleIndex_[i] = 0;
		stateValue_[envelopeStateOff][i] = 0;
		stateValue_[envelopeStateAttack][i] = 0.0001;
		gPhase_[i] = 0.0;
		modeExcitation[i] = 0;
	}
	
	// Mode T60 decay times, adapted from Lukkari's and Välimäki's "Modal Synthesis of Wind Chime Sounds with Stochastic Event Triggering"
	stateValue_[envelopeStateDecay][0] = 20.0;
	stateValue_[envelopeStateDecay][1] = 3.5;
	stateValue_[envelopeStateDecay][2] = 1.0;
	stateValue_[envelopeStateDecay][3] = 0.5;
	stateValue_[envelopeStateDecay][4] = 0.25;
	stateValue_[envelopeStateDecay][5] = 0.125;
	stateValue_[envelopeStateDecay][6] = 0.0625;
	stateValue_[envelopeStateDecay][7] = 0.03125;
	stateValue_[envelopeStateDecay][8] = 0.015625;
}

/*
  * @brief Initialise the chime modes, and store them in wavetables
  * @param Chime index number
  * @param Sample rate
*/
void Chime::chimeInit(int chimeNum, int sampleRate)
{
	// Set sample rate and the total length of each chime in seconds, which is the attack time plus the decay time of the lowest mode
	sampleRate_ = sampleRate;
	totalLength = static_cast<int>((stateValue_[envelopeStateAttack][0] + stateValue_[envelopeStateDecay][0]) * sampleRate_);
	
	// Readings taken Sonic Visualiser, please refer to accompanying material
	switch (chimeNum)
	{
	case 0:
		modes_[0] = 829.56;
		modes_[1] = 2224.63;
		modes_[2] = 4191.66;
		modes_[3] = 6586.79;
		modes_[4] = 9306.54;
		modes_[5] = 10353.6;
		modes_[6] = 10570.4;
		modes_[7] = 10892.7;
		modes_[8] = 11328.3;

		maximumLevel_[0] = 0.449;
		maximumLevel_[1] = 0.601;
		maximumLevel_[2] = 0.208;
		maximumLevel_[3] = 0.176;
		maximumLevel_[4] = 0.111;
		maximumLevel_[5] = 0.112;
		maximumLevel_[6] = 0.110;
		maximumLevel_[7] = 0.103;
		maximumLevel_[8] = 0.098;
		
		break;
		
	case 1:
		modes_[0] = 1118.3;
		modes_[1] = 2959.24;
		modes_[2] = 5521.3;
		modes_[3] = 8594.23;
		modes_[4] = 10371.8;
		modes_[5] = 10701.2;
		modes_[6] = 11168.1;
		modes_[7] = 11956.1;
		modes_[8] = 12775.9;
		
		maximumLevel_[0] = 0.647;
		maximumLevel_[1] = 0.45;
		maximumLevel_[2] = 0.154;
		maximumLevel_[3] = 0.141;
		maximumLevel_[4] = 0.129;
		maximumLevel_[5] = 0.134;
		maximumLevel_[6] = 0.121;
		maximumLevel_[7] = 0.117;
		maximumLevel_[8] = 0.099;
		
		break;
		
	case 2:
		modes_[0] = 1636.85;
		modes_[1] = 4268.98;
		modes_[2] = 10246.1;
		modes_[3] = 10392;
		modes_[4] = 10551.2;
		modes_[5] = 11814.2;
		modes_[6] = 13018.5;
		modes_[7] = 0;
		modes_[8] = 0;
		
		maximumLevel_[0] = 0.583;
		maximumLevel_[1] = 0.273;
		maximumLevel_[2] = 0.111;
		maximumLevel_[3] = 0.125;
		maximumLevel_[4] = 0.130;
		maximumLevel_[5] = 0.099;
		maximumLevel_[6] = 0.106;
		maximumLevel_[7] = 0;
		maximumLevel_[8] = 0;
		
		break;
		
	case 3:
		modes_[0] = 1333.49;
		modes_[1] = 3466.18;
		modes_[2] = 6483.16;
		modes_[3] = 9951.58;
		modes_[4] = 10421.5;
		modes_[5] = 10855.8;
		modes_[6] = 11448.7;
		modes_[7] = 13482.7;
		modes_[8] = 14975.5;

		maximumLevel_[0] = 0.628;
		maximumLevel_[1] = 0.338;
		maximumLevel_[2] = 0.155;
		maximumLevel_[3] = 0.146;
		maximumLevel_[4] = 0.155;
		maximumLevel_[5] = 0.121;
		maximumLevel_[6] = 0.132;
		maximumLevel_[7] = 0.093;
		maximumLevel_[8] = 0.064;
		
		break;
		
	}	
	
	for (int i = 0; i < NUMBER_OF_MODES; i++)
	{
		// Delete any pre-existing wavetables, if they exist
		if(wavetable[i] != 0)
		{
			delete[] wavetable[i];
		}
		
		// Allocate memory for new wavetables
		wavetable[i] = new float[totalLength];
		
		// Trigger the attack envelope of each mode, ensuring it is being indexed from the start
		currentState_[i] = envelopeStateAttack;
		currentSampleIndex_[i] = 0;
	}
	
	for (int i = 0; i < totalLength; i++)
	{
		for (int j = 0; j < NUMBER_OF_MODES; j++)
		{
			if (currentState_[j] == envelopeStateOff)	// Envelope is off
			{
				currentLevel_[j] = 0;
			}
			else if (currentState_[j] == envelopeStateAttack)   // Envelope is in its attack state
			{
				currentSampleIndex_[j]++;
				
				// Set the value at which the envelope will transition from attack to decay
				nextStateSampleIndex_[j] = stateValue_[envelopeStateAttack][j] * sampleRate_;
				
				// The current level will increase linearly by the increment calculated below
				float increment_ = (maximumLevel_[j] - currentLevel_[j]) / (stateValue_[envelopeStateAttack][j] * sampleRate_);
				currentLevel_[i] += increment_;
				
				// Check to see if current index has reached the index at which the envelope will transtion into its decay state
				if (currentSampleIndex_[j] >= nextStateSampleIndex_[j])
				{
					currentState_[j] = envelopeStateDecay;
					currentSampleIndex_[j] = 0;
				}
			}
			else if (currentState_[j] == envelopeStateDecay)	// Envelope is in its decay state
			{
				currentSampleIndex_[j]++;
				
				// Set the index at which the envelope will transition from decay to off
				nextStateSampleIndex_[j] = stateValue_[envelopeStateDecay][j] * sampleRate_;
				
				// The current level will decrease Logarithmically by the multiplier calculated below 
				float multiplier_ = 1.0 + (log(minimumLevel_) - log(currentLevel_[j])) / (nextStateSampleIndex_[j]);
				currentLevel_[j] *= multiplier_;
				
				// Check to see if current index has reached the index at which the envelope will transtion into its off state
				if (currentSampleIndex_[j] >= nextStateSampleIndex_[j])
				{
					currentState_[j] = envelopeStateOff;
					currentSampleIndex_[j] = 0;
				}
			}
		
			// Wrap the phase of the math-neon sin function oscillating at frequency modes_[j], if its value is equal to or greater than 2π
			gPhase_[j] += 2.0 * M_PI * modes_[j] / sampleRate_;
			if(gPhase_[j] >= 2.0 * M_PI)
			{
				gPhase_[j] -= 2.0 * M_PI;
			}
			
		// Sample index i of the wavetable of mode j is set to the current envelope level multiplied by the sin function
		wavetable[j][i] = (currentLevel_[j]*0.1) * sinf_neon(gPhase_[j]);
		
		}
	}
}

/*
  * @brief Triggers the playback of mode wavetables
*/
void Chime::trigger()
{
	isPlaying_ = true;
	readPointer_ = 0;
	chimeSum_ = 0;
}

/*
  * @brief Returns the next sample of the summation of the mode wavetables for a given chime
*/
float Chime::nextValue()
{
	if (isPlaying_ == true)
	{
		// If the read pointer has reached the end of the chime length, stop playback
		if (readPointer_++ >= totalLength)
		{
			readPointer_ = 0;
			isPlaying_ = false;
		}
		
		// Sum the mode wavetables, scaled indirectly by the wind strength
		for (int i = 0; i < NUMBER_OF_MODES; i++)
		{
			chimeSum_ += (wavetable[i][readPointer_] * modeExcitation[i]);
		}
	}
	else
	{
		readPointer_ = 0;
		isPlaying_ = false;
	}
	return chimeSum_;
}

/*
  * @brief Destructor
*/
Chime::~Chime()
{
	for (int i = 0; NUMBER_OF_MODES; i++)
	{
		// Free memory allocated to the wavetables in chimeInit()
		if(wavetable[i] != 0)
		{
			delete[] wavetable[i];
		}
	}	
}
