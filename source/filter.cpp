/*
  ==============================================================================

        MAP FINAL PROJECT
        v1.0
        Jack Walters

  ==============================================================================
*/
#include <Bela.h>
#include "filter.h"

/*
  * @brief Constructor
*/
LPFilter::LPFilter()
{
	// Initialise frequency and Q values
	frequency_ = 100.0;
	q_ = 0.707;

	// When this constructor is called, call private calculateCoefficients method with initialised frequency and Q values
	calculateCoefficients(frequency_, q_);
}

/*
  * @brief This method is called in setup() to set the sample rate
  * @param Sample rate
*/
void LPFilter::setSampleRate(int rate)
{
	sampleRate_ = rate;	
	
	// When this method setSampleRate() is called, call private calculateCoefficients method with initialised frequency and Q values
	calculateCoefficients(frequency_, q_);
}

/*
  * @brief Set the frequency and recalculate coefficients
  * @param Cutoff frequency
*/
void LPFilter::setFrequency(float frequency)
{
	frequency_ = frequency;
	calculateCoefficients(frequency_, q_);
}
	
/*
  * @brief Set the Q and recalculate the coefficients
  * @param Q factor
*/
void LPFilter::setQ(float q)
{
	q_ = q;
	calculateCoefficients(frequency_, q_);
}
	
/*
  * @brief Calculate coefficients
  * @param Cutoff frequency
  * @param Q factor
*/
void LPFilter::calculateCoefficients(float frequency, float Q)
{
	// Helper variables
	float Omega0 = frequency_ * 2.0 * M_PI;
	float T = 1.0 / sampleRate_;
	
	// Coefficient calculation
	b0 = (pow(Omega0,2)*Q*pow(T,2)) / (4*Q + 2*Omega0*T + pow(Omega0,2)*Q*pow(T,2));
	b1 = (2 * pow(Omega0,2)*Q*pow(T,2)) / (4*Q + 2*Omega0*T + pow(Omega0,2)*Q*pow(T,2));
	b2 =(pow(Omega0,2)*Q*pow(T,2)) / (4*Q + 2*Omega0*T + pow(Omega0,2)*Q*pow(T,2));
	
	a0 = (4*Q + 2*Omega0*T + pow(Omega0,2)*Q*pow(T,2)) / (4*Q + 2*Omega0*T + pow(Omega0,2)*Q*pow(T,2));
	a1 = (-8*Q + 2*pow(Omega0,2)*pow(T,2)*Q) / (4*Q + 2*Omega0*T + pow(Omega0,2)*Q*pow(T,2));
	a2 = (4*Q - 2*Omega0*T + pow(Omega0,2)*Q*pow(T,2)) / (4*Q + 2*Omega0*T + pow(Omega0,2)*Q*pow(T,2));
}

/*
  * @brief Destructor
*/
LPFilter::~LPFilter()
{
	
}