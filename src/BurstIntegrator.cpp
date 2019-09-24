#include "plugin.hpp"

#define minLength 0.01f

//Based on ML Switches
////https://github.com/martin-lueders/ML_modules
//Added sample and hold and clock generator to add "bursts" to trigger patterns


struct BurstIntegrator : Module {
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
		BST1CLK_PARAM,
		RATE_PARAM,
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
		CVIN_INPUT,
		BRESET_INPUT,
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

	BurstIntegrator() {
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
	configParam(BST1CLK_PARAM, 0.0, 2.0, 1.0,"Burst Rate");
	configParam(RATE_PARAM, -2.f, 6.f, 2.f, "Clock Rate");
	}


	

	void process(const ProcessArgs &args) override;

	int position=0;

        const float in_min[4] = {0.0, 0.0, 0.0, -5.0};
        const float in_max[4] = {8.0, 6.0, 10.0, 5.0};

	dsp::SchmittTrigger upTrigger, resetTrigger, gateTrigger, stepTriggers[8], stepATriggers[8];
	dsp::SchmittTrigger BresetTrigger;
	dsp::PulseGenerator on1;
	dsp::PulseGenerator gatePulse1;
	dsp::PulseGenerator gatePulse2;
	dsp::PulseGenerator gatePulse3;
	dsp::PulseGenerator BresetPulse;
	float phase1 = 0.0f;
	float phase2 = 0.0f;
	float phase3 = 0.0f;
	int stepCount1 = 0;
	int stepCount2 = 0;
	int stepCount3 = 0;
	
	bool gate=false;

	void resetClock() {
		phase1 = 0.0;
		phase2 = 0.0;
		phase3 = 0.0;
		BresetPulse.trigger(0.001);
		stepCount1 = 0;
		stepCount2 = 0;
		stepCount3 = 0;
		}


	void reset() {

		position = 0;
		for(int i=0; i<8; i++) lights[i].value = 0.0;
		gate=false;
	};

};


void BurstIntegrator::process(const ProcessArgs &args) {

	float out=0.0;
	
	float var=0.0;
	
	float gSampleTime = args.sampleTime;
	
	if (BresetTrigger.process(inputs[BRESET_INPUT].getVoltage())){
		resetClock();
		}	
	
	float outA=0.0;

	float length = clamp(params[LENGTH_PARAM].getValue() + ((inputs[VARMOD_INPUT].getVoltage() / 10.0f) * params[LENGTHMOD_PARAM].getValue()), 0.0, 1.0);

	int numSteps = round(clamp(params[NUM_STEPS].getValue(),1.0f,8.0f));
	if( inputs[NUMSTEPS_INPUT].isConnected() ) numSteps = round(clamp(inputs[NUMSTEPS_INPUT].getVoltage(),1.0f,8.0f));

 	if( inputs[TRIGUP_INPUT].isConnected() ) {
			if (upTrigger.process(inputs[TRIGUP_INPUT].getVoltage()) ) position++;
	}

	if( inputs[RESET_INPUT].isConnected() ) {
			if (resetTrigger.process(inputs[RESET_INPUT].getVoltage()) ) 
				position = 0;
	}

	for(int i=0; i<numSteps; i++) {
		if( stepTriggers[i].process(params[STEP1_PARAM+i].getValue())) position = i;

	};

	while( position < 0 )         position += numSteps;
	while( position >= numSteps ) position -= numSteps;

	out = inputs[IN1_INPUT+position].getVoltage(0.0);
	
	bool nextStep1 = false;
	bool nextStep2 = false;
	bool nextStep3 = false;
	
	float clockTime1 = powf(2.0, (params[RATE_PARAM].getValue() + 1.0));	
	float clockTime2 = powf(2.0, (params[RATE_PARAM].getValue() + 1.5));	
	float clockTime3 = powf(2.0, (params[RATE_PARAM].getValue() + 2.0));	
	
	
	if (inputs[CVIN_INPUT].isConnected()) {
clockTime1 = powf(2.0, (inputs[CVIN_INPUT].getVoltage()+ 1.0));			
clockTime2 = powf(2.0, (inputs[CVIN_INPUT].getVoltage()+ 1.5));		
clockTime3 = powf(2.0, (inputs[CVIN_INPUT].getVoltage() + 2.0));	
	}
	
		phase1 += clockTime1 / args.sampleRate;
		phase2 += clockTime2 / args.sampleRate;
		phase3 += clockTime3 / args.sampleRate;
		
		if (phase1 >= 1.0) {
			phase1 -= 1.0;
			nextStep1 = true;
		}
		
		if (phase2 >= 1.0) {
			phase2 -= 1.0;
			nextStep2 = true;
		}
		
		if (phase3 >= 1.0) {
			phase3 -= 1.0;
			nextStep3 = true;
		}
		
		if (nextStep1) {
		stepCount1 = (stepCount1 + 1);
		gatePulse1.trigger(0.01);
	}
	
	if (nextStep2) {
		stepCount2 = (stepCount2 + 1);
		gatePulse2.trigger(0.01);
	}
	
	if (nextStep3) {
		stepCount3 = (stepCount3 + 1);
		gatePulse3.trigger(0.01);
	}
	
	bool rpulse = BresetPulse.process(1.0 / args.sampleRate);
	bool gpulse1 = gatePulse1.process(1.0 / args.sampleRate);
	bool gpulse2 = gatePulse2.process(1.0 / args.sampleRate);
	bool gpulse3 = gatePulse3.process(1.0 / args.sampleRate);
	
	float bst32 = (gpulse1 && (stepCount1 % 1 == 0) ? 10.0 : 0.0 + rpulse ? 10.0 : 0.0);
	float bst64 = (gpulse2 && (stepCount2 % 1 == 0) ? 10.0 : 0.0 + rpulse ? 10.0 : 0.0);
	float bst128 = (gpulse3 && (stepCount3 % 1 == 0) ? 10.0 : 0.0 + rpulse ? 10.0 : 0.0);
	
	if ((params[BST1CLK_PARAM].getValue()) == 2.0) {
	outA = (bst32);
	}
	if ((params[BST1CLK_PARAM].getValue()) == 1.0) {
	outA = (bst64);
	}
		if ((params[BST1CLK_PARAM].getValue()) == 0.0) {
	outA = (bst128);
	}
	
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

struct BurstIntegratorWidget : ModuleWidget {
	BurstIntegratorWidget(BurstIntegrator *module) {
		setModule(module);

		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BurstIntegrator.svg")));

	addParam(createParam<LFMSnapKnob>(Vec(17.5f, 53 ), module, BurstIntegrator::NUM_STEPS));
	addParam(createParam<LFMTinyKnob>(Vec(113, 185 ), module, BurstIntegrator::LENGTH_PARAM));
	addParam(createParam<LFMTinyKnob>(Vec(113, 240 ), module, BurstIntegrator::LENGTHMOD_PARAM));
	
	addInput(createInput<JackPort>(Vec(15, 170), module, BurstIntegrator::TRIGUP_INPUT));
	addInput(createInput<JackPort>(Vec(15, 125), module, BurstIntegrator::RESET_INPUT));
	addInput(createInput<JackPort>(Vec(15, 215), module, BurstIntegrator::VAR_INPUT));
	addInput(createInput<JackPort>(Vec(15, 260), module, BurstIntegrator::VARMOD_INPUT));
	addInput(createInput<JackPort>(Vec(75, 80), module, BurstIntegrator::BRESET_INPUT));
	
	addParam(createParam<LFMTinyKnob>(Vec(113,130), module, BurstIntegrator::RATE_PARAM));
	addInput(createInput<InJackPort>(Vec(113, 80), module, BurstIntegrator::CVIN_INPUT));
	addParam(createParam<LFMSwitch>(Vec(107,47), module, BurstIntegrator::BST1CLK_PARAM));

	const float offset_y = 125, delta_y=30;

	addInput(createInput<JackPort>(Vec(77, offset_y + 0*delta_y), module, BurstIntegrator::IN1_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 1*delta_y), module, BurstIntegrator::IN2_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 2*delta_y), module, BurstIntegrator::IN3_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 3*delta_y), module, BurstIntegrator::IN4_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 4*delta_y), module, BurstIntegrator::IN5_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 5*delta_y), module, BurstIntegrator::IN6_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 6*delta_y), module, BurstIntegrator::IN7_INPUT));
	addInput(createInput<JackPort>(Vec(77, offset_y + 7*delta_y), module, BurstIntegrator::IN8_INPUT));
	
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 0*delta_y), module, BurstIntegrator::STEP1_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 1*delta_y), module, BurstIntegrator::STEP2_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 2*delta_y), module, BurstIntegrator::STEP3_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 3*delta_y), module, BurstIntegrator::STEP4_PARAM));

	addParam(createParam<ButtonLED>(Vec(52, offset_y + 4*delta_y), module, BurstIntegrator::STEP5_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 5*delta_y), module, BurstIntegrator::STEP6_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 6*delta_y), module, BurstIntegrator::STEP7_PARAM));
	addParam(createParam<ButtonLED>(Vec(52, offset_y + 7*delta_y), module, BurstIntegrator::STEP8_PARAM));

	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 0*delta_y), module, BurstIntegrator::STEP1_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 1*delta_y), module, BurstIntegrator::STEP2_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 2*delta_y), module, BurstIntegrator::STEP3_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 3*delta_y), module, BurstIntegrator::STEP4_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 4*delta_y), module, BurstIntegrator::STEP5_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 5*delta_y), module, BurstIntegrator::STEP6_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 6*delta_y), module, BurstIntegrator::STEP7_LIGHT));
	addChild(createLight<LargeLight<GreenLight>>(Vec(53.4f, offset_y + 1.4 + 7*delta_y), module, BurstIntegrator::STEP8_LIGHT));

	addOutput(createOutput<OutJackPort>(Vec(15, 307), module, BurstIntegrator::OUT1_OUTPUT));

}

};


Model *modelBurstIntegrator = createModel<BurstIntegrator, BurstIntegratorWidget>("BurstIntegrator");
