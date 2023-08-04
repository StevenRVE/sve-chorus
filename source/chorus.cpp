#include <stdio.h>
#include "chorus.hpp"

START_NAMESPACE_DISTRHO

   /**
      Plugin class constructor.
      You must set all parameter values to their defaults, matching the value in initParameter().
    */
Chorus::Chorus()
    : Plugin(PARAM_COUNT, 0, 0),
    delayLine(getSampleRate(), 1),
    lfo(getSampleRate(), 500.0f)
{
    /**
      Initialize all our parameters to their defaults.
      In this example all parameters have 0 as default, so we can simply zero them.
    */
}

// -------------------------------------------------------------------
// Init
// -------------------------------------------------------------------

/**
  Initialize the parameter @a index.
  This function will be called once, shortly after the plugin is created.
*/
void Chorus::initParameter(uint32_t index, Parameter& parameter)
{
    if (index >= PARAM_COUNT) { return; }

    struct ParamProps
    {
        bool automatable, boolean, integer, logarithmic, output, trigger;
        float min, max, def;
        const char* name;
        const char* symbol;
    };

    const auto setParamProps = [](auto& param, ParamProps props)
    {
        if(props.automatable){ param.hints |= kParameterIsAutomatable;}
        if(props.boolean){param.hints |= kParameterIsBoolean;}
        if(props.integer){param.hints |= kParameterIsInteger;}
        if(props.logarithmic){param.hints |= kParameterIsLogarithmic;}
        if(props.output){param.hints |= kParameterIsOutput;}
        if(props.trigger){param.hints |= kParameterIsTrigger | kParameterIsBoolean;}
        param.ranges.min = props.min;
        param.ranges.max = props.max;
        param.ranges.def = props.def;
        param.name = props.name;
        param.symbol = props.symbol;
    };

    switch (index)
    {
        // PARAMS
        case PARAM_GAIN:
            setParamProps(parameter, { .automatable=true, .min=0.0f, .max=2.0f, .def=1.0f, .name="Gain", .symbol="gain" });
            break;
        case PARAM_RATE:
            setParamProps(parameter, { .automatable=true,  .min=0.1f, .max=1.0f, .def=0.5f, .name="Rate", .symbol="rate" });
            lfo.setFrequency(rate);
            break;
        case PARAM_DEPTH:
            setParamProps(parameter, { .automatable=true, .min=0.0f, .max=1.0f, .def=0.5f, .name="Depth", .symbol="depth" });
            lfo.setAmplitude(depth);
            break;
        default:
            break;
    }
}

// -------------------------------------------------------------------
// Internal data
// -------------------------------------------------------------------

/**
  Get the current value of a parameter.
  The host may call this function from any context, including realtime processing.
*/
float Chorus::getParameterValue(uint32_t index) const
{
    switch (index)
    {
        // PARAMS
        case PARAM_GAIN:
            return gain;
        case PARAM_RATE:
            return rate;
        case PARAM_DEPTH:
            return depth;
        default:
            return 0;
    }
}

/**
  Change a parameter value.
  The host may call this function from any context, including realtime processing.
  When a parameter is marked as automable, you must ensure no non-realtime
  operations are performed.
*/
void Chorus::setParameterValue(uint32_t index, float value)
{
    switch (index)
    {
    // PARAMS
    case PARAM_GAIN:
        gain = value;
        break;
    case PARAM_RATE:
        rate = value;
        lfo.setFrequency(rate);
        break;
    case PARAM_DEPTH:
        depth = value;
        lfo.setAmplitude(depth);
        break;
    default:
        break;
    }
}

// -------------------------------------------------------------------
// methods
// -------------------------------------------------------------------

// -------------------------------------------------------------------
// Process

void Chorus::activate()
{
    // plugin is activated
}

void Chorus::deactivate()
{
    // plugin is deactivated
}

/**
  Run/process function for plugins with audio input.
*/
void Chorus::run(const float** inputs, float** outputs, uint32_t nframes)
{
    const float* const input = inputs[0];
          float* output      = outputs[0];

    // run
    for (uint32_t currentFrame = 0; currentFrame < nframes; ++currentFrame)
    {
        delayLine.write(input[currentFrame]);

        // process
        delayLine.setDistanceReadWriteHead(lfo.getSample() + 1.0f);

        output[currentFrame] = 0.5f * (input[currentFrame] * (1 - gain) + delayLine.read() * gain);

        lfo.tick();
        delayLine.tick();
    }
}

// -------------------------------------------------------------------
// callbacks

/**
    Optional callback to inform the plugin about a sample rate change.
    This function will only be called when the plugin is deactivated.
*/
void Chorus::sampleRateChanged(double newSampleRate)
{
    (void)newSampleRate;
    delayLine.setSampleRate(newSampleRate);
    lfo.setSampleRate(newSampleRate);

}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new Chorus();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO