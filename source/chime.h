/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include "Bela.h"
#include <cmath>
#include <math_neon.h>

#define NUMBER_OF_MODES 9

class Chime {
	
public:
	
	Chime();
	
	enum envelopeState {
		envelopeStateOff = 0,
		envelopeStateAttack,
		envelopeStateDecay,
		numEnvelopeStates
	};
	
	void chimeInit(int chimeNum, int Samplerate);
	void trigger();
	float nextValue();
	
	float* wavetable[NUMBER_OF_MODES];
	float modeExcitation[NUMBER_OF_MODES];
	
	~Chime();
	
private:

	int sampleRate_;
	float modes_[NUMBER_OF_MODES];
	float gPhase_[NUMBER_OF_MODES];
	
	envelopeState currentState_[NUMBER_OF_MODES];
	float currentLevel_[NUMBER_OF_MODES];
	unsigned int currentSampleIndex_[NUMBER_OF_MODES];
	unsigned int nextStateSampleIndex_[NUMBER_OF_MODES];
	float minimumLevel_;
	float maximumLevel_[NUMBER_OF_MODES];
	float stateValue_[numEnvelopeStates][NUMBER_OF_MODES];
	float chimeSum_;
	int readPointer_;
	int totalLength;
	bool isPlaying_;
};