#include "plugin.hpp"


struct QuadSteppedOffset : Module {
	enum ParamIds {
		OFFSET1_PARAM,
		OFFSET2_PARAM,
		OFFSET3_PARAM,
		OFFSET4_PARAM,
		SEMITRSP1_PARAM,
		SEMITRSP1INV_PARAM,
		SEMITRSP2_PARAM,
		SEMITRSP2INV_PARAM,
		SEMITRSP3_PARAM,
		SEMITRSP3INV_PARAM,
		SEMITRSP4_PARAM,
		SEMITRSP4INV_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		OFFSET1IN_INPUT,
		OFFSET2IN_INPUT,
		OFFSET3IN_INPUT,
		OFFSET4IN_INPUT,
		SEMITRSP1IN_INPUT,
		SEMITRSP2IN_INPUT,
		SEMITRSP3IN_INPUT,
		SEMITRSP4IN_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OFFSET1OUT_OUTPUT,
		OFFSET2OUT_OUTPUT,
		OFFSET3OUT_OUTPUT,
		OFFSET4OUT_OUTPUT,
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	

	QuadSteppedOffset() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(OFFSET1_PARAM, -5.0, 5.0, 0.0,"Octave offset 1");
		configParam(OFFSET2_PARAM, -5.0, 5.0, 0.0,"Octave offset 2");
		configParam(OFFSET3_PARAM, -5.0, 5.0, 0.0,"Octave offset 3");
		configParam(OFFSET4_PARAM, -5.0, 5.0, 0.0,"Octave offset 4");
		configParam(SEMITRSP1_PARAM, 0.0, 11.0, 0.0,"Semitone offset 1");
		configParam(SEMITRSP2_PARAM, 0.0, 11.0, 0.0,"Semitone offset 2");
		configParam(SEMITRSP3_PARAM, 0.0, 11.0, 0.0,"Semitone offset 3");
		configParam(SEMITRSP4_PARAM, 0.0, 11.0, 0.0,"Semitone offset 4");
	}

	void process(const ProcessArgs &args) override {

		float integer1out = (inputs[OFFSET1IN_INPUT].getVoltage()) + (params[OFFSET1_PARAM].getValue());
		float integer2out = (inputs[OFFSET2IN_INPUT].getVoltage()) + (params[OFFSET2_PARAM].getValue());
		float integer3out = (inputs[OFFSET3IN_INPUT].getVoltage()) + (params[OFFSET3_PARAM].getValue());
		float integer4out = (inputs[OFFSET4IN_INPUT].getVoltage()) + (params[OFFSET4_PARAM].getValue());
		
		outputs[OFFSET1OUT_OUTPUT].setVoltage(integer1out);
		outputs[OFFSET2OUT_OUTPUT].setVoltage(integer2out);
		outputs[OFFSET3OUT_OUTPUT].setVoltage(integer3out);
		outputs[OFFSET4OUT_OUTPUT].setVoltage(integer4out);
		
		float semitrsp1out = (params[SEMITRSP1_PARAM].getValue() / 12.0);
		
		if (params[SEMITRSP1INV_PARAM].getValue()) {
		semitrsp1out = ((params[SEMITRSP1_PARAM].getValue() / 12.0) * -1.0);
		}
		
		if (inputs[SEMITRSP1IN_INPUT].isConnected()) {
		semitrsp1out = inputs[SEMITRSP1IN_INPUT].getVoltage() + (params[SEMITRSP1_PARAM].getValue() / 12.0);
		
		
		if (params[SEMITRSP1INV_PARAM].getValue()) {
		semitrsp1out = inputs[SEMITRSP1IN_INPUT].getVoltage() + ((params[SEMITRSP1_PARAM].getValue() / 12.0) * -1.0);
		}	
		
		}
		
		float semitrsp1 = clamp(semitrsp1out, -10.f, 10.f);
		
		float semitrsp2out = (params[SEMITRSP2_PARAM].getValue() / 12.0);
		
		if (params[SEMITRSP2INV_PARAM].getValue()) {
		semitrsp2out = ((params[SEMITRSP1_PARAM].getValue() / 12.0) * -1.0);
		}
		
		
		if (inputs[SEMITRSP2IN_INPUT].isConnected()) {
		semitrsp2out = inputs[SEMITRSP2IN_INPUT].getVoltage() + (params[SEMITRSP2_PARAM].getValue() / 12.0);
		
		
		if (params[SEMITRSP2INV_PARAM].getValue()) {
		semitrsp2out = inputs[SEMITRSP2IN_INPUT].getVoltage() + ((params[SEMITRSP2_PARAM].getValue() / 12.0) * -1.0);
		}	
		
		}
		
		float semitrsp2 = clamp(semitrsp2out, -10.f, 10.f);
				
		float semitrsp3out = (params[SEMITRSP3_PARAM].getValue() / 12.0);
		
		if (params[SEMITRSP3INV_PARAM].getValue()) {
		semitrsp3out = ((params[SEMITRSP3_PARAM].getValue() / 12.0) * -1.0);
		}
		
		
		if (inputs[SEMITRSP3IN_INPUT].isConnected()) {
		semitrsp3out = inputs[SEMITRSP3IN_INPUT].getVoltage() + (params[SEMITRSP3_PARAM].getValue() / 12.0);
		
		
		if (params[SEMITRSP3INV_PARAM].getValue()) {
		semitrsp3out = inputs[SEMITRSP3IN_INPUT].getVoltage() + ((params[SEMITRSP3_PARAM].getValue() / 12.0) * -1.0);
		}	
		
		}
		
		float semitrsp3 = clamp(semitrsp3out, -10.f, 10.f);
		
		float semitrsp4out = (params[SEMITRSP4_PARAM].getValue() / 12.0);
		
		if (params[SEMITRSP4INV_PARAM].getValue()) {
		semitrsp4out = ((params[SEMITRSP4_PARAM].getValue() / 12.0) * -1.0);
		}
		
		
		if (inputs[SEMITRSP4IN_INPUT].isConnected()) {
		semitrsp4out = inputs[SEMITRSP4IN_INPUT].getVoltage() + (params[SEMITRSP4_PARAM].getValue() / 12.0);
		
		
		if (params[SEMITRSP4INV_PARAM].getValue()) {
		semitrsp4out = inputs[SEMITRSP4IN_INPUT].getVoltage() + ((params[SEMITRSP4_PARAM].getValue() / 12.0) * -1.0);
		}	
		
		}
		
		float semitrsp4 = clamp(semitrsp4out, -10.f, 10.f);
		
		outputs[OUT1_OUTPUT].setVoltage(semitrsp1);
		outputs[OUT2_OUTPUT].setVoltage(semitrsp2);
		outputs[OUT3_OUTPUT].setVoltage(semitrsp3);
		outputs[OUT4_OUTPUT].setVoltage(semitrsp4);
	}	
};


struct QuadSteppedOffsetWidget : ModuleWidget {
	QuadSteppedOffsetWidget(QuadSteppedOffset *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/QuadSteppedOffset.svg")));

		addParam(createParam<LFMSnapKnob>(Vec(40.5, 50), module, QuadSteppedOffset::OFFSET1_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(40.5, 125), module, QuadSteppedOffset::OFFSET2_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(40.5, 200), module, QuadSteppedOffset::OFFSET3_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(40.5, 275), module, QuadSteppedOffset::OFFSET4_PARAM));
		
		addInput(createInput<JackPort>(Vec(5, 45), module, QuadSteppedOffset::OFFSET1IN_INPUT));
		addInput(createInput<JackPort>(Vec(5, 120), module, QuadSteppedOffset::OFFSET2IN_INPUT));
		addInput(createInput<JackPort>(Vec(5, 195), module, QuadSteppedOffset::OFFSET3IN_INPUT));
		addInput(createInput<JackPort>(Vec(5, 270), module, QuadSteppedOffset::OFFSET4IN_INPUT));
	
		addOutput(createOutput<OutJackPort>(Vec(92, 45), module, QuadSteppedOffset::OFFSET1OUT_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(92, 120), module, QuadSteppedOffset::OFFSET2OUT_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(92, 195), module, QuadSteppedOffset::OFFSET3OUT_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(92, 270), module, QuadSteppedOffset::OFFSET4OUT_OUTPUT));
		
		addParam(createParam<LFMSnapKnob>(Vec(160.5, 50), module, QuadSteppedOffset::SEMITRSP1_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(160.5, 125), module, QuadSteppedOffset::SEMITRSP2_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(160.5, 200), module, QuadSteppedOffset::SEMITRSP3_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(160.5, 275), module, QuadSteppedOffset::SEMITRSP4_PARAM));
		
		addParam(createParam<MS>(Vec(126, 90), module, QuadSteppedOffset::SEMITRSP1INV_PARAM));
		addParam(createParam<MS>(Vec(126, 165), module, QuadSteppedOffset::SEMITRSP2INV_PARAM));
		addParam(createParam<MS>(Vec(126, 240), module, QuadSteppedOffset::SEMITRSP3INV_PARAM));
		addParam(createParam<MS>(Vec(126, 315), module, QuadSteppedOffset::SEMITRSP4INV_PARAM));
		
		addInput(createInput<JackPort>(Vec(124, 45), module, QuadSteppedOffset::SEMITRSP1IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 120), module, QuadSteppedOffset::SEMITRSP2IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 195), module, QuadSteppedOffset::SEMITRSP3IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 270), module, QuadSteppedOffset::SEMITRSP4IN_INPUT));

		addOutput(createOutput<OutJackPort>(Vec(212, 45), module, QuadSteppedOffset::OUT1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(212, 120), module, QuadSteppedOffset::OUT2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(212, 195), module, QuadSteppedOffset::OUT3_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(212, 270), module, QuadSteppedOffset::OUT4_OUTPUT));

	}
};


Model *modelQuadSteppedOffset = createModel<QuadSteppedOffset, QuadSteppedOffsetWidget>("QuadSteppedOffset");