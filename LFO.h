#ifndef LFO_H
#define LFO_H

#include "OscSwitch.h"

// wrapped class around OscSwith
class LFO
{
public:
    /// constructor that resets filter instance
    LFO(juce::NormalisableRange<float> frequencyRange)
        : minFrequency(frequencyRange.start), maxFrequency(frequencyRange.end)
    {
        frequencyMaxOffset = 0.5 * (maxFrequency - minFrequency); // max frequency offset for an external LFO
    }

    float process()
    {
        // LFO amount
        float am = amount + amountOffset;
        if (am > 1.0f)
            am = 1.0f;
        if (am < -1.0f)
            am = -1.0f;
        // LFO frequency
        float freq = frequency + frequencyOffset;
        if (freq > maxFrequency)
            freq = maxFrequency;
        if (freq < minFrequency)
            freq = minFrequency;
        lfo.setFrequency (freq);
        float lfoSample = am * lfo.process();
        phase = lfo.getPhase();
        resetModulations();
        return lfoSample;
    }

    /// set sample rate for LFO
    /// @param float, sample rate in Hz
    void setSampleRate (float _sampleRate)
    {
        lfo.setSampleRate (_sampleRate);
    }

    /// set LFO waveshape
    /// @param int, waveshape id (0 - sine, 1 - triangle, 2 - saw, 3 - square)
    void setLFOWaveshape (int _lfoWaveshapeId)
    {
        lfo.setWaveshape (_lfoWaveshapeId);
    }

    /// set LFO frequency
    /// @param float, frequency
    void setLFOFrequency (float _frequency)
    {
        frequency = _frequency;
        lfo.setFrequency (_frequency);
    }

    /// set LFO frequency offset (useful for frequency modulation)
    /// @param float, frequency offset amount (from -1 to 1)
    void setLFOFrequencyOffset (float _frequencyOffsetAmount)
    {
        frequencyOffset += _frequencyOffsetAmount * frequencyMaxOffset;
    }

    /// set LFO amplitude
    /// @param float, amplitude
    void setLFOAmplitude (float _amplitude)
    {
        lfo.setAmplitude (_amplitude);
    }

    void setLFOAmount (float _amount)
    {
        amount = _amount;
    }

    void setLFOAmountOffset (float _amountOffset)
    {
        amountOffset += _amountOffset;
    }

    /// start the attack phase of the envelope
    void startNote (Parameters* _param, int _idx, float _sampleRate)
    {
        (*this).setLFOWaveshape (*_param->lfoWaveshapeParam[_idx]);
        (*this).setSampleRate (_sampleRate);
        (*this).setLFOFrequency (*_param->lfoRateParam[_idx]);
        (*this).setLFOAmount (*_param->lfoAmountParam[_idx]);
        if (*_param->lfoRetrigger[_idx] == true)
        {
            phase = 0.0f;
            lfo.setPhase (0.0f);
        }
    }

    /// start the release phase of the envelope
    void stopNote()
    {}

    ///
    bool isAppliedToOpLevel (int lfoDestination, int numOperators)
    {
        if (lfoDestination < 2 * numOperators && lfoDestination % 2 == 0)
            return true;
        else
            return false;
    }

    bool isAppliedToOpPhase (int lfoDestination, int numOperators)
    {
        if (lfoDestination < 2 * numOperators && lfoDestination % 2 == 1)
            return true;
        else
            return false;
    }

    bool isAppliedToFilterFreq (int lfoDestination)
    {
        if (lfoDestination == 8)
            return true;
        else
            return false;
    }

    bool isAppliedToFilterRes (int lfoDestination)
    {
        if (lfoDestination == 9)
            return true;
        else
            return false;
    }

    bool isAppliedToLFORate (int lfoDestination, int numLFOs)
    {
        int idxStart = 10;
        if (lfoDestination >= idxStart && lfoDestination < idxStart + 2 * (numLFOs-1) && (lfoDestination - idxStart) % 2 == 0)
            return true;
        else
            return false;
    }

    bool isAppliedToLFOAmount (int lfoDestination, int numLFOs)
    {
        int idxStart = 10;
        if (lfoDestination >= idxStart && lfoDestination < idxStart + 2 * (numLFOs - 1) && (lfoDestination - idxStart) % 2 == 1)
            return true;
        else
            return false;
    }

private:
    // base members
    OscSwitch lfo;
    // LFO parameters
    float amount;
    float frequency;
    float phase = 0.0f; // is storesd so there is an option to not retrigger LFO with a new note
    // modulation parameters
    float amountOffset = 0.0f;
    float frequencyOffset = 0.0f;
    float frequencyMaxOffset;
    // bounds
    float minFrequency;
    float maxFrequency;

    /// reset external LFO modulations (frequency and amount modulations)
    void resetModulations()
    {
        frequencyOffset = 0.0f;
        amountOffset = 0.0f;
    }
};

#endif // LFO_H