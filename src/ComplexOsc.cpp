#include "plugin.hpp"
#include "processor.h"

struct ComplexOsc : Module {
	enum ParamIds {
		KNOB2_PARAM,
		KNOB3_PARAM,
		KNOB4_PARAM,
		KNOB5_PARAM,
		KNOB6_PARAM,
		KNOB7_PARAM,
		D2_MOD_PARAM,
		V2_MOD_PARAM,
		D_MOD_PARAM,
		V_MOD_PARAM,
		FM_MOD_PARAM,
		CROSS_PARAM,
		CROSS_MOD_PARAM,
		MODFINE_PARAM,
		MODCOARSE_PARAM,
		CARFINE_PARAM,
		CARCOARSE_PARAM,
		OCTAVE_PARAM,
		OSCMIX_PARAM,
		OSCMIX_MOD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		A1IN_INPUT,
		A2IN_INPUT,
		D2_MOD_INPUT,
		D_MOD_INPUT,
		V2_MOD_INPUT,
		V_MOD_INPUT,
		FM_MOD_INPUT,
		CROSS_INPUT,
		OSCMIX_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	Processor_process_type processor;
   
   float phase = 0.0;
   float phase2 = 0.0;
   
   

	ComplexOsc() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(KNOB2_PARAM, 0.0, 0.9, 0.0,"D");
		configParam(D_MOD_PARAM, -1.0, 1.0, 0.0,"Dmod");
		configParam(KNOB3_PARAM, 0.0, 1.0, 0.0,"v");
		configParam(V_MOD_PARAM, -1.0, 1.0, 0.0,"Vmod");
		configParam(KNOB4_PARAM, 0.0, 1.0, 0.0,"fm");
		configParam(KNOB5_PARAM, -2.0, 2.0, 0.0,"offset");
		configParam(OCTAVE_PARAM, -2.0, 2.0, 0.0,"Octave");
		configParam(KNOB6_PARAM, 0.0, 0.9, 0.0,"D2");
		configParam(D2_MOD_PARAM, -1.0, 1.0, 0.0,"D2mod");
		configParam(KNOB7_PARAM, 0.0, 1.0, 0.0,"v2");
		configParam(V2_MOD_PARAM, -1.0, 1.0, 0.0,"V2mod");
		configParam(FM_MOD_PARAM, -1.0, 1.0, 0.0,"FMmod");
		configParam(CROSS_PARAM, 0.0, 1.0, 0.0,"Cross");
		configParam(CROSS_MOD_PARAM, -1.0, 1.0, 0.0,"CrossMod");
		configParam(MODFINE_PARAM, -1.0, 1.0, 0.0,"ModFine");
		configParam(CARFINE_PARAM, -1.0, 1.0, 0.0,"CarFine");
		configParam(MODCOARSE_PARAM, 0.0, 12.0, 0.0,"ModCoarse");
		configParam(CARCOARSE_PARAM, 0.0, 12.0, 0.0,"CarCoarse");
		configParam(OSCMIX_PARAM, 0.0, 1.0, 0.0,"OscMix");
		configParam(OSCMIX_MOD_PARAM, -1.0, 1.0, 0.0,"OscMixmod");
		Processor_process_init(processor);
	}

	void process(const ProcessArgs &args) {
		
		float pitch = params[OCTAVE_PARAM].getValue() + (params[CARFINE_PARAM].getValue() / 120.0) + (params[CARCOARSE_PARAM].getValue() / 12.0);
		

        pitch += inputs[A1IN_INPUT].getVoltage();
        pitch = clamp(pitch, -4.f, 4.f);

        float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);
		

        phase += freq * args.sampleTime;
        if (phase >= 0.5f)
            phase -= 1.f;
		
		float modpitch = inputs[A1IN_INPUT].getVoltage();
		
		if (inputs[A2IN_INPUT].isConnected()) {
			modpitch = inputs[A2IN_INPUT].getVoltage();
		}
			
		
		float pitch2 = params[KNOB5_PARAM].getValue() + (params[MODFINE_PARAM].getValue() / 120.0) + (params[MODCOARSE_PARAM].getValue() / 12.0) ;
        pitch2 += modpitch;
        pitch2 = clamp(pitch2, -4.f, 4.f);

        float freq2 = dsp::FREQ_C4 * std::pow(2.f, pitch2);
		

        phase2 += freq2 * args.sampleTime;
        if (phase2 >= 0.5f)
            phase2 -= 1.f;
		
		
		
		float in1 = phase;
		float in2 = 0.5  - ((params[KNOB2_PARAM].getValue() + ((inputs[D_MOD_INPUT].getVoltage() / 10.0f) * params[D_MOD_PARAM].getValue())) / 2.0);
		in2 = clamp(in2 , 0.1, 1.0);
		float in3 = params[KNOB3_PARAM].getValue() + ((inputs[V_MOD_INPUT].getVoltage() / 10.0f) * params[V_MOD_PARAM].getValue()) + 1.0;
		in3 = clamp(in3 , 1.0, 2.0);
		float in4 = params[KNOB4_PARAM].getValue() + ((inputs[FM_MOD_INPUT].getVoltage() / 10.0f) * params[FM_MOD_PARAM].getValue());
		in4 = clamp(in4 , 0.0, 1.0);
		float in5 = phase2;
		float in6 = 0.5 - ((params[KNOB6_PARAM].getValue() + ((inputs[D2_MOD_INPUT].getVoltage() / 10.0f) * params[D2_MOD_PARAM].getValue())) / 2.0);
		in6 = clamp(in6 , 0.1, 1.0);
		float in7 = params[KNOB7_PARAM].getValue() + ((inputs[V2_MOD_INPUT].getVoltage() / 10.0f) * params[V2_MOD_PARAM].getValue()) + 1.0;
		in7 = clamp(in7 , 1.0, 2.0);
	
		
		float xfade = params[CROSS_PARAM].getValue() + ((inputs[CROSS_INPUT].getVoltage() / 10.0f) * params[CROSS_MOD_PARAM].getValue());
		xfade = clamp(xfade , 0.0, 1.0);
		float in8 = xfade;
		
		float in9 = params[OSCMIX_PARAM].getValue() + ((inputs[OSCMIX_INPUT].getVoltage() / 10.0f) * params[OSCMIX_MOD_PARAM].getValue());;
		
		Processor_process(processor, in1, in2, in3, in4, in5, in6, in7, in8, in9);
		

		
		outputs[OUT1_OUTPUT].setVoltage(crossfade ((Processor_process_ret_2(processor) * 5.0f),(Processor_process_ret_0(processor) * 5.0),xfade));
		outputs[OUT2_OUTPUT].setVoltage(Processor_process_ret_1(processor) * 5.0f);
		outputs[OUT3_OUTPUT].setVoltage(Processor_process_ret_3(processor) * 5.0f);


		
	}	
};


struct ComplexOscWidget : ModuleWidget {
	ComplexOscWidget(ComplexOsc *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComplexOsc.svg")));

		addInput(createInput<JackPort>(Vec(13, 320), module, ComplexOsc::A1IN_INPUT));
		addInput(createInput<JackPort>(Vec(53, 320), module, ComplexOsc::A2IN_INPUT));
		
		addOutput(createOutput<OutJackPort>(Vec(170, 320), module, ComplexOsc::OUT1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(205, 320), module, ComplexOsc::OUT2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(240, 320), module, ComplexOsc::OUT3_OUTPUT));
		

		addParam(createParam<LFMKnob>(Vec(170, 125), module, ComplexOsc::KNOB2_PARAM));
		addParam(createParam<LFMKnob>(Vec(170, 215), module, ComplexOsc::KNOB3_PARAM));
		addParam(createParam<LFMKnob>(Vec(112.5, 30), module, ComplexOsc::KNOB4_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(165, 25), module, ComplexOsc::KNOB5_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(10, 25), module, ComplexOsc::OCTAVE_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(173, 90), module, ComplexOsc::MODFINE_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(13, 90), module, ComplexOsc::CARFINE_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(220, 38), module, ComplexOsc::MODCOARSE_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(60, 38), module, ComplexOsc::CARCOARSE_PARAM));
		addParam(createParam<LFMKnob>(Vec(10, 125), module, ComplexOsc::KNOB6_PARAM));
		addParam(createParam<LFMKnob>(Vec(10, 215), module, ComplexOsc::KNOB7_PARAM));
		
		
		addParam(createParam<LFMKnob>(Vec(112.5, 145), module, ComplexOsc::OSCMIX_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(130, 222), module, ComplexOsc::OSCMIX_MOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(110, 207), module, ComplexOsc::OSCMIX_INPUT));
		
		addParam(createParam<LFMTinyKnob>(Vec(70, 150), module, ComplexOsc::D2_MOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(58, 130), module, ComplexOsc::D2_MOD_INPUT));
		
		addParam(createParam<LFMTinyKnob>(Vec(230, 150), module, ComplexOsc::D_MOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(218, 130), module, ComplexOsc::D_MOD_INPUT));
		
		addParam(createParam<LFMTinyKnob>(Vec(70, 240), module, ComplexOsc::V2_MOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(58, 220), module, ComplexOsc::V2_MOD_INPUT));
		
		addParam(createParam<LFMTinyKnob>(Vec(230, 240), module, ComplexOsc::V_MOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(218, 220), module, ComplexOsc::V_MOD_INPUT));
		
		addParam(createParam<LFMTinyKnob>(Vec(130, 107), module, ComplexOsc::FM_MOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(110, 92), module, ComplexOsc::FM_MOD_INPUT));
		
		addParam(createParam<LFMKnob>(Vec(112.5, 260), module, ComplexOsc::CROSS_PARAM));
		
		addParam(createParam<LFMTinyKnob>(Vec(130, 337), module, ComplexOsc::CROSS_MOD_PARAM));
		addInput(createInput<MiniJackPort>(Vec(110, 322), module, ComplexOsc::CROSS_INPUT));
		
		
	}
};

Model *modelComplexOsc = createModel<ComplexOsc, ComplexOscWidget>("ComplexOsc");