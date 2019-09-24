#include "plugin.hpp"
#include "dsp/math.hpp"

//Based on 21Khz PalmLoop sine fm engine
//https://github.com/21kHz/21kHz-rack-plugins
//Env taken from Rampage 
//https://github.com/VCVRack/Befaco

static float shapeDelta(float delta, float tau, float shape) {
	float lin = sgn(delta) * 10.f / tau;
	if (shape < 0.f) {
		float log = sgn(delta) * 40.f / tau / (std::fabs(delta) + 1.f);
		return crossfade(lin, log, -shape * 0.95f);
	}
	else {
		float exp = M_E * delta / tau;
		return crossfade(lin, exp, shape * 0.90f);
	}
}

struct AdditiveVibration : Module {
	enum ParamIds {
        OCT_PARAM,
		D_PARAM,
		V_PARAM,
		HARM1LVL_PARAM,
		HARM2LVL_PARAM,
		HARM3LVL_PARAM,
		DMOD_PARAM,
		VMOD_PARAM,
		HARM1MOD_PARAM,
		HARM2MOD_PARAM,
		HARM3MOD_PARAM,
		TZ_PARAM,
		HARMMIX_PARAM,
		TZENVMOD_PARAM,
		TZMOD_PARAM,
		TZMOD2_PARAM,
		RANGE_A_PARAM,
		SHAPE_A_PARAM,
		TRIGG_A_PARAM,
		RISE_A_PARAM,
		FALL_A_PARAM,
		CYCLE_A_PARAM,
		CV_A_DEST_PARAM,
		CV_A_MOD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
        V_OCT_INPUT,
		V_OCT2_INPUT,
		V_OCT3_INPUT,
		D_INPUT,
		V_INPUT,
		HARM1_INPUT,
		HARM2_INPUT,
		HARM3_INPUT,
		TZMOD_INPUT,
		TZMOD2_INPUT,
		CV_A_INPUT,
		TRIGG_A_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
        SIN_OUTPUT,
		OUT_A_OUTPUT,
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	    
    float phase = 0.0f;
    float oldPhase = 0.0f;
    float square = 1.0f;
    int discont = 0;
    int oldDiscont = 0;
	
	float phaseA = 0.0f;
    float oldPhaseA = 0.0f;
    float squareA = 1.0f;
    int discontA = 0;
    int oldDiscontA = 0;
	
	float phaseB = 0.0f;
    float oldPhaseB = 0.0f;
    float squareB = 1.0f;
    int discontB = 0;
    int oldDiscontB = 0;
	
	float phase2 = 0.0f;
    float oldPhase2 = 0.0f;
	float square2 = 1.0f;
	int discont2 = 0;
    int oldDiscont2 = 0;
	
	float phase2A = 0.0f;
    float oldPhase2A = 0.0f;
	float square2A = 1.0f;
	int discont2A = 0;
    int oldDiscont2A = 0;
	
	float phase2B = 0.0f;
    float oldPhase2B = 0.0f;
	float square2B = 1.0f;
	int discont2B = 0;
    int oldDiscont2B = 0;
    
	float phase3 = 0.0f;
    float oldPhase3 = 0.0f;
	float square3 = 1.0f;
	int discont3 = 0;
    int oldDiscont3 = 0;
	
	float phase3A = 0.0f;
    float oldPhase3A = 0.0f;
	float square3A = 1.0f;
	int discont3A = 0;
    int oldDiscont3A = 0;
	
	float phase3B = 0.0f;
    float oldPhase3B = 0.0f;
	float square3B = 1.0f;
	int discont3B = 0;
    int oldDiscont3B = 0;
  
   	float phase4 = 0.0f;
    float oldPhase4 = 0.0f;
	float square4 = 1.0f;
	int discont4 = 0;
    int oldDiscont4 = 0;
	
	float phase4A = 0.0f;
    float oldPhase4A = 0.0f;
	float square4A = 1.0f;
	int discont4A = 0;
    int oldDiscont4A = 0;
	
	float phase4B = 0.0f;
    float oldPhase4B = 0.0f;
	float square4B = 1.0f;
	int discont4B = 0;
    int oldDiscont4B = 0;
	
	float phase5 = 0.0f;
    float oldPhase5 = 0.0f;
	float square5 = 1.0f;
	int discont5 = 0;
    int oldDiscont5 = 0;
	
	float phase5A = 0.0f;
    float oldPhase5A = 0.0f;
	float square5A = 1.0f;
	int discont5A = 0;
    int oldDiscont5A = 0;
	
	float phase5B = 0.0f;
    float oldPhase5B = 0.0f;
	float square5B = 1.0f;
	int discont5B = 0;
    int oldDiscont5B = 0;
	
	float phase6 = 0.0f;
    float oldPhase6 = 0.0f;
	float square6 = 1.0f;
	int discont6 = 0;
    int oldDiscont6 = 0;
	
	float phase6A = 0.0f;
    float oldPhase6A = 0.0f;
	float square6A = 1.0f;
	int discont6A = 0;
    int oldDiscont6A = 0;
	
	float phase6B = 0.0f;
    float oldPhase6B = 0.0f;
	float square6B = 1.0f;
	int discont6B = 0;
    int oldDiscont6B = 0;
	
    float log2sampleFreq = 15.4284f;
	
	float tzharmmix = 0.0f;
	float tzharmmixA = 0.0f;
	float tzharmmixB = 0.0f;
    
    dsp::SchmittTrigger resetTrigger;
	
	float out = {};
	bool gate = {};
	dsp::SchmittTrigger trigger;
	dsp::PulseGenerator endOfCyclePulse;
	
	AdditiveVibration(){
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
configParam(OCT_PARAM, 4.0, 12.0, 8.0,"Octave");
configParam(D_PARAM, 0.1, 0.9, 0.0,"Sine Width");
configParam(V_PARAM, 0.0, 0.9, 0.0,"Sine Curve");
configParam(HARM1LVL_PARAM, 0.0, 1.0, 1.0,"1st Harmonic level");
configParam(HARM2LVL_PARAM, 0.0, 1.0, 1.0,"2nd Harmonic level");
configParam(HARM3LVL_PARAM, 0.0, 1.0, 1.0,"3rd Harmonic level");
configParam(DMOD_PARAM, -1.0, 1.0, 0.0,"Sine Width modulation");
configParam(VMOD_PARAM, -1.0, 1.0, 0.0,"Sine Curve modulation");
configParam(HARM1MOD_PARAM, -1.0, 1.0, 0.0,"1st Harmonic level mod");
configParam(HARM2MOD_PARAM, -1.0, 1.0, 0.0,"2nd Harmonic level mod");
configParam(HARM3MOD_PARAM, -1.0, 1.0, 0.0,"3rd Harmonic level mod");
configParam(TZ_PARAM, 0.0, 7.0, 0.0,"FM Depth");
configParam(HARMMIX_PARAM, 0.0, 7.0, 0.0, "Harmonics set");
configParam(TZMOD_PARAM, -1.0, 1.0, 0.0, "FM Depth mod");
configParam(TZMOD2_PARAM, -1.0, 1.0, 0.0,"FM Depth mod2");
configParam(RANGE_A_PARAM, 0.0, 2.0, 0.0, "Env time");
configParam(SHAPE_A_PARAM, 0.0, 1.0, 0.0, "Env shape");
configParam(TRIGG_A_PARAM, 0.0, 1.0, 0.0, "Env trigger");
configParam(RISE_A_PARAM, 0.0, 1.0, 0.0, "Env rise time");
configParam(FALL_A_PARAM, 0.0, 1.0, 0.0, "Env fall time");
configParam(CYCLE_A_PARAM, 0.0, 1.0, 0.0, "Lfo mode");
configParam(CV_A_DEST_PARAM, 0.0, 2.0, 0.0, "Env mod destination");
configParam(CV_A_MOD_PARAM, -0.5, 0.5, 0.0, "Env mod amount");
configParam(TZENVMOD_PARAM, -1.0, 1.0, 0.0,"FM Depth env mod");

	}


void onSampleRateChange() override {
    log2sampleFreq = log2f(1.0f / APP->engine->getSampleTime()) - 0.00009f;
}


void process(const ProcessArgs &args) override{
	
    float freq = params[OCT_PARAM].getValue() + 0.031360 + inputs[V_OCT_INPUT].getVoltage();

    if (freq >= log2sampleFreq) {
        freq = log2sampleFreq;
    }
    freq = powf(2.0f, freq);
    float incr = 0.0f;
			
		freq += clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * tzharmmix;
	
        incr = args.sampleTime * freq;
        incr = args.sampleTime * freq;
        if (incr > 1.0f) {
            incr = 1.0f;
        }
        else if (incr < -1.0f) {
            incr = -1.0f;
        }
    
    phase += incr;
    if (phase >= 0.0f && phase < 1.0f) {
        discont = 0;
    }
    else if (phase >= 1.0f) {
        discont = 1;
        --phase;
        square *= -1.0f;
    }
    else {
        discont = -1;
        ++phase;
        square *= -1.0f;
    }
	
	float d = clamp((params[D_PARAM].getValue() + ((inputs[D_INPUT].getVoltage() / 10.0f) * params[DMOD_PARAM].getValue())),0.1,0.9);
	float v = clamp((params[V_PARAM].getValue() + ((inputs[V_INPUT].getVoltage() / 10.0f) * params[VMOD_PARAM].getValue())),0.0,0.9);
	
	float saw = (1.0-v)*((phase-d)/(1.0-d)) + v ;
	
	if (phase < d) {
		 saw = (v * (phase)) / d;
	}
	    
    oldPhase = phase;
    oldDiscont = discont;
	
	float freqA = params[OCT_PARAM].getValue() + 0.031360 + inputs[V_OCT2_INPUT].getVoltage();

    if (freqA >= log2sampleFreq) {
        freqA = log2sampleFreq;
    }
    freqA = powf(2.0f, freqA);
    float incrA = 0.0f;
			
		freqA += clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * tzharmmixA;
        incrA = args.sampleTime * freqA;
        incrA = args.sampleTime * freqA;
        if (incrA > 1.0f) {
            incrA = 1.0f;
        }
        else if (incrA < -1.0f) {
            incrA = -1.0f;
        }
    
    phaseA += incrA;
    if (phaseA >= 0.0f && phaseA < 1.0f) {
        discontA = 0;
    }
    else if (phaseA >= 1.0f) {
        discontA = 1;
        --phaseA;
        squareA *= -1.0f;
    }
    else {
        discontA = -1;
        ++phaseA;
        squareA *= -1.0f;
    }
		
	float sawA = (1.0-v)*((phaseA-d)/(1.0-d)) + v ;
	
	if (phaseA < d) {
		 sawA = (v * (phaseA)) / d;
	}
	
	oldPhaseA = phaseA;
    oldDiscontA = discontA;	
	
	float freqB = params[OCT_PARAM].getValue() + 0.031360 + inputs[V_OCT3_INPUT].getVoltage();	
	    
    if (freqB >= log2sampleFreq) {
        freqB = log2sampleFreq;
    }
    freqB = powf(2.0f, freqB);
    float incrB = 0.0f;
		freqB += clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * clamp(((params[TZENVMOD_PARAM].getValue() * out) + params[TZ_PARAM].getValue() + (inputs[TZMOD_INPUT].getVoltage() * params[TZMOD_PARAM].getValue()) + (inputs[TZMOD2_INPUT].getVoltage() * params[TZMOD2_PARAM].getValue())), 0.0f, 10.0f) * tzharmmixB;
        incrB = args.sampleTime * freqB;
        incrB = args.sampleTime * freqB;
        if (incrB > 1.0f) {
            incrB = 1.0f;
        }
        else if (incrB < -1.0f) {
            incrB = -1.0f;
        }
    
    phaseB += incrB;
    if (phaseB >= 0.0f && phaseB < 1.0f) {
        discontB = 0;
    }
    else if (phaseB >= 1.0f) {
        discontB = 1;
        --phaseB;
        squareB *= -1.0f;
    }
    else {
        discontB = -1;
        ++phaseB;
        squareB *= -1.0f;
    }
		
	float sawB = (1.0-v)*((phaseB-d)/(1.0-d)) + v ;
	
	if (phaseB < d) {
		 sawB = (v * (phaseB)) / d;
	}
	
	oldPhaseB = phaseB;
    oldDiscontB = discontB;
			
	float freq2 = params[OCT_PARAM].getValue() + 0.031360  +  inputs[V_OCT_INPUT].getVoltage() + 1.0;

    if (freq2 >= log2sampleFreq) {
        freq2 = log2sampleFreq;
    }
    freq2 = powf(2.0f, freq2);
    float incr2 = 0.0f;
	incr2 = args.sampleTime * freq2;
	phase2 += incr2;

    if (phase2 >= 0.0f && phase2 < 1.0f) {
        discont2 = 0;
    }
    else if (phase2 >= 1.0f) {
        discont2 = 1;
        --phase2;
        square2 *= -1.0f;
    }
    else {
        discont2 = -1;
        ++phase2;
        square2 *= -1.0f;
    }
	 oldPhase2 = phase2;
	 oldDiscont2 = discont2;
	 	 
	float freq3 = params[OCT_PARAM].getValue() + 0.031360  +  inputs[V_OCT_INPUT].getVoltage() + 1.0 + ((1.0/12.0) * 7.0) + + ((1.0/12.0) * 0.02);

    if (freq3 >= log2sampleFreq) {
        freq3 = log2sampleFreq;
    }
    freq3 = powf(2.0f, freq3);
    float incr3 = 0.0f;
	incr3 = args.sampleTime * freq3;
	phase3 += incr3;

    if (phase3 >= 0.0f && phase3 < 1.0f) {
        discont3 = 0;
    }
    else if (phase3 >= 1.0f) {
        discont3 = 1;
        --phase3;
        square3 *= -1.0f;
    }
    else {
        discont3 = -1;
        ++phase3;
        square3 *= -1.0f;
    }
	 oldPhase3 = phase3;
	 oldDiscont3 = discont3;
	 
	float freq4 = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT_INPUT].getVoltage() + 2.0 ;

    if (freq4 >= log2sampleFreq) {
        freq4 = log2sampleFreq;
    }
    freq4 = powf(2.0f, freq4);
    float incr4 = 0.0f;
	incr4 = args.sampleTime * freq4;
	phase4 += incr4;

    if (phase4 >= 0.0f && phase4 < 1.0f) {
        discont4 = 0;
    }
    else if (phase4 >= 1.0f) {
        discont4 = 1;
        --phase4;
        square4 *= -1.0f;
    }
    else {
        discont4 = -1;
        ++phase4;
        square4 *= -1.0f;
    }
	 oldPhase4 = phase4;
	 oldDiscont4 = discont4;
	 	 
	float freq5 = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT_INPUT].getVoltage() + 2.0 + ((1.0 / 12.0) * 4.0) - ((1.0/12.0) * 0.14);

    if (freq5 >= log2sampleFreq) {
        freq5 = log2sampleFreq;
    }
    freq5 = powf(2.0f, freq5);
    float incr5 = 0.0f;
	incr5 = args.sampleTime * freq5;
	phase5 += incr5;

    if (phase5 >= 0.0f && phase5 < 1.0f) {
        discont5 = 0;
    }
    else if (phase5 >= 1.0f) {
        discont5 = 1;
        --phase5;
        square5 *= -1.0f;
    }
    else {
        discont5 = -1;
        ++phase5;
        square5 *= -1.0f;
    }
	 oldPhase5 = phase5;
	 oldDiscont5 = discont5;

	 
	float freq6 = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT_INPUT].getVoltage() + 2.0 + ((1.0 / 12.0) * 7.0) + ((1.0/12.0) * 0.02) ;

    if (freq6 >= log2sampleFreq) {
        freq6 = log2sampleFreq;
    }
    freq6 = powf(2.0f, freq6);
    float incr6 = 0.0f;
	incr6 = args.sampleTime * freq6;
	phase6 += incr6;

    if (phase6 >= 0.0f && phase6 < 1.0f) {
        discont6 = 0;
    }
    else if (phase6 >= 1.0f) {
        discont6 = 1;
        --phase6;
        square6 *= -1.0f;
    }
    else {
        discont6 = -1;
        ++phase6;
        square6 *= -1.0f;
    }
	 oldPhase6 = phase6;
	 oldDiscont6 = discont6;
	
	float freq2A = params[OCT_PARAM].getValue() + 0.031360  +  inputs[V_OCT2_INPUT].getVoltage() + 1.0;

    if (freq2A >= log2sampleFreq) {
        freq2A = log2sampleFreq;
    }
    freq2A = powf(2.0f, freq2A);
    float incr2A = 0.0f;
	incr2A = args.sampleTime * freq2A;
	phase2A += incr2A;

    if (phase2A >= 0.0f && phase2A < 1.0f) {
        discont2A = 0;
    }
    else if (phase2A >= 1.0f) {
        discont2A = 1;
        --phase2A;
        square2A *= -1.0f;
    }
    else {
        discont2A = -1;
        ++phase2A;
        square2A *= -1.0f;
    }
	 oldPhase2A = phase2A;
	 oldDiscont2A = discont2A;
	
	float freq3A = params[OCT_PARAM].getValue() + 0.031360  +  inputs[V_OCT2_INPUT].getVoltage() + 1.0 + ((1.0/12.0) * 7.0) + + ((1.0/12.0) * 0.02);
	 
	if (freq3A >= log2sampleFreq) {
        freq3A = log2sampleFreq;
    }
    freq3A = powf(2.0f, freq3A);
    float incr3A = 0.0f;
	incr3A = args.sampleTime * freq3A;
	phase3A += incr3A;

    if (phase3A >= 0.0f && phase3A < 1.0f) {
        discont3A = 0;
    }
    else if (phase3A >= 1.0f) {
        discont3A = 1;
        --phase3A;
        square3A *= -1.0f;
    }
    else {
        discont3A = -1;
        ++phase3A;
        square3A *= -1.0f;
    }
	 oldPhase3A = phase3A;
	 oldDiscont3A = discont3A;
	 
	float freq4A = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT2_INPUT].getVoltage() + 2.0 ;

    if (freq4A >= log2sampleFreq) {
        freq4A = log2sampleFreq;
    }
    freq4A = powf(2.0f, freq4A);
    float incr4A = 0.0f;
	incr4A = args.sampleTime * freq4A;
	phase4A += incr4A;

    if (phase4A >= 0.0f && phase4A < 1.0f) {
        discont4A = 0;
    }
    else if (phase4A >= 1.0f) {
        discont4A = 1;
        --phase4A;
        square4A *= -1.0f;
    }
    else {
        discont4A = -1;
        ++phase4A;
        square4A *= -1.0f;
    }
	 oldPhase4A = phase4A;
	 oldDiscont4A = discont4A;	

	float freq5A = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT2_INPUT].getVoltage() + 2.0 + ((1.0 / 12.0) * 4.0) - ((1.0/12.0) * 0.14);

    if (freq5A >= log2sampleFreq) {
        freq5A = log2sampleFreq;
    }
    freq5A = powf(2.0f, freq5A);
    float incr5A = 0.0f;
	incr5A = args.sampleTime * freq5A;
	phase5A += incr5A;

    if (phase5A >= 0.0f && phase5A < 1.0f) {
        discont5A = 0;
    }
    else if (phase5A >= 1.0f) {
        discont5A = 1;
        --phase5A;
        square5A *= -1.0f;
    }
    else {
        discont5A = -1;
        ++phase5A;
        square5A *= -1.0f;
    }
	 oldPhase5A = phase5A;
	 oldDiscont5A = discont5A;
	 
	float freq6A = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT2_INPUT].getVoltage() + 2.0 + ((1.0 / 12.0) * 7.0) + ((1.0/12.0) * 0.02) ;

    if (freq6A >= log2sampleFreq) {
        freq6A = log2sampleFreq;
    }
    freq6A = powf(2.0f, freq6A);
    float incr6A = 0.0f;
	incr6A = args.sampleTime * freq6A;
	phase6A += incr6A;

    if (phase6A >= 0.0f && phase6A < 1.0f) {
        discont6A = 0;
    }
    else if (phase6A >= 1.0f) {
        discont6A = 1;
        --phase6A;
        square6A *= -1.0f;
    }
    else {
        discont6A = -1;
        ++phase6A;
        square6A *= -1.0f;
    }
	 oldPhase6A = phase6A;
	 oldDiscont6A = discont6A;	 
	 		 

	float freq2B = params[OCT_PARAM].getValue() + 0.031360  +  inputs[V_OCT3_INPUT].getVoltage() + 1.0;

    if (freq2B >= log2sampleFreq) {
        freq2B = log2sampleFreq;
    }
    freq2B = powf(2.0f, freq2B);
    float incr2B = 0.0f;
	incr2B = args.sampleTime * freq2B;
	phase2B += incr2B;

    if (phase2B >= 0.0f && phase2B < 1.0f) {
        discont2B = 0;
    }
    else if (phase2B >= 1.0f) {
        discont2B = 1;
        --phase2B;
        square2B *= -1.0f;
    }
    else {
        discont2B = -1;
        ++phase2B;
        square2B *= -1.0f;
    }
	 oldPhase2B = phase2B;
	 oldDiscont2B = discont2B;
	 
	float freq3B = params[OCT_PARAM].getValue() + 0.031360  +  inputs[V_OCT3_INPUT].getVoltage() + 1.0 + ((1.0/12.0) * 7.0) + + ((1.0/12.0) * 0.02);

    if (freq3B >= log2sampleFreq) {
        freq3B = log2sampleFreq;
    }
    freq3B = powf(2.0f, freq3B);
    float incr3B = 0.0f;
	incr3B = args.sampleTime * freq3B;
	phase3B += incr3B;

    if (phase3B >= 0.0f && phase3B < 1.0f) {
        discont3B = 0;
    }
    else if (phase3B >= 1.0f) {
        discont3B = 1;
        --phase3B;
        square3B *= -1.0f;
    }
    else {
        discont3B = -1;
        ++phase3B;
        square3B *= -1.0f;
    }
	 oldPhase3B = phase3B;
	 oldDiscont3B = discont3B;
	 
	float freq4B = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT3_INPUT].getVoltage() + 2.0 ;

    if (freq4B >= log2sampleFreq) {
        freq4B = log2sampleFreq;
    }
    freq4B = powf(2.0f, freq4B);
    float incr4B = 0.0f;
	incr4B = args.sampleTime * freq4B;
	phase4B += incr4B;

    if (phase4B >= 0.0f && phase4B < 1.0f) {
        discont4B = 0;
    }
    else if (phase4B >= 1.0f) {
        discont4B = 1;
        --phase4B;
        square4B *= -1.0f;
    }
    else {
        discont4B = -1;
        ++phase4B;
        square4B *= -1.0f;
    }
	 oldPhase4B = phase4B;
	 oldDiscont4B = discont4B;
	 
	float freq5B = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT3_INPUT].getVoltage() + 2.0 + ((1.0 / 12.0) * 4.0) - ((1.0/12.0) * 0.14);

    if (freq5B >= log2sampleFreq) {
        freq5B = log2sampleFreq;
    }
    freq5B = powf(2.0f, freq5B);
    float incr5B = 0.0f;
	incr5B = args.sampleTime * freq5B;
	phase5B += incr5B;

    if (phase5B >= 0.0f && phase5B < 1.0f) {
        discont5B = 0;
    }
    else if (phase5B >= 1.0f) {
        discont5B = 1;
        --phase5B;
        square5B *= -1.0f;
    }
    else {
        discont5B = -1;
        ++phase5B;
        square5B *= -1.0f;
    }
	 oldPhase5B = phase5B;
	 oldDiscont5B = discont5B;
	 
	float freq6B = params[OCT_PARAM].getValue() + 0.031360 +  inputs[V_OCT2_INPUT].getVoltage() + 2.0 + ((1.0 / 12.0) * 7.0) + ((1.0/12.0) * 0.02) ;

    if (freq6B >= log2sampleFreq) {
        freq6B = log2sampleFreq;
    }
    freq6B = powf(2.0f, freq6B);
    float incr6B = 0.0f;
	incr6B = args.sampleTime * freq6B;
	phase6B += incr6B;

    if (phase6B >= 0.0f && phase6B < 1.0f) {
        discont6B = 0;
    }
    else if (phase6B >= 1.0f) {
        discont6B = 1;
        --phase6B;
        square6B *= -1.0f;
    }
    else {
        discont6B = -1;
        ++phase6B;
        square6B *= -1.0f;
    }
	 oldPhase6B = phase6B;
	 oldDiscont6B = discont6B;


float osc1 = 5.0f * sin_01(saw);
float osc2 = 5.0f * sin_01(sawA);
float osc3 = 5.0f * sin_01(sawB);

outputs[SIN_OUTPUT].setVoltage(osc1);
outputs[AUDIO_OUTPUT].setVoltage(osc1 * (out/10.0));

if (inputs[V_OCT2_INPUT].isConnected()){
	outputs[SIN_OUTPUT].setVoltage((osc1 + osc2)/2.0);
	outputs[AUDIO_OUTPUT].setVoltage(((osc1 + osc2)/2.0) * (out/10.0));
}
if (inputs[V_OCT3_INPUT].isConnected()){
	outputs[SIN_OUTPUT].setVoltage((osc1 + osc3)/2.0);
	outputs[AUDIO_OUTPUT].setVoltage(((osc1 + osc3)/2.0) * (out/10.0)); 
}

if (inputs[V_OCT2_INPUT].isConnected() && inputs[V_OCT3_INPUT].isConnected()){
	outputs[SIN_OUTPUT].setVoltage((osc1 + osc2 + osc3) / 3.0);
	outputs[AUDIO_OUTPUT].setVoltage(((osc1 + osc2 + osc3) / 3.0) * (out/10.0));
}
	
	 tzharmmix =  ((5.0f * sin_01(phase2) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;
	 
	 if (params[HARMMIX_PARAM].getValue() == 1.0) {
	tzharmmix =  ((5.0f * sin_01(phase2) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase3) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 2.0) {
	tzharmmix =  ((5.0f * sin_01(phase4) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 3.0) {
	tzharmmix =  ((5.0f * sin_01(phase3) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 4.0) {
	tzharmmix =  ((5.0f * sin_01(phase2) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 
	 if (params[HARMMIX_PARAM].getValue() == 5.0) {
	tzharmmix =  ((5.0f * sin_01(phase3) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	  if (params[HARMMIX_PARAM].getValue() == 6.0) {
	tzharmmix =  ((5.0f * sin_01(phase4) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	if (params[HARMMIX_PARAM].getValue() == 7.0) {
	tzharmmix =  ((5.0f * sin_01(phase3) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 
	tzharmmixA =  ((5.0f * sin_01(phase2A) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4A) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6A) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;
	 
	 if (params[HARMMIX_PARAM].getValue() == 1.0) {
	tzharmmixA =  ((5.0f * sin_01(phase2A) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase3A) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5A) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 2.0) {
	tzharmmixA =  ((5.0f * sin_01(phase4A) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5A) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6A) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 3.0) {
	tzharmmixA =  ((5.0f * sin_01(phase3A) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4A) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5A) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 4.0) {
	tzharmmixA =  ((5.0f * sin_01(phase2A) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5A) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6A) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 
	 if (params[HARMMIX_PARAM].getValue() == 5.0) {
	tzharmmixA =  ((5.0f * sin_01(phase3A) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5A) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6A) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	  if (params[HARMMIX_PARAM].getValue() == 6.0) {
	tzharmmixA =  ((5.0f * sin_01(phase4A) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5A) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6A) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	if (params[HARMMIX_PARAM].getValue() == 7.0) {
	tzharmmixA =  ((5.0f * sin_01(phase3A) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4A) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6A) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 
	 tzharmmixB =  ((5.0f * sin_01(phase2B) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4B) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6B) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;
	 
	 if (params[HARMMIX_PARAM].getValue() == 1.0) {
	tzharmmixB =  ((5.0f * sin_01(phase2B) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase3B) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5B) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 2.0) {
	tzharmmixB =  ((5.0f * sin_01(phase4B) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5B) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6B) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 3.0) {
	tzharmmixB =  ((5.0f * sin_01(phase3B) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4B) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5B) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 
	  if (params[HARMMIX_PARAM].getValue() == 4.0) {
	tzharmmixB =  ((5.0f * sin_01(phase2B) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5B) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6B) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 
	 if (params[HARMMIX_PARAM].getValue() == 5.0) {
	tzharmmixB =  ((5.0f * sin_01(phase3B) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5B) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6B) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	  if (params[HARMMIX_PARAM].getValue() == 6.0) {
	tzharmmixB =  ((5.0f * sin_01(phase4B) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase5B) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6B) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0))  / 3.0) ;	 
	 }
	if (params[HARMMIX_PARAM].getValue() == 7.0) {
	tzharmmixB =  ((5.0f * sin_01(phase3B) * clamp((params[HARM1LVL_PARAM].getValue() + clamp(((inputs[HARM1_INPUT].getVoltage() / 10.0f) * params[HARM1MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase4B) * clamp((params[HARM2LVL_PARAM].getValue()+ clamp(((inputs[HARM2_INPUT].getVoltage() / 10.0f) * params[HARM2MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) + (5.0f * sin_01(phase6B) * clamp((params[HARM3LVL_PARAM].getValue()+ clamp(((inputs[HARM3_INPUT].getVoltage() / 10.0f) * params[HARM3MOD_PARAM].getValue()),-1.0,1.0)),0.0,1.0)) / 3.0) ;	 
	 }
	 

			float in = 0.0;
			if (trigger.process(inputs[TRIGG_A_INPUT].getVoltage())) {
				gate = true;
			}
			if (gate) {
				in = 10.0;
			}

			float shape = params[SHAPE_A_PARAM].getValue();
			float delta = in - out;

			float minTime;
			switch ((int) params[RANGE_A_PARAM].getValue()) {
				case 0: minTime = 1e-2; break;
				case 1: minTime = 1e-3; break;
				default: minTime = 1e-1; break;
			}

			bool rising = false;
			bool falling = false;

			if (delta > 0) {
				
				float riseCv = params[RISE_A_PARAM].getValue(); 
				if (params[CV_A_DEST_PARAM].getValue() == 2.0) {
				riseCv = params[RISE_A_PARAM].getValue()+ (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0 ;
				}
				if (params[CV_A_DEST_PARAM].getValue() == 0.0) {
				riseCv = params[RISE_A_PARAM].getValue()+ (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0 ;
				}
				riseCv = clamp(riseCv, 0.0f, 1.0f);
				float rise = minTime * std::pow(2.0, riseCv * 10.0);
				out += shapeDelta(delta, rise, shape) * args.sampleTime;
				rising = (in - out > 1e-3);
				if (!rising) {
					gate = false;
				}
			}
			else if (delta < 0) {
				
				float fallCv = params[FALL_A_PARAM].getValue(); 
				if (params[CV_A_DEST_PARAM].getValue() == 2.0) {
				fallCv = params[FALL_A_PARAM].getValue() + (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0;
				}
				if (params[CV_A_DEST_PARAM].getValue() == 1.0) {
				fallCv = params[FALL_A_PARAM].getValue() + (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue())/ 10.0;
				}
				fallCv = clamp(fallCv, 0.0f, 1.0f);
				float fall = minTime * std::pow(2.0, fallCv * 10.0);
				out += shapeDelta(delta, fall, shape) * args.sampleTime;
				falling = (in - out < -1e-3);
				if (!falling) {
					endOfCyclePulse.trigger(1e-3);
					if (params[CYCLE_A_PARAM].getValue()) {
						gate = true;
					}
				}
			}
			else {
				gate = false;
			}

			if (!rising && !falling) {
				out = in;
			}

outputs[OUT_A_OUTPUT].setVoltage(out);

}

};

	struct AdditiveVibrationWidget : ModuleWidget {	
	AdditiveVibrationWidget(AdditiveVibration *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AdditiveVibration.svg")));

		addParam(createParam<LFMSnapKnob>(Vec(10, 40), module, AdditiveVibration::OCT_PARAM));

		addParam(createParam<LFMKnob>(Vec(10, 110), module, AdditiveVibration::D_PARAM));
		addParam(createParam<LFMKnob>(Vec(130, 110), module, AdditiveVibration::V_PARAM));
		addParam(createParam<LFMKnob>(Vec(250, 40), module, AdditiveVibration::TZ_PARAM));
		addParam(createParam<LFMKnob>(Vec(250, 110), module, AdditiveVibration::TZENVMOD_PARAM));
		
		addParam(createParam<LFMSliderWhite>(Vec(25.5, 190), module, AdditiveVibration::HARM1LVL_PARAM));
		addParam(createParam<LFMSliderWhite>(Vec(85.5, 190), module, AdditiveVibration::HARM2LVL_PARAM));
		addParam(createParam<LFMSliderWhite>(Vec(145.5, 190), module, AdditiveVibration::HARM3LVL_PARAM));

		addParam(createParam<LFMTinyKnob>(Vec(83, 127), module, AdditiveVibration::DMOD_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(203, 127), module, AdditiveVibration::VMOD_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(28, 317), module, AdditiveVibration::HARM1MOD_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(88, 317), module, AdditiveVibration::HARM2MOD_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(148, 317), module, AdditiveVibration::HARM3MOD_PARAM));

		addParam(createParam<LFMSnapKnob>(Vec(190, 230), module, AdditiveVibration::HARMMIX_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(263, 197), module, AdditiveVibration::TZMOD_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(263, 267), module, AdditiveVibration::TZMOD2_PARAM));
		
		addInput(createInput<MiniJackPort>(Vec(68, 110), module, AdditiveVibration::D_INPUT));
		addInput(createInput<MiniJackPort>(Vec(188, 110), module, AdditiveVibration::V_INPUT));
		addInput(createInput<MiniJackPort>(Vec(8, 300), module, AdditiveVibration::HARM1_INPUT));
		addInput(createInput<MiniJackPort>(Vec(68, 300), module, AdditiveVibration::HARM2_INPUT));
		addInput(createInput<MiniJackPort>(Vec(128, 300), module, AdditiveVibration::HARM3_INPUT));
		addInput(createInput<MiniJackPort>(Vec(248, 180), module, AdditiveVibration::TZMOD_INPUT));
		addInput(createInput<MiniJackPort>(Vec(248, 250), module, AdditiveVibration::TZMOD2_INPUT));
		
		addInput(createInput<JackPort>(Vec(78, 50), module, AdditiveVibration::V_OCT_INPUT));
		addInput(createInput<JackPort>(Vec(138, 50), module, AdditiveVibration::V_OCT2_INPUT));
		addInput(createInput<JackPort>(Vec(198, 50), module, AdditiveVibration::V_OCT3_INPUT));

		addOutput(createOutput<OutJackPort>(Vec(258, 318), module, AdditiveVibration::SIN_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(318, 318), module, AdditiveVibration::AUDIO_OUTPUT));

       addParam(createParam<LFMKnob>(Vec(310, 40), module, AdditiveVibration::RISE_A_PARAM));
	   addParam(createParam<LFMKnob>(Vec(310, 110), module, AdditiveVibration::FALL_A_PARAM));
	   addInput(createInput<MiniJackPort>(Vec(308, 180), module, AdditiveVibration::CV_A_INPUT));
	   addParam(createParam<LFMTinyKnob>(Vec(323, 197), module, AdditiveVibration::CV_A_MOD_PARAM));
	   addParam(createParam<LFMSwitch>(Vec(370, 48), module, AdditiveVibration::RANGE_A_PARAM));
	   addParam(createParam<MS>(Vec(368, 130), module, AdditiveVibration::SHAPE_A_PARAM));
	   addParam(createParam<MS>(Vec(398, 130), module, AdditiveVibration::CYCLE_A_PARAM));
	   addParam(createParam<LFMSwitch>(Vec(370, 188), module, AdditiveVibration::CV_A_DEST_PARAM));
	   addInput(createInput<JackPort>(Vec(318, 260), module, AdditiveVibration::TRIGG_A_INPUT));
	   addOutput(createOutput<OutJackPort>(Vec(378, 260), module, AdditiveVibration::OUT_A_OUTPUT));
  	
	}
	
};


Model *modelAdditiveVibration = createModel<AdditiveVibration, AdditiveVibrationWidget>("AdditiveVibration");