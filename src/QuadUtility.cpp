#include "plugin.hpp"


struct QuadUtility : Module {
	enum ParamIds {
		SCALE_PARAM,
		SCALE2_PARAM,
		SCALE3_PARAM,
		SCALE4_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
	A1IN_INPUT,
	A2IN_INPUT,
	B1IN_INPUT,
	B2IN_INPUT,
	C1IN_INPUT,
	C2IN_INPUT,
	D1IN_INPUT,
	D2IN_INPUT,
	SCALEIN_INPUT,
	SCALEIN2_INPUT,
	SCALEIN3_INPUT,
	SCALEIN4_INPUT,
	VCAA1IN_INPUT,
	VCAA2IN_INPUT,
	VCAB1IN_INPUT,
	VCAB2IN_INPUT,
	VCAC1IN_INPUT,
	VCAC2IN_INPUT,
	VCAD1IN_INPUT,
	VCAD2IN_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
	OUT1_OUTPUT,
	OUT2_OUTPUT,
	OUT3_OUTPUT,
	OUT4_OUTPUT,
	SCALEOUT_OUTPUT,
	SCALEOUT2_OUTPUT,
	SCALEOUT3_OUTPUT,
	SCALEOUT4_OUTPUT,
	VCAOUT1_OUTPUT,
	VCAOUT2_OUTPUT,
	VCAOUT3_OUTPUT,
	VCAOUT4_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	


	QuadUtility() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SCALE_PARAM, 0.0, 2.0, 0.0,"Scale1");
		configParam(SCALE2_PARAM, 0.0, 2.0, 0.0,"Scale2");
		configParam(SCALE3_PARAM, 0.0, 2.0, 0.0,"Scale3");
		configParam(SCALE4_PARAM, 0.0, 2.0, 0.0,"Scale4");
	}

	void process(const ProcessArgs &args) override {
		
		float vcaout1 = (inputs[VCAA1IN_INPUT].getVoltage()) * ((inputs[VCAA2IN_INPUT].getVoltage()) / 10.0);
		outputs[VCAOUT1_OUTPUT].setVoltage(vcaout1);
		
		float vcaout2 = (inputs[VCAB1IN_INPUT].getVoltage()) * ((inputs[VCAB2IN_INPUT].getVoltage()) / 10.0);
		outputs[VCAOUT2_OUTPUT].setVoltage(vcaout2);
		
		float vcaout3 = (inputs[VCAC1IN_INPUT].getVoltage()) * ((inputs[VCAC2IN_INPUT].getVoltage()) / 10.0);
		outputs[VCAOUT3_OUTPUT].setVoltage(vcaout3);
		
		float vcaout4 = (inputs[VCAD1IN_INPUT].getVoltage()) * ((inputs[VCAD2IN_INPUT].getVoltage()) / 10.0);
		outputs[VCAOUT4_OUTPUT].setVoltage(vcaout4);
		

		float out1 = (inputs[A1IN_INPUT].getVoltage()) + (inputs[A2IN_INPUT].getVoltage());
		float out2 = (inputs[B1IN_INPUT].getVoltage()) + (inputs[B2IN_INPUT].getVoltage());
		float out3 = (inputs[C1IN_INPUT].getVoltage()) + (inputs[C2IN_INPUT].getVoltage());
		float out4 = (inputs[D1IN_INPUT].getVoltage()) + (inputs[D2IN_INPUT].getVoltage());
		
		outputs[OUT1_OUTPUT].setVoltage(out1);
		outputs[OUT2_OUTPUT].setVoltage(out2);
		outputs[OUT3_OUTPUT].setVoltage(out3);
		outputs[OUT4_OUTPUT].setVoltage(out4);
		
		float scaleout = (inputs[SCALEIN_INPUT].getVoltage());
		
		if (inputs[SCALEIN_INPUT].isConnected()) {
		
		if (params[SCALE_PARAM].getValue() == 2.0) {
			scaleout = (inputs[SCALEIN_INPUT].getVoltage() * 0.1);
		}
		
		if (params[SCALE_PARAM].getValue() == 1.0) {
			scaleout = (inputs[SCALEIN_INPUT].getVoltage() * 0.2);
		}

		if (params[SCALE_PARAM].getValue() == 0.0) {
		scaleout = (inputs[SCALEIN_INPUT].getVoltage() * 0.5);	
		}

		}

		outputs[SCALEOUT_OUTPUT].setVoltage(scaleout);
		
		float scaleout2 = (inputs[SCALEIN2_INPUT].getVoltage());
		
		if (inputs[SCALEIN2_INPUT].isConnected()) {
		
		if (params[SCALE2_PARAM].getValue() == 2.0) {
			scaleout2 = (inputs[SCALEIN2_INPUT].getVoltage() * 0.1);
		}
		
		if (params[SCALE2_PARAM].getValue() == 1.0) {
			scaleout2 = (inputs[SCALEIN2_INPUT].getVoltage() * 0.2);
		}

		if (params[SCALE2_PARAM].getValue() == 0.0) {
		scaleout2 = (inputs[SCALEIN2_INPUT].getVoltage() * 0.5);	
		}

		}

		outputs[SCALEOUT2_OUTPUT].setVoltage(scaleout2);
		
		float scaleout3 = (inputs[SCALEIN3_INPUT].getVoltage());
		
		if (inputs[SCALEIN3_INPUT].isConnected()) {
		
		if (params[SCALE3_PARAM].getValue() == 2.0) {
			scaleout3 = (inputs[SCALEIN3_INPUT].getVoltage() * 0.1);
		}
		
		if (params[SCALE3_PARAM].getValue() == 1.0) {
			scaleout3 = (inputs[SCALEIN3_INPUT].getVoltage() * 0.2);
		}

		if (params[SCALE3_PARAM].getValue() == 0.0) {
		scaleout3 = (inputs[SCALEIN3_INPUT].getVoltage() * 0.5);	
		}

		}

		outputs[SCALEOUT3_OUTPUT].setVoltage(scaleout3);
		
		float scaleout4 = (inputs[SCALEIN4_INPUT].getVoltage());
		
		if (inputs[SCALEIN4_INPUT].isConnected()) {
		
		if (params[SCALE4_PARAM].getValue() == 2.0) {
			scaleout4 = (inputs[SCALEIN4_INPUT].getVoltage() * 0.1);
		}
		
		if (params[SCALE4_PARAM].getValue() == 1.0) {
			scaleout4 = (inputs[SCALEIN4_INPUT].getVoltage() * 0.2);
		}

		if (params[SCALE4_PARAM].getValue() == 0.0) {
		scaleout4 = (inputs[SCALEIN4_INPUT].getVoltage() * 0.5);	
		}

		}

		outputs[SCALEOUT4_OUTPUT].setVoltage(scaleout4);
		
	}	
};


struct QuadUtilityWidget : ModuleWidget {
	QuadUtilityWidget(QuadUtility *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/QuadUtility.svg")));

		addInput(createInput<JackPort>(Vec(94, 30), module, QuadUtility::A1IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 30), module, QuadUtility::A2IN_INPUT));
		addInput(createInput<JackPort>(Vec(94, 90), module, QuadUtility::B1IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 90), module, QuadUtility::B2IN_INPUT));
		addInput(createInput<JackPort>(Vec(94, 150), module, QuadUtility::C1IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 150), module, QuadUtility::C2IN_INPUT));
		addInput(createInput<JackPort>(Vec(94, 210), module, QuadUtility::D1IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 210), module, QuadUtility::D2IN_INPUT));
	
		addOutput(createOutput<OutJackPort>(Vec(154, 30), module, QuadUtility::OUT1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(154, 90), module, QuadUtility::OUT2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(154, 150), module, QuadUtility::OUT3_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(154, 210), module, QuadUtility::OUT4_OUTPUT));
		
		addParam(createParam<LFMSwitch>(Vec(30, 30), module, QuadUtility::SCALE_PARAM));
		addInput(createInput<JackPort>(Vec(4, 30), module, QuadUtility::SCALEIN_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(64, 30), module, QuadUtility::SCALEOUT_OUTPUT));
		
		addParam(createParam<LFMSwitch>(Vec(30, 90), module, QuadUtility::SCALE2_PARAM));
		addInput(createInput<JackPort>(Vec(4, 90), module, QuadUtility::SCALEIN2_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(64, 90), module, QuadUtility::SCALEOUT2_OUTPUT));
		
		addParam(createParam<LFMSwitch>(Vec(30, 150), module, QuadUtility::SCALE3_PARAM));
		addInput(createInput<JackPort>(Vec(4, 150), module, QuadUtility::SCALEIN3_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(64, 150), module, QuadUtility::SCALEOUT3_OUTPUT));
		
		addParam(createParam<LFMSwitch>(Vec(30, 210), module, QuadUtility::SCALE4_PARAM));
		addInput(createInput<JackPort>(Vec(4, 210), module, QuadUtility::SCALEIN4_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(64, 210), module, QuadUtility::SCALEOUT4_OUTPUT));
				
		addInput(createInput<JackPort>(Vec(4, 270), module, QuadUtility::VCAA1IN_INPUT));
		addInput(createInput<JackPort>(Vec(34, 270), module, QuadUtility::VCAA2IN_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(64, 270), module, QuadUtility::VCAOUT1_OUTPUT));
		
		addInput(createInput<JackPort>(Vec(4, 330), module, QuadUtility::VCAB1IN_INPUT));
		addInput(createInput<JackPort>(Vec(34, 330), module, QuadUtility::VCAB2IN_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(64, 330), module, QuadUtility::VCAOUT2_OUTPUT));
		
		addInput(createInput<JackPort>(Vec(94, 270), module, QuadUtility::VCAC1IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 270), module, QuadUtility::VCAC2IN_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(154, 270), module, QuadUtility::VCAOUT3_OUTPUT));
		
		addInput(createInput<JackPort>(Vec(94, 330), module, QuadUtility::VCAD1IN_INPUT));
		addInput(createInput<JackPort>(Vec(124, 330), module, QuadUtility::VCAD2IN_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(154, 330), module, QuadUtility::VCAOUT4_OUTPUT));
		
	}
};

Model *modelQuadUtility = createModel<QuadUtility, QuadUtilityWidget>("QuadUtility");