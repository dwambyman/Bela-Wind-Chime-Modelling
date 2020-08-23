
/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/

#include <Bela.h>
#include <cmath>
#include <cstring>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>

#include "filter.h"
#include "util.h"
#include "chime.h"
#include "procedural.h"
#include "eventTrigger.h"

#define BLOCK_SIZE 16
#define NUMBER_OF_MODES 9
#define NUMBER_OF_FILTERS 4
#define NUMBER_OF_CHIMES 4

// GUI to adjust wind strength
Gui gui;
GuiController controller;
int sliderPosition = 1;
int sliderPositionMapped = 1;

// ARM-NEON IIR
extern "C" {
	float NEON_IIRFilter(float in);
}

// IIR filter variables
LPFilter LPFilter[NUMBER_OF_FILTERS];
float* b0_arr;
float* b1_arr;
float* b2_arr;
float* a1_arr;
float* a2_arr;
float* previousXn_1;
float* previousXn_2;
float* previousYn_1;
float* previousYn_2;
float filterAmplitude[NUMBER_OF_FILTERS];

// Chime variables
void chimeBackground(void *);
Chime chime[NUMBER_OF_CHIMES];
const int buttonPin = 0;

// Procedural wind generation
int windStrength = 0;
DrunkWalk drunkWalk;
int frequencySeed[NUMBER_OF_FILTERS];
Procedural windSource[NUMBER_OF_FILTERS];
float windFreq[NUMBER_OF_FILTERS];
float windQ[NUMBER_OF_FILTERS];
Ramp windFreqRamp[NUMBER_OF_FILTERS];
Ramp windQRamp[NUMBER_OF_FILTERS];
int windFreqAdd[NUMBER_OF_FILTERS];
float windQAdd[NUMBER_OF_FILTERS];
float rampingWindFreq[NUMBER_OF_FILTERS];
float rampingWindQ[NUMBER_OF_FILTERS];

// Probabilities
EventTrigger eventTrigger;
int randomiser1 = 0;
int randomiser2 = 0;
int randomiser3 = 0;

// Counters
int testCounter = 0;
int printCounter = 0;
int drunkWalkCounter = 0;
int chimeStrikeCounter = 0;
int windCounter = 0;

// General
int sampleRate;
StateMachine stateMachine;

//==============================================================================

/*
  * @brief Prepare variables that are to be used in audio processing
  * @param Pointer to the BelaContext structure
  * @param Opque pointer to the Bela backend when initialising audio
*/
bool setup(BelaContext *context, void *userData)
{
	sampleRate = context->audioSampleRate;

	// Pass project name into the setup of the GUI object 'gui'
	gui.setup(context->projectName);
	controller.setup(&gui, "Wind Controller");
	controller.addSlider("Wind Strength", 50, 1, 99, 1);
	
	// Set up the button, which toggles control mode
	pinMode(context, 0, buttonPin, INPUT);
	
	// Initialise the 4 chimes (see chime.h and chime.cpp)
	for (int i = 0; i < NUMBER_OF_CHIMES; i++)
	{
		rt_printf("\n");
		rt_printf("Loading chime %i...\n", i);
		chime[i].chimeInit(i, sampleRate);
	}
	
	// Set the sample rate for the eventTrigger object, whose methods trigger wind chimes based on probability from the wind strength
	eventTrigger.setSampleRate(sampleRate);
	
	// For each array, allocate 4 * 4 bytes of storage space, ensuring it is along 16 byte boundaries for use in q registers in the assembly code in NEONFilter.S
	// Each array will contain 4 coefficient values, one for each filter being processing in parallel in NEONFilter.S
	b0_arr = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	b1_arr = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	b2_arr = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	a1_arr = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	a2_arr = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	previousXn_1 = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	previousXn_2 = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	previousYn_1 = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	previousYn_2 = (float*)aligned_alloc(16, NUMBER_OF_FILTERS * sizeof(float));
	
	frequencySeed[0] = 500;
	frequencySeed[1] = 700;
	frequencySeed[2] = 1200;
	frequencySeed[3] = 1800;
	
	windFreqAdd[0] = 250;
	windFreqAdd[1] = 500;
	windFreqAdd[2] = 800;
	windFreqAdd[3] = 1200;
	
	windQAdd[0] = 4;
	windQAdd[1] = 6;
	windQAdd[2] = 9;
	windQAdd[3] = 18;
	
	// Set the sample rate of each filter object for use in coefficient calculation, and initial values for the ramping functions
	for (int i = 0; i < NUMBER_OF_FILTERS; i++)
	{
		LPFilter[i].setSampleRate(context->audioSampleRate);
		rampingWindFreq[i] = windFreqAdd[i];
		rampingWindQ[i] = windQAdd[i];
	}
	
	rt_printf("Ready to play...\n");
	
	return true;
}

/*
  * @brief Audio processing
  * @param Pointer to the BelaContext structure
  * @param Opque pointer to the Bela backend when initialising audio
*/
void render(BelaContext *context, void *userData)
{
    for(unsigned int n = 0; n < context->audioFrames; n++)
    {
    	// State machine toggles control mode from manual to automatic (see util.h)
		stateMachine.states(digitalRead(context, n, buttonPin));

		// Every 1/10th of a second check to see if control mode has changed, and update amplitudes of individual filters and chime partials
		if (drunkWalkCounter++ >= sampleRate*0.1)
		{
			// Update the current control mode to be that of the previous
			stateMachine.controlMode = stateMachine.previousMode;
			
			// Print console message once
			if (stateMachine.controlMode == stateMachine.manual && stateMachine.changeTheMode == true)
			{
				rt_printf("Manual...drag the slider to control the wind! \n");
				stateMachine.changeTheMode = false;
			}
			else if (stateMachine.controlMode == stateMachine.automatic  && stateMachine.changeTheMode == true)
			{
				rt_printf("Automatic...you have no control! \n");
				stateMachine.changeTheMode = false;
			}
			
			// If the user has selected manual mode, the wind strength in controlled manually by the slider in the GUI
			if (stateMachine.controlMode == stateMachine.manual)
			{
				sliderPosition = controller.getSliderValue(0);
				windStrength = sliderPosition;
			}
			// If the user has selected automatic mode, the wind strength is controlled by the drunkWalk object, the value of which
			// randomly increments up or down by 1 every time its stagger method is called
			else if (stateMachine.controlMode == stateMachine.automatic)
			{
				if (stateMachine.justSwitchedToAuto == true)
				{
					drunkWalk.staggerPosition = windStrength;
					stateMachine.justSwitchedToAuto = false;
				}
				else
				{
					windStrength = drunkWalk.stagger();
				}
				rt_printf("Wind Strength: %i\n", windStrength);
			}
			
			// Each filter in NEONFilter.S (i.e. each s register in a q register) is responsible for its own frequency band of the wind generation, so
			// as the wind strength increases, the filters whose cutoff frequencies ar higher are incrementally brought in, and all of their amplitudes increase
			for (int i = 0; i < NUMBER_OF_FILTERS; i++)
			{
				filterAmplitude[i] = windStrength - (i * 20);
				if (filterAmplitude[i] < 0)
				{
					filterAmplitude[i] = 0;
				}
				else if (filterAmplitude[i] > 40)
				{
					filterAmplitude[i] = 40;
				}
				// Ensure that the lowpass filters whose cutoff frequencies are lower are scaled appropriately so they are audible
				filterAmplitude[i] = (filterAmplitude[i] / 40) * ((5 - i)/2);
				if (i == 0)
				{
					filterAmplitude[i] = filterAmplitude[i] + 1;
				}
			}
			
			// As wind strength increases, to simulate the wind chimes increased brightness after being hit harder by the clapper, upper partials
			// are amplified exponentially in relation to the lower ones
			for (int i = 0; i < NUMBER_OF_CHIMES; i++)
			{
				for (int j = 0; j < NUMBER_OF_MODES; j++)
				{
					chime[i].modeExcitation[j] = exp((j*windStrength/99)*0.2);
				}
			}
			drunkWalkCounter = 0;
		}

		// The chime is triggered in intervals dicated by the layered stochasticity of the randomiser1 and randomiser2 objects; randomiser3 is
		// determined by the reciprocal of the wind strength, meaning a higher wind strength results in shorter intervals 
		if (chimeStrikeCounter++ >= randomiser1 + randomiser2 + randomiser3)
		{
			randomiser1 = sampleRate * 0.2;
			randomiser2 = randInt(rng, (sampleRate*0.3));
			randomiser3 = static_cast<int>(sampleRate * (1/windStrength));

			// the chimeTrigger method returns a chime number to be struck, the likelihood of which dependent on the altered sigmoid function
			// in eventTrigger.cpp
			chime[eventTrigger.chimeTrigger(windStrength)].trigger();
			chimeStrikeCounter = 0;
		}
		
		// Once a second, the random number generators for the 4 filters are seeded with different cutoff frequency and Q values, see procedural.h and
		// procedural.cpp
		if (windCounter++ >= sampleRate)
		{
			for (int i = 0; i < NUMBER_OF_FILTERS; i++)
			{
				windSource[i].freqSeed = randInt(rng, frequencySeed[i]);    // Seed random cutoff frequency generator
				windSource[i].qSeed = randFloat(rng);    // Seed random Q generator	
				windFreq[i] = windSource[i].calculateFrequency(windFreqAdd[i]);    // Return new stochastic cutoff frequency
				windQ[i] = windSource[i].calculateQ(2, windQAdd[i]);    // Return new stochastic Q value
			}
			windCounter = 0;
		}
		
		// Pass destination values to ramp functions, set the cutoff frequency and Q to whatever intermediate value the ramp function is returning, to
		// ensure smoother transitions and update coefficient values
		for (int i = 0; i < NUMBER_OF_FILTERS; i++)
		{
			// The ramp functions will increment in steps of 0.05 and 0.0001 respetively from their current value to the the target (windFreq or windQ), see
			// util.h
			rampingWindFreq[i] = windFreqRamp[i].nextRampValue(rampingWindFreq[i], windFreq[i], 0.05);
			rampingWindQ[i] = windQRamp[i].nextRampValue(rampingWindQ[i], windQ[i], 0.0001);

			// Irrespective of whether the ramp functions have reached their targets, the values they return are used as the actual cutoff frequency and Q of
			// the filters
			LPFilter[i].setFrequency(rampingWindFreq[i]);
			LPFilter[i].setQ(rampingWindQ[i]);
		
			// The coefficient values calculated in filter.cpp are copied into arrays for use in NEONFilter.S, after their amplitude is scaled by the
			// windStrength variable
			b0_arr[i] = LPFilter[i].b0 * filterAmplitude[i];
			b1_arr[i] = LPFilter[i].b1 * filterAmplitude[i];
			b2_arr[i] = LPFilter[i].b2 * filterAmplitude[i];
			a1_arr[i] = LPFilter[i].a1 * filterAmplitude[i];
			a2_arr[i] = LPFilter[i].a2 * filterAmplitude[i];
		}
		
		// Pass a random number between -1 and 1 into the assembly filter bank, which returns the sum of 4 low-pass filters, computed in parallel. See
		// NEONFilter.S
		float in = whiteNoise(rng);
		float out = NEON_IIRFilter(in);

		// Update state variables
		for (int i = 0; i < NUMBER_OF_FILTERS; i++)
		{
	   		previousXn_2[i] = previousXn_1[i];    // x[n-2] = x[n-1]
    		previousXn_1[i] = in * 0.1;    // x[n-1] = x[n]

    		previousYn_2[i] = previousYn_1[i];    // y[n-2] = y[n-1]
    		previousYn_1[i] = out * 0.1;    // y[n-1] = y[n]
		
			// Sum each chime, scaled by between 0.25-0.35, dependent on the value of windStrength
			out += chime[i].nextValue() * (0.25 + ((windStrength*0.1)/99));
		}
		
    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
    		audioWrite(context, n, channel, out);
    	}
    }
}


/*
  * @brief Free memory allocated in the aligned_alloc functions in setup()
  * @param Pointer to the BelaContext structure
  * @param Opque pointer to the Bela backend when initialising audio
*/
void cleanup(BelaContext *context, void *userData)
{
	free(b0_arr);
	free(b1_arr);
	free(b2_arr);
	free(a1_arr);
	free(a2_arr);
	free(previousXn_1);
	free(previousXn_2);
	free(previousYn_1);
	free(previousYn_2);
}
