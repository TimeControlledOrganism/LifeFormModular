#include "plugin.hpp"

#define minLength 0.01f

//Based on ML Switches
////https://github.com/martin-lueders/ML_modules
//Added sample and hold, input scaler, designed to work for pitch sequences

struct PitchIntegrator : Module {
	enum ParamIds {
		NUM_STEPS,
		STEP1_PARAM,
		STEP2_PARAM,
		STEP3_PARAM,
		STEP4_PARAM,
		STEP5_PARAM,
		STEP6_PARAM,
		STEP7_PARAM,
		STEP8_PARAM,
		LENGTH_PARAM,
		LENGTHMOD_PARAM,
		SCALE_PARAM,
		SCALEINV_PARAM,
		SCALEON_PARAM,
		OCTTRSP_PARAM,
		SEMITRSP_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		TRIGUP_INPUT,
		RESET_INPUT,
		NUMSTEPS_INPUT,
		VAR_INPUT,
		VARMOD_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		STEP1_LIGHT,
		STEP2_LIGHT,
		STEP3_LIGHT,
		STEP4_LIGHT,
		STEP5_LIGHT,
		STEP6_LIGHT,
		STEP7_LIGHT,
		STEP8_LIGHT,
		NUM_LIGHTS
	};

	PitchIntegrator() {
	config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );
	configParam(STEP1_PARAM, 0.0, 1.0, 0.0,"Step1");
	configParam(STEP2_PARAM, 0.0, 1.0, 0.0,"Step2");
	configParam(STEP3_PARAM, 0.0, 1.0, 0.0,"Step3");
	configParam(STEP4_PARAM, 0.0, 1.0, 0.0,"Step4");
	configParam(STEP5_PARAM, 0.0, 1.0, 0.0,"Step5");
	configParam(STEP6_PARAM, 0.0, 1.0, 0.0,"Step6");
	configParam(STEP7_PARAM, 0.0, 1.0, 0.0,"Step7");
	configParam(STEP8_PARAM, 0.0, 1.0, 0.0,"Step8");
	configParam(NUM_STEPS, 1.0, 8.0, 8.0,"Number of steps");
	configParam(LENGTH_PARAM, minLength, 1.0, 0.1,"Variation Length");
	configParam(LENGTHMOD_PARAM, -0.5f, 0.5f, 0.0f,"Variation Length Mod");
	configParam(SCALE_PARAM, 0.0, 2.0, 0.0,"Scale");
	configParam(SCALEINV_PARAM, 0.0, 1.0, 0.0,"Invert scale");
	configParam(SCALEON_PARAM, 0.0, 1.0, 0.0,"Scale active");
	configParam(OCTTRSP_PARAM, -2.0, 2.0, 0.0,"Variation octave offset");
	configParam(SEMITRSP_PARAM, 0.0, 11.0, 0.0,"Variation semitone offset");
	}

	
	void process(const ProcessArgs &args) override;

	int position=0;
    const float in_min[4] = {0.0, 0.0, 0.0, -5.0};
    const float in_max[4] = {8.0, 6.0, 10.0, 5.0};
	dsp::SchmittTrigger upTrigger, resetTrigger, gateTrigger, stepTriggers[8], stepATriggers[8];
	dsp::PulseGenerator on1;
	bool gate=false;

	
	void reset() {

		position = 0;
		for(int i=0; i<8; i++) lights[i].value = 0.0;
		gate=false;
	};

};


void PitchIntegrator::process(const ProcessArgs &args) {

	float out=0.0;
	
	float var=0.0;
	
	float gSampleTime = args.sampleTime;
	
	float outA=0.0;
	
	float length = clamp(params[LENGTH_PARAM].getValue() + ((inputs[VARMOD_INPUT].getVoltage() / 10.0f) * params[LENGTHMOD_PARAM].getValue()), 0.0, 1.0);

	int numSteps = round(clamp(params[NUM_STEPS].getValue(),1.0f,8.0f));
	if( inputs[NUMSTEPS_INPUT].isConnected() ) numSteps = round(clamp(inputs[NUMSTEPS_INPUT].getVoltage(),1.0f,8.0f));


		if( inputs[TRIGUP_INPUT].isConnected() ) {
			if (upTrigger.process(inputs[TRIGUP_INPUT].getVoltage()) ) position++;
		}

		if( inputs[RESET_INPUT].isConnected() ) {
			if (resetTrigger.process(inputs[RESET_INPUT].getVoltage()) ) position = 0;
		}

	


	for(int i=0; i<numSteps; i++) {
		if( stepTriggers[i].process(params[STEP1_PARAM+i].getValue())) position = i;

	};

	while( position < 0 )         position += numSteps;
	while( position >= numSteps ) position -= numSteps;

	out = inputs[IN1_INPUT+position].getNormalVoltage(0.0);
	
	float scaleout = (inputs[IN1_INPUT+position].getNormalVoltage(0.0));
	
	if (params[SCALEON_PARAM].getValue()) {
		
				if (params[SCALE_PARAM].getValue() == 2.0) {
			scaleout = ((inputs[IN1_INPUT+position].getNormalVoltage(0.0)) * 0.1);
		}
		
		if (params[SCALE_PARAM].getValue() == 1.0) {
			scaleout = ((inputs[IN1_INPUT+position].getNormalVoltage(0.0)) * 0.2);
		}

		if (params[SCALE_PARAM].getValue() == 0.0) {
			scaleout = ((inputs[IN1_INPUT+position].getNormalVoltage(0.0)) * 0.5);	
		}	
		
		if (params[SCALEINV_PARAM].getValue()) {
		scaleout = (scaleout * -1.0);
		}

	out = scaleout;

	}	
	
	outA = scaleout + params[OCTTRSP_PARAM].getValue() + (params[SEMITRSP_PARAM].getValue() / 12.0f);

	for(int i=0; i<8; i++) lights[i].value = (i==position)?1.0:0.0;

	outputs[OUT1_OUTPUT].setVoltage(out);
	
	if( inputs[VAR_INPUT].isConnected() ) {
	       
		if(gateTrigger.process(inputs[VAR_INPUT].getVoltage())) {
			gate = true;
		};
	
	if(gate) {			
		on1.trigger(length);
		gate = false;
	};
	
	var=(on1.process(gSampleTime) ? 10.0 : 0.0);
	
	};
	
	if (var){
		outputs[OUT1_OUTPUT].setVoltage(outA);
	}
};

struct PitchIntegratorWidget : ModuleWidget {
	PitchIntegratorWidget(PitchIntegrator *module) {
		setModule(module);

		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PitchIntegrator.svg")));


	addParam(createParam<LFMSnapKnob>(Vec(17.5f, 53 ), module, PitchIntegrator::NUM_STEPS));
	addParam(createParam<LFMTinyKnob>(Vec(113, 120 ), module, PitchIntegrator::LENGTH_PARAM));
	addParam(createParam<LFMTinyKnob>(Vec(113, 180 ), module, PitchIntegrator::LENGTHMOD_PARAM));
	addParam(createParam<LFMSwitch>(Vec(77,47), module, PitchIntegrator::SCALE_PARAM));
	addParam(createParam<MS>(Vec(113, 46), module, PitchIntegrator::SCALEINV_PARAM));
	addParam(createParam<MS>(Vec(113, 76), module, PitchIntegrator::SCALEON_PARAM));
	addParam(createParam<LFMTinySnapKnob>(Vec(113, 240), module, PitchIntegrator::OCTTRSP_PARAM));
	addParam(createParam<LFMTinySnapKnob>(Vec(113, 300), module, PitchIntegrator::SEMITRSP_PARAM));

	addInput(createInput<JackPort>(Vec(15, 170), module, PitchIntegrator::TRIGUP_INPUT));
	addInput(createInput<JackPort>(Vec(15, 125), module, PitchIntegrator::RESET_INPUT));
	addInput(createInput<JackPort>(Vec(15, 215), module, PitchIntegrator::VAR_INPUT));
	addInput(createInput<JackPort>(Vec(15, 260), module, PitchIntegrator::VARMOD_INPUT));

	const float offset_y = 125, delta_y=30;

	addInput(createInput<JackPort>(Vec(77, offset_y + 0*delta_y), module, PitchIntegrator::IN1_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 1*delta_y), module, PitchIntegrator::IN2_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 2*delta_y), module, PitchIntegrator::IN3_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 3*delta_y), module, PitchIntegrator::IN4_INPUT));

	addInput(createInput<JackPort>(Vec(77, offset_y + 4*delta_y), module, PitchIntegrator::IN5_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 5*delta_y), module, PitchIntegrator::IN6_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 6*delta_y), module, PitchIntegrator::IN7_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 7*delta_y), module, PitchIntegrator::IN8_INPUT));
	
	
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 0*delta_y), module, PitchIntegrator::STEP1_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 1*delta_y), module, PitchIntegrator::STEP2_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 2*delta_y), module, PitchIntegrator::STEP3_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 3*delta_y), module, PitchIntegrator::STEP4_PARAM));

	addParam(createParam<ButtonLED>(Vec(52, offset_y + 4*delta_y), module, PitchIntegrator::STEP5_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 5*delta_y), module, PitchIntegrator::STEP6_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 6*delta_y), module, PitchIntegrator::STEP7_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 7*delta_y), module, PitchIntegrator::STEP8_PARAM));

	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 0*delta_y), module, PitchIntegrator::STEP1_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 1*delta_y), module, PitchIntegrator::STEP2_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 2*delta_y), module, PitchIntegrator::STEP3_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 3*delta_y), module, PitchIntegrator::STEP4_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 4*delta_y), module, PitchIntegrator::STEP5_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 5*delta_y), module, PitchIntegrator::STEP6_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 6*delta_y), module, PitchIntegrator::STEP7_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 7*delta_y), module, PitchIntegrator::STEP8_LIGHT));

	addOutput(createOutput<OutJackPort>(Vec(15, 305), module, PitchIntegrator::OUT1_OUTPUT));

}

};


Model *modelPitchIntegrator = createModel<PitchIntegrator, PitchIntegratorWidget>("PitchIntegrator");