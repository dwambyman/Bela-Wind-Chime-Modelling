/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include "Bela.h"
#include <random>
#include <cstdlib>

typedef std::mt19937 rng_t;
rng_t rng;

/*
  * @brief Calculate a random float between -1 and 1
  * @param Random number generator
*/
float whiteNoise(rng_t& rng)
{
    uint16_t x = rng();
    uint32_t m = uint16_t(x) * 200;
    float f = (static_cast<float>(m >> 20) * 0.1) - 1;
    return f;	
}

/*
  * @brief Calculate random float between 0-1
  * @param Random number generator
*/
float randFloat(rng_t& rng)
{
    uint16_t x = rng();
    uint32_t m = uint16_t(x) * 100;
    float f = (static_cast<float>(m >> 16) * 0.01);
    return f;	
}

/*
  * @brief Calculate random integer
  * @param Random number generator
  * @param Range the random integer is to be in
*/
uint32_t randInt(rng_t& rng, uint16_t range)
{
    uint16_t x = rng();
    uint32_t m = uint16_t(x) * uint16_t(range);
    return m >> 16;	
}

/*
  * @brief State machine for the button, which sets control mode (manual or automatic)
*/
class StateMachine {
	
public:
	
	StateMachine()
	{
		changeTheMode = false;
		justSwitchedToAuto = false;
		debounceState_ = stateOn_;
		debounceCounter_ = 0;
		debounceInterval_ = 250;
		previousButtonState_ = 0;
		previousMode = 0;
		controlMode = 0;
	}
	
	void states(int buttonValue)
	{
		if(debounceState_ == stateOn_)
   		{
   			controlMode = automatic;
   			if (buttonValue == 0)
   			{
   				previousMode = !previousMode;
   				debounceState_ = stateJustOff_;
   				debounceCounter_ = 0;
   				previousMode = !previousMode;
   			}
   		}
   		else if(debounceState_ == stateJustOff_)
   		{
   			if (debounceCounter_++ >= debounceInterval_)
   			{
   				changeTheMode = true;
		   		debounceState_ = stateOff_;
		   	}
   		}
   		else if (debounceState_ == stateOff_)
   		{
   			controlMode = manual;
   			if (buttonValue != 0)
   			{
   				previousMode = !previousMode;
   				debounceState_ = stateJustOn_;
   				debounceCounter_ = 0;
   			}
   		}
   		else if(debounceState_ == stateJustOn_)
   		{
   			if (debounceCounter_++ >= debounceInterval_)
   			{
   				changeTheMode = true;
   				justSwitchedToAuto = true;
		   		debounceState_ = stateOn_;
		   	}
   		}
   		previousButtonState_ = buttonValue;
	}
	
	bool changeTheMode;
	bool justSwitchedToAuto;
	int controlMode;
	int previousMode;
	enum {
		manual = 0,
		automatic
	};
	
	~StateMachine() {}
	
private:

	int previousButtonState_;;
	enum {
		stateOn_ = 0,
		stateJustOff_,
		stateOff_,
		stateJustOn_
	};
	int debounceState_;
	int debounceCounter_;
	int	debounceInterval_;
	
};

/*
  * @brief Random walk incrementation generation
*/
class DrunkWalk {

public:

	DrunkWalk()
	{
		staggerPosition = 50;
	}
	
	int stagger()
		{
			float seed = randFloat(rng);
			if (seed >= 0.5)
			{
				if (staggerPosition++ > 100)
				{
					staggerPosition--;
				}
				return staggerPosition;
			}
			else
			{
				if (staggerPosition-- <= 0)
				{
					staggerPosition++;
				}
				return staggerPosition;
			}
		}
		
	~DrunkWalk() {}
	
	int staggerPosition;
	
};

/*
  * @brief Contains ramping fucntion which allows for gradual change between frequency and Q values
*/
class Ramp {

public:
    
    Ramp()
    {
        lower_ = 0;
        upper_ = 0;
        incrementing_ = true;
    }

    float nextRampValue(float start, float end, float inc)
    {
    	increment_ = inc;
    	
    	float value = start;
    
    	lower_ = std::min(start, end);
        upper_ = std::max(start, end);
        
        if (start >= upper_ || start <= lower_)
        {
            incrementing_ = !incrementing_;
        }
        if (start < end)
        {
        	incrementing_ = true;
        }
        else if (start > end)
        {
        	incrementing_ = false;
        }
        else if (start == end)
        {
        	increment_ = 0;
        }

        value += incrementing_ ? increment_ : -increment_;

        return value;
    }
    
    ~Ramp() {};

private:

    float lower_;
    float upper_;
    float increment_;
    bool incrementing_;
    
};
