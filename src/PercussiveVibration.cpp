#include "plugin.hpp"
#include "dsp/math.hpp"
#include "filter.h"

//Based on 21Khz PalmLoop sine fm engine
//https://github.com/21kHz/21kHz-rack-plugins
//Envs are simpler Rampage channels
//https://github.com/VCVRack/Befaco
//Included filter taken from Vult language basic examples
//https://github.com/modlfo/vult

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

struct PercussiveVibration : Module {
	enum ParamIds {
        OCT_PARAM,
        EXP_FM_PARAM,
		D_PARAM,
		HARM1LVL_PARAM,
		DMOD_PARAM,
		HARM1MOD_PARAM,
		FALL_A_PARAM,
		FALL_PITCH_PARAM,
		FALL_ACCENT_PARAM,
		FALL_FM_PARAM,
		FM_DEPTH_PARAM,
		RATIO_PARAM,
		SOURCE_PARAM,
		FM_MOD_PARAM,
		FALL_VCA_MOD_PARAM,
		FALL_PITCH_MOD_PARAM,
		FALL_ACC_MOD_PARAM,
		ACC_LONG_PARAM,
		VCA_LONG_PARAM,
		PITCH_LONG_PARAM,
		CUTOFF_PARAM,
		CUTOFFMOD_PARAM,
		CUTOFFMOD2_PARAM,
		RESO_PARAM,
		MODE_PARAM,
		ENVMOD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
        RESET_INPUT,
        V_OCT_INPUT,
        EXP_FM_INPUT,
        FM_MOD_INPUT,
		D_INPUT,
		V_INPUT,
		HARM1_INPUT,
		TRIGG_A_INPUT,
		ACCENT_A_INPUT,
		FALL_MOD_INPUT,
		PITCHFALL_MOD_INPUT,
		ACCFALL_MOD_INPUT,
		CUTMOD_INPUT,
		CUTMOD2_INPUT,
		VEL_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OSC_OUTPUT,
        SIN_OUTPUT,
		VCA_ENV_OUTPUT,
		PITCH_ENV_OUTPUT,
		ACC_ENV_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	Filter_process_type processor;
	
	dsp::SchmittTrigger VelTrigger;
	dsp::PulseGenerator on1;
    
    float phase = 0.0f;
    float oldPhase = 0.0f;
    float square = 1.0f;
    int discont = 0;
    int oldDiscont = 0;
	
	float phase2 = 0.0f;
    float oldPhase2 = 0.0f;
	float square2 = 1.0f;
	int discont2 = 0;
    int oldDiscont2 = 0;
    
    float log2sampleFreq = 15.4284f;
	
	float out = {};
	bool gate = {};
	dsp::SchmittTrigger trigger;
	
	float pitchout = {};
	bool pitchgate = {};
	dsp::SchmittTrigger pitchtrigger;
	
	float accentout = {};
	bool accentgate = {};
	dsp::SchmittTrigger accenttrigger;
	
	float fmout = {};
	bool fmgate = {};
	dsp::SchmittTrigger fmtrigger;
	
	float gate1 = 0.0;
	float sample1 = 0.0;

    
    dsp::SchmittTrigger resetTrigger;

	PercussiveVibration(){
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		
Filter_process_init(processor);


configParam(EXP_FM_PARAM, 0.0, 1.0, 0.0,"Pitch Enveloppe Depth");
configParam(OCT_PARAM, 4.0, 12.0, 8.0,"Octave");
configParam(D_PARAM, 0.1, 0.9, 0.0,"Sine shape");
configParam(HARM1LVL_PARAM, 0.0, 1.0, 1.0,"1st Harmonic Level");
configParam(DMOD_PARAM, -1.0, 1.0, 0.0,"Sine shape Modulation");
configParam(HARM1MOD_PARAM, -1.0, 1.0, 0.0,"1st Harmonic level modulation");
configParam(FALL_A_PARAM, 0.0, 1.0, 0.5,"Main env fall time");
configParam(FALL_PITCH_PARAM, 0.0, 1.0, 0.0,"Pitch env fall time" );
configParam(FM_DEPTH_PARAM, 0.0, 1.0, 0.0,"Fm Depth");
configParam(RATIO_PARAM, -2.0, 3.0, 0.0,"Modulator Ratio" );
configParam(SOURCE_PARAM, 0.0, 1.0, 0.0, "Oscillator source");
configParam(FM_MOD_PARAM, -1.0, 1.0, 0.0,"FM Depth modulation");
configParam(FALL_VCA_MOD_PARAM, -1.0, 1.0, 0.0,"Main env modulation");
configParam(FALL_PITCH_MOD_PARAM, -1.0, 1.0, 0.0,"Pitch env modulation");
configParam(FALL_ACC_MOD_PARAM, -1.0, 1.0, 0.0,"Accent env modulation");
configParam(ACC_LONG_PARAM, 0.0, 1.0, 0.0,"Accent env time");
configParam(VCA_LONG_PARAM, 0.0, 1.0, 0.0,"Main env time" );
configParam(PITCH_LONG_PARAM, 0.0, 1.0, 0.0,"Pitch env time");
configParam(CUTOFF_PARAM, 0.f, 0.9f, 0.9f,"Filter cutoff");
configParam(RESO_PARAM, 0.f, 1.0f, 0.f,"Filter resonance");
configParam(CUTOFFMOD_PARAM, -1.f, 1.0f, 0.f,"Filter cutoff mod");
configParam(CUTOFFMOD2_PARAM, -1.f, 1.0f, 0.f,"Filter cutoff mod 2");
configParam(MODE_PARAM, 0, 1, 0,"Filter type");
configParam(ENVMOD_PARAM, -1.0, 1.0, 0.0, "Filter cutoff env modulation");
}

void onSampleRateChange() override {
    log2sampleFreq = log2f(1.0f / APP->engine->getSampleTime()) - 0.00009f;
}

void process(const ProcessArgs &args) override{
	
		float gSampleTime = args.sampleTime;
	
    if (resetTrigger.process(inputs[TRIGG_A_INPUT].getVoltage())) {
        phase = 0.0f;
		phase2 = 0.0f;
    }
    
	float  tzharmmix = ((5.0f * sin_01(phase2)) * clamp((params[FM_DEPTH_PARAM].getValue() + ((inputs[FM_MOD_INPUT].getVoltage() / 10.0)  * params[FM_MOD_PARAM].getValue()) * params[FM_DEPTH_PARAM].getValue() + ((inputs[FM_MOD_INPUT].getVoltage() / 10.0)  * params[FM_MOD_PARAM].getValue()) * params[FM_DEPTH_PARAM].getValue() + ((inputs[FM_MOD_INPUT].getVoltage() / 10.0)  * params[FM_MOD_PARAM].getValue())),0.0f,1.0f));
  
    float freq = params[OCT_PARAM].getValue() + 0.031360 + inputs[V_OCT_INPUT].getVoltage();
	freq += params[EXP_FM_PARAM].getValue() * (pitchout / 2.0);
    if (freq >= log2sampleFreq) {
        freq = log2sampleFreq;
    }
    freq = powf(2.0f, freq);
    float incr = 0.0f;		
	freq += 7.0 * 7.0 * 7.0 * tzharmmix;
		

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
	
	float saw = (1.0)*((phase-d)/(1.0-d));
	
	if (phase < d) {
		 saw = 0.0;
	}

    oldPhase = phase;
    oldDiscont = discont;
	
	
	float freq2 = params[OCT_PARAM].getValue() + 0.031360 + inputs[V_OCT_INPUT].getVoltage() + params[RATIO_PARAM].getValue();
   freq2 += params[EXP_FM_PARAM].getValue() * (pitchout / 2.0);
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
	 
	 
		float in = 0.0;
			if (trigger.process(inputs[TRIGG_A_INPUT].getVoltage())) {
				gate = true;
			}
			if (gate) {
				in = 10.0;
			}

			float shape = 1.0;
			float delta = in - out;

			float minTime = 1e-3;
			if (params[VCA_LONG_PARAM].getValue() == 1.0) {
				minTime = 1e-2;
			}
	
			bool rising = false;
			bool falling = false;

			if (delta > 0) {
				
				float riseCv = 0.0; 
				riseCv = clamp(riseCv, 0.0f, 1.0f);
				float rise = minTime * std::pow(2.0, riseCv * 10.0);
				out += shapeDelta(delta, rise, shape) * args.sampleTime;
				rising = (in - out > 1e-3);
				if (!rising) {
					gate = false;
				}
			}
			else if (delta < 0) {
				float fallCv = clamp(params[FALL_A_PARAM].getValue() + ((inputs[FALL_MOD_INPUT].getVoltage() / 10.0f) * params[FALL_VCA_MOD_PARAM].getValue()),0.0,1.0);
				fallCv = clamp(fallCv, 0.0f, 1.0f);
				float fall = minTime * std::pow(2.0, (fallCv * 10.0));
				out += shapeDelta(delta, fall, shape) * args.sampleTime;
				falling = (in - out < -1e-3);
				}
			
		if (delta == 0.0){
				gate = false;
		}

			if (!rising && !falling) {
				out = in;
			}
			
			float pitchin = 0.0;
			if (pitchtrigger.process(inputs[TRIGG_A_INPUT].getVoltage())) {
				pitchgate = true;
			}
			if (pitchgate) {
				pitchin = 10.0;
			}

			float pitchshape = 1.0;
			float pitchdelta = pitchin - pitchout;

			float pitchminTime = 1e-3;
			if (params[PITCH_LONG_PARAM].getValue() == 1.0) {
			pitchminTime = 1e-2;
			}
	
			bool pitchrising = false;
			bool pitchfalling = false;

			if (pitchdelta > 0) {
				
				float pitchriseCv = 0.0; 
				pitchriseCv = clamp(pitchriseCv, 0.0f, 1.0f);
				float pitchrise = pitchminTime * std::pow(2.0, pitchriseCv * 10.0);
				pitchout += shapeDelta(pitchdelta, pitchrise, pitchshape) * args.sampleTime;
				pitchrising = (pitchin - pitchout > 1e-3);
				if (!pitchrising) {
					pitchgate = false;
				}
			}
			else if (pitchdelta < 0) {
				float pitchfallCv = clamp(params[FALL_PITCH_PARAM].getValue() + ((inputs[PITCHFALL_MOD_INPUT].getVoltage() / 10.0f) * params[FALL_PITCH_MOD_PARAM].getValue()),0.0,1.0);
				pitchfallCv = clamp(pitchfallCv, 0.0f, 1.0f);
				float pitchfall = pitchminTime * std::pow(2.0, pitchfallCv * 10.0);
				pitchout += shapeDelta(pitchdelta, pitchfall, pitchshape) * args.sampleTime;
				pitchfalling = (pitchin - pitchout < -1e-3);
				}
			
		if (pitchdelta == 0.0){
				pitchgate = false;
		}

			if (!pitchrising && !pitchfalling) {
				pitchout = pitchin;
			}

			float accentin = 0.0;
			if (accenttrigger.process(inputs[ACCENT_A_INPUT].getVoltage())) {
				accentgate = true;
			}
			if (accentgate) {
				accentin = 10.0;
			}

			float accentshape = 1.0;
			float accentdelta = accentin - accentout;

			float accentminTime = 1e-3;
			
			if (params[ACC_LONG_PARAM].getValue() == 1.0) {
			accentminTime = 1e-2;
			}
	
			bool accentrising = false;
			bool accentfalling = false;

			if (accentdelta > 0) {
				
				float accentriseCv = 0.0; 
				accentriseCv = clamp(accentriseCv, 0.0f, 1.0f);
				float accentrise = accentminTime * std::pow(2.0, accentriseCv * 10.0);
				accentout += shapeDelta(accentdelta, accentrise, accentshape) * args.sampleTime;
				accentrising = (accentin - accentout > 1e-3);
				if (!accentrising) {
					accentgate = false;
				}
			}
			else if (accentdelta < 0) {
				float accentfallCv = clamp(params[FALL_ACCENT_PARAM].getValue() + ((inputs[ACCFALL_MOD_INPUT].getVoltage() / 10.0f) * params[FALL_ACC_MOD_PARAM].getValue()),0.0,1.0); 
				accentfallCv = clamp(accentfallCv, 0.0f, 1.0f);
				float accentfall = accentminTime * std::pow(2.0, accentfallCv * 10.0);
				accentout += shapeDelta(accentdelta, accentfall, accentshape) * args.sampleTime;
				accentfalling = (accentin - accentout < -1e-3);
				}
			
		if (accentdelta == 0.0){
				accentgate = false;
		}

			if (!accentrising && !accentfalling) {
				accentout = accentin;
			}
			
			
			float noise = 2.0 * random::normal();
			float oscout = 0.0;
			
		if (VelTrigger.process(inputs[TRIGG_A_INPUT].getVoltage())){
		gate1 = true;
		}
		if(gate1) {	
		on1.trigger(0.01);
		gate1 = false;
	};
	
	float velcheck = (on1.process(gSampleTime) ? 10.0 : 0.0);
	if (velcheck){	
sample1 = (inputs[VEL_INPUT].getVoltage());
	}
	
	float rawoscout =   crossfade((5.0f * sin_01(saw) / 5.0), (noise / 5.0), params[SOURCE_PARAM].getValue());
					
		     if (outputs[SIN_OUTPUT].isConnected()) { 
			 oscout = (1.0 * (crossfade((5.0f * sin_01(saw) * (out / 5.0)), (noise * out/5.0), params[SOURCE_PARAM].getValue())));
			 if (inputs[VEL_INPUT].isConnected()) {	 
			 oscout = ((1.0 * (sample1/ 10.0)) * (crossfade((5.0f * sin_01(saw) * (out / 5.0)), (noise * out/5.0), params[SOURCE_PARAM].getValue())));
			 }
			 }
			 
			 if (outputs[VCA_ENV_OUTPUT].isConnected()) {
		outputs[VCA_ENV_OUTPUT].setVoltage(out);
			 }		
			if (outputs[PITCH_ENV_OUTPUT].isConnected()) {
		outputs[PITCH_ENV_OUTPUT].setVoltage(pitchout);
			 }	
			if (outputs[ACC_ENV_OUTPUT].isConnected()) {
		outputs[ACC_ENV_OUTPUT].setVoltage(accentout);
			 }					 
			 
		float x = oscout / 10.0f;
		float cv = params[CUTOFF_PARAM].getValue() + ((out / 5.0f) * params[ENVMOD_PARAM].getValue()) + ((inputs[CUTMOD_INPUT].getVoltage() / 10.0f) * params[CUTOFFMOD_PARAM].getValue()) + ((inputs[CUTMOD2_INPUT].getVoltage() / 10.0f) * params[CUTOFFMOD2_PARAM].getValue()) ;
		cv = clamp (cv, 0.0, 0.9);
		float q = params[RESO_PARAM].getValue();
	    int sel = params[MODE_PARAM].getValue(); 
		float sr = args.sampleRate;
			 
		float filterout = Filter_process(processor,x, cv, q, sel,sr);	 
		filterout = clamp(filterout,-1.0,1.0);
				
				outputs[SIN_OUTPUT].setVoltage( filterout * 5.0);
				outputs[OSC_OUTPUT].setVoltage( rawoscout * 5.0);
			 	
}

};


	struct PercussiveVibrationWidget : ModuleWidget {	
	PercussiveVibrationWidget(PercussiveVibration *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PercussiveVibration.svg")));


		addParam(createParam<LFMSnapKnob>(Vec(70, 40), module, PercussiveVibration::OCT_PARAM));

		addParam(createParam<LFMKnob>(Vec(130, 40), module, PercussiveVibration::EXP_FM_PARAM));
		addParam(createParam<LFMKnob>(Vec(250, 40), module, PercussiveVibration::D_PARAM));     
		addParam(createParam<LFMTinyKnob>(Vec(263, 127), module, PercussiveVibration::DMOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(248, 110), module, PercussiveVibration::D_INPUT));

		addParam(createParam<LFMSliderWhite>(Vec(25, 253), module, PercussiveVibration::FALL_A_PARAM));
		addParam(createParam<LFMSliderWhite>(Vec(145, 253), module, PercussiveVibration::FALL_PITCH_PARAM));
		addParam(createParam<LFMSliderWhite>(Vec(265, 253), module, PercussiveVibration::FALL_ACCENT_PARAM));

		addParam(createParam<LFMKnob>(Vec(70, 110), module, PercussiveVibration::FM_DEPTH_PARAM));
		addParam(createParam<LFMKnob>(Vec(10, 110), module, PercussiveVibration::RATIO_PARAM));
		addParam(createParam<LFMKnob>(Vec(190, 40), module, PercussiveVibration::SOURCE_PARAM));
		
		addParam(createParam<MS>(Vec(314, 190), module, PercussiveVibration::MODE_PARAM));
		
		addParam(createParam<LFMKnob>(Vec(310, 40), module, PercussiveVibration::CUTOFF_PARAM));
		addParam(createParam<LFMKnob>(Vec(370, 40), module, PercussiveVibration::ENVMOD_PARAM));
		addParam(createParam<LFMKnob>(Vec(310, 110), module, PercussiveVibration::RESO_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(383, 127), module, PercussiveVibration::CUTOFFMOD_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(383, 197), module, PercussiveVibration::CUTOFFMOD2_PARAM));
		
		addInput(createInput<MiniJackPort>(Vec(368, 110), module, PercussiveVibration::CUTMOD_INPUT));
		addInput(createInput<MiniJackPort>(Vec(368, 180), module, PercussiveVibration::CUTMOD2_INPUT));
				
		addParam(createParam<LFMTinyKnob>(Vec(148, 127), module, PercussiveVibration::FM_MOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(128, 110), module, PercussiveVibration::FM_MOD_INPUT));

		addInput(createInput<JackPort>(Vec(18, 190), module, PercussiveVibration::TRIGG_A_INPUT));
		addInput(createInput<JackPort>(Vec(258, 190), module, PercussiveVibration::ACCENT_A_INPUT));
		addInput(createInput<JackPort>(Vec(78, 190), module, PercussiveVibration::VEL_INPUT));
    
		addInput(createInput<JackPort>(Vec(18, 50), module, PercussiveVibration::V_OCT_INPUT));    

		addOutput(createOutput<OutJackPort>(Vec(378, 320), module, PercussiveVibration::SIN_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(378, 260), module, PercussiveVibration::OSC_OUTPUT));
	
		addOutput(createOutput<OutJackPort>(Vec(88, 260), module, PercussiveVibration::VCA_ENV_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(208, 260), module, PercussiveVibration::PITCH_ENV_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(328, 260), module, PercussiveVibration::ACC_ENV_OUTPUT));
		
		addInput(createInput<MiniJackPort>(Vec(68, 305), module, PercussiveVibration::FALL_MOD_INPUT));
		addParam(createParam<LFMTinyKnob>(Vec(83, 322), module, PercussiveVibration::FALL_VCA_MOD_PARAM));
		
		addInput(createInput<MiniJackPort>(Vec(188, 305), module, PercussiveVibration::PITCHFALL_MOD_INPUT));
		addParam(createParam<LFMTinyKnob>(Vec(203, 322), module, PercussiveVibration::FALL_PITCH_MOD_PARAM));
		
		addInput(createInput<MiniJackPort>(Vec(308, 305), module, PercussiveVibration::ACCFALL_MOD_INPUT));
		addParam(createParam<LFMTinyKnob>(Vec(323, 322), module, PercussiveVibration::FALL_ACC_MOD_PARAM));
			
		addParam(createParam<MS>(Vec(48, 260), module, PercussiveVibration::VCA_LONG_PARAM));
		addParam(createParam<MS>(Vec(168, 260), module, PercussiveVibration::PITCH_LONG_PARAM));
		addParam(createParam<MS>(Vec(288, 260), module, PercussiveVibration::ACC_LONG_PARAM));
	}

};


Model *modelPercussiveVibration = createModel<PercussiveVibration, PercussiveVibrationWidget>("PercussiveVibration");