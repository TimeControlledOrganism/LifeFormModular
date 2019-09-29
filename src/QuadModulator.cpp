#include "plugin.hpp"

//This one is using the same formula as jw clock to generate phase values for sines
//https://github.com/jeremywen/JW-Modules
//Its also using a formula from HetrickCV to "sharpen" the sines
//https://github.com/mhetrick/hetrickcv
//Also bits of Fundamental Unity Mixer
//https://github.com/VCVRack/Fundamental


struct QuadModulator : Module {
	enum ParamId {
		RATE_PARAM,
		DIV_PARAM,
		SHAPEX_PARAM,
		SHAPEXMODAMP_PARAM,
		DIV2_PARAM,
		SHAPEX2_PARAM,
		SHAPEXMODAMP2_PARAM,
		DIV3_PARAM,
		SHAPEX3_PARAM,
		SHAPEXMODAMP3_PARAM,
		DIV4_PARAM,
		SHAPEX4_PARAM,
		SHAPEXMODAMP4_PARAM,
		MIXA1_PARAM,
		MIXA2_PARAM,
		MIXA3_PARAM,
		MIXA4_PARAM,
		MIXB1_PARAM,
		MIXB2_PARAM,
		MIXB3_PARAM,
		MIXB4_PARAM,
		MXA1_PARAM,
		MXA2_PARAM,
		MXA3_PARAM,
		MXA4_PARAM,
		MXB1_PARAM,
		MXB2_PARAM,
		MXB3_PARAM,
		MXB4_PARAM,
		EXP1_PARAM,
		EXP2_PARAM,
		EXP3_PARAM,
		EXP4_PARAM,
		
		NUM_PARAMS
	};
	enum InputId {
		RATE_INPUT,
		RESET_INPUT,
		SHAPEXMOD_INPUT,
		RESET2_INPUT,
		SHAPEXMOD2_INPUT,
		RESET3_INPUT,
		SHAPEXMOD3_INPUT,
		RESET4_INPUT,
		SHAPEXMOD4_INPUT,
		RESETG_INPUT,
		ALTSHTRIG_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		STEPLFO1_OUTPUT,
		STEPLFO2_OUTPUT,
		STEPLFO3_OUTPUT,
		STEPLFO4_OUTPUT,
		STEPLFO5_OUTPUT,
		STEPLFO6_OUTPUT,
		STEPLFO7_OUTPUT,
		STEPLFO8_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		MIXA1_LIGHT,
		MIXA2_LIGHT,
		MIXA3_LIGHT,
		MIXA4_LIGHT,
		MIXB1_LIGHT,
		MIXB2_LIGHT,
		MIXB3_LIGHT,
		MIXB4_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float phase2 = 0.0;
	float phase3 = 0.0;
	float phase4 = 0.0;
	
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger reset2Trigger;
	dsp::SchmittTrigger reset3Trigger;
	dsp::SchmittTrigger reset4Trigger;
	dsp::SchmittTrigger resetGTrigger;
	dsp::SchmittTrigger gateTrigger1;
	dsp::PulseGenerator delay1;
	dsp::PulseGenerator on1;
	dsp::SchmittTrigger AltSHTrigger;
	dsp::SchmittTrigger gateTrigger2;
	dsp::PulseGenerator delay2;
	dsp::PulseGenerator on2;
	
	float lfo1 = 0.0;
	float lfo2 = 0.0;
	float lfo3 = 0.0;
	float lfo4 = 0.0;
	
	float sample1 = 0.0;
	float sample2 = 0.0;
	float sample3 = 0.0;
	float sample4 = 0.0;
	float sample5 = 0.0;
	float sample6 = 0.0;
	float sample7 = 0.0;
	float sample8 = 0.0;
	
	
	bool gate1=false;
	bool gate2=false;
	
	float mix = 0.0;
	float mix2 = 0.0;
	
	QuadModulator() {

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);


		configParam(RATE_PARAM, -2.f, 6.f, 2.f, "Clock Rate");
		configParam(DIV_PARAM, 0.0, 9.0, 0.0, "LFO1 Clock Division");
		configParam(SHAPEX_PARAM, 0.0, 1.0, 0.5,"LFO1 Shape");
		configParam(SHAPEXMODAMP_PARAM, -1.0, 1.0, 0.0,"LFO1 Shape Mod");
		configParam(DIV2_PARAM, 0.0, 9.0, 0.0, "LFO2 Clock Division");
		configParam(SHAPEX2_PARAM, 0.0, 1.0, 0.5,"LFO2 Shape");
		configParam(SHAPEXMODAMP2_PARAM, -1.0, 1.0, 0.0,"LFO2 Shape Mod");
		configParam(DIV3_PARAM, 0.0, 9.0, 0.0,"LFO3 Clock Division");
		configParam(SHAPEX3_PARAM, 0.0, 1.0, 0.5,"LFO3 Shape");
		configParam(SHAPEXMODAMP3_PARAM, -1.0, 1.0, 0.0,"LFO3 Shape Mod");
		configParam(DIV4_PARAM, 0.0, 9.0, 0.0,"LFO4 Clock Division");
		configParam(SHAPEX4_PARAM, 0.0, 1.0, 0.5,"LFO4 Shape");
		configParam(SHAPEXMODAMP4_PARAM, -1.0, 1.0, 0.0,"LFO4 Shape Mod");
		configParam(MIXA1_PARAM, 0.0, 1.0, 0.0,"MIX1 LFO1 On");
		configParam(MIXA2_PARAM, 0.0, 1.0, 0.0,"MIX1 LFO2 On");
		configParam(MIXA3_PARAM, 0.0, 1.0, 0.0,"MIX1 LFO3 On");
		configParam(MIXA4_PARAM, 0.0, 1.0, 0.0,"MIX1 LFO4 On");
		configParam(MIXB1_PARAM, 0.0, 1.0, 0.0,"MIX2 LFO1 On");
		configParam(MIXB2_PARAM, 0.0, 1.0, 0.0, "MIX2 LFO2 On");
		configParam(MIXB3_PARAM, 0.0, 1.0, 0.0, "MIX2 LFO3 On");
		configParam(MIXB4_PARAM, 0.0, 1.0, 0.0, "MIX2 LFO4 On");
		configParam(EXP1_PARAM, 0.0, 1.0, 0.0,"LFO1 Exp Shape");
		configParam(EXP2_PARAM, 0.0, 1.0, 0.0,"LFO2 Exp Shape");
		configParam(EXP3_PARAM, 0.0, 1.0, 0.0,"LFO3 Exp Shape");
		configParam(EXP4_PARAM, 0.0, 1.0, 0.0,"LFO4 Exp Shape");
	}
	
	void resetClock() {
		phase = 0.0;
	}

	void resetClock2() {
		phase2 = 0.0;
	}
	
	void resetClock3() {
		phase3 = 0.0;
	}
	
	void resetClock4() {
		phase4 = 0.0;
	}
	
	void resetClockG() {
		phase = 0.0;
		phase2 = 0.0;
		phase3 = 0.0;
		phase4 = 0.0;
	}
	
	void process(const ProcessArgs &args) override {
		
		float gSampleTime = args.sampleTime;
		float delayTime1 = 0.005;

	if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())){
		resetClock();
	}
	
	if (reset2Trigger.process(inputs[RESET2_INPUT].getVoltage())){
		resetClock2();
	}
	
	if (reset3Trigger.process(inputs[RESET3_INPUT].getVoltage())){
		resetClock3();
	}
	
	if (reset4Trigger.process(inputs[RESET4_INPUT].getVoltage())){
		resetClock4();
	}
	
	if (resetGTrigger.process(inputs[RESETG_INPUT].getVoltage())){
		delay1.trigger(delayTime1);
		gate1 = true;
		}
		if(  gate1 && !delay1.process(gSampleTime) ) {	
		on1.trigger(0.01);
		gate1 = false;
	};
	
	float dclock = (on1.process(gSampleTime) ? 10.0 : 0.0);
	
if (dclock){	
sample1 = lfo1;
sample2 = lfo2;
sample3 = lfo3;
sample4 = lfo4;
sample5 = mix;
sample6 = mix2;
}

	if (AltSHTrigger.process(inputs[ALTSHTRIG_INPUT].getVoltage())){
		delay2.trigger(delayTime1);
		gate2 = true;
		}
		if(  gate2 && !delay2.process(gSampleTime) ) {	
		on2.trigger(0.01);
		gate2 = false;
	};

	float altdclock = (on2.process(gSampleTime) ? 10.0 : 0.0);

	
if (altdclock){	
sample7 = mix;
sample8 = mix2;
}

	
	float shape = -3.0;
	shape *= 0.99f;
	const float shapeB = (1.0 - shape) / (1.0 + shape);
	const float shapeA = (4.0 * shape) / ((1.0 - shape) * (1.0 + shape));
	
	float clockTime = powf(2.0, (params[RATE_PARAM].getValue()) - (params[DIV_PARAM].getValue()));	float clockTime2 = powf(2.0, (params[RATE_PARAM].getValue()) - (params[DIV2_PARAM].getValue()));
	float clockTime3 = powf(2.0, (params[RATE_PARAM].getValue()) - (params[DIV3_PARAM].getValue()));
	float clockTime4 = powf(2.0, (params[RATE_PARAM].getValue()) - (params[DIV4_PARAM].getValue()));
	
	
	if (inputs[RATE_INPUT].isConnected()) {
		clockTime = powf(2.0, (inputs[RATE_INPUT].getVoltage()) - (params[DIV_PARAM].getValue()));
		clockTime2 = powf(2.0, (inputs[RATE_INPUT].getVoltage()) - (params[DIV2_PARAM].getValue()));
		clockTime3 = powf(2.0, (inputs[RATE_INPUT].getVoltage()) - (params[DIV3_PARAM].getValue()));
		clockTime4 = powf(2.0, (inputs[RATE_INPUT].getVoltage()) - (params[DIV4_PARAM].getValue()));
	}
		
	phase += clockTime / args.sampleRate;
		if (phase >= 1.0) {
			phase -= 1.0;
		}
						
	float d = clamp((params[SHAPEX_PARAM].getValue() + ((inputs[SHAPEXMOD_INPUT].getVoltage() / 10.0f) *(params[SHAPEXMODAMP_PARAM].getValue()))), 0.0, 1.0);

float sawwave = (1.0-0.5)*((phase-d)/(1.0-d)) + 0.5 ;

	if (outputs[OUT1_OUTPUT].isConnected() || params[MIXA1_PARAM].getValue() == 1.0 || params[MIXB1_PARAM].getValue() == 1.0 || outputs[STEPLFO1_OUTPUT].isConnected()){
	
	if (phase < d ){
  sawwave = (0.5 * phase) / d;
	}	

float saw = sinf((1.0f * M_PI * sawwave));

float input = saw;
float output = input * (shapeA + shapeB);
	output = output / ((std::abs(input) * shapeA) + shapeB);
	
	lfo1 = (10.0f * saw);

if (params[EXP1_PARAM].getValue() == 1.0){
		lfo1 = (10.0f * output);
}
		
outputs[OUT1_OUTPUT].setVoltage(lfo1);
				
	}
		
	phase2 += clockTime2 / args.sampleRate;
		if (phase2 >= 1.0) {
			phase2 -= 1.0;
		}
		
	float d2 = clamp((params[SHAPEX2_PARAM].getValue() + ((inputs[SHAPEXMOD2_INPUT].getVoltage() / 10.0f) *(params[SHAPEXMODAMP2_PARAM].getValue()))), 0.0, 1.0);

float sawwave2 = (1.0-0.5)*((phase2-d2)/(1.0-d2)) + 0.5 ;

	if (outputs[OUT2_OUTPUT].isConnected() || params[MIXA2_PARAM].getValue() == 1.0 || params[MIXB2_PARAM].getValue() == 1.0 || outputs[STEPLFO2_OUTPUT].isConnected() ){
	
	if (phase2 < d2 ){
  sawwave2 = (0.5 * phase2) / d2;
	}	

float saw2 = sinf((1.0f * M_PI * sawwave2));

float input2 = saw2;
float output2 = input2 * (shapeA + shapeB);
	output2 = output2 / ((std::abs(input2) * shapeA) + shapeB);
	
	lfo2 = (10.0f * saw2);


if (params[EXP2_PARAM].getValue() == 1.0){
		lfo2 = (10.0f * output2);
}

outputs[OUT2_OUTPUT].setVoltage(lfo2);

	}
	
	phase3 += clockTime3 / args.sampleRate;
		if (phase3 >= 1.0) {
			phase3 -= 1.0;
		}
		
	float d3 = clamp((params[SHAPEX3_PARAM].getValue() + ((inputs[SHAPEXMOD3_INPUT].getVoltage() / 10.0f) *(params[SHAPEXMODAMP3_PARAM].getValue()))), 0.0, 1.0);

float sawwave3 = (1.0-0.5)*((phase3-d3)/(1.0-d3)) + 0.5 ;

	if (outputs[OUT3_OUTPUT].isConnected() || params[MIXA3_PARAM].getValue() == 1.0 || params[MIXB3_PARAM].getValue() == 1.0 || outputs[STEPLFO3_OUTPUT].isConnected()){
	
	if (phase3 < d3 ){
  sawwave3 = (0.5 * phase3) / d3;
	}	

float saw3 = sinf((1.0f * M_PI * sawwave3));
	
	lfo3 = (10.0f * saw3);


float input3 = saw3;
float output3 = input3 * (shapeA + shapeB);
	output3 = output3 / ((std::abs(input3) * shapeA) + shapeB);

if (params[EXP3_PARAM].getValue() == 1.0){
		lfo3 = (10.0f * output3);
}

outputs[OUT3_OUTPUT].setVoltage(lfo3);
			
	}
	
	phase4 += clockTime4 / args.sampleRate;
		if (phase4 >= 1.0) {
			phase4 -= 1.0;
		}
		
	float d4 = clamp((params[SHAPEX4_PARAM].getValue() + ((inputs[SHAPEXMOD4_INPUT].getVoltage() / 10.0f) *(params[SHAPEXMODAMP4_PARAM].getValue()))), 0.0, 1.0);

float sawwave4 = (1.0-0.5)*((phase4-d4)/(1.0-d4)) + 0.5 ;

	if (outputs[OUT4_OUTPUT].isConnected() || params[MIXA4_PARAM].getValue() == 1.0 || params[MIXB4_PARAM].getValue() == 1.0 || outputs[STEPLFO4_OUTPUT].isConnected()){
	
	if (phase4 < d4 ){
  sawwave4 = (0.5 * phase4) / d4;
	}	

float saw4 = sinf((1.0f * M_PI * sawwave4));
	
	lfo4 = (10.0f * saw4);


float input4 = saw4;
float output4 = input4 * (shapeA + shapeB);
	output4 = output4 / ((std::abs(input4) * shapeA) + shapeB);


if (params[EXP4_PARAM].getValue() == 1.0){
		lfo4 = (10.0f * output4);
}

outputs[OUT4_OUTPUT].setVoltage(lfo4);

		
	}
	
		lights[MIXA1_LIGHT].value = (params[MIXA1_PARAM].getValue());
		lights[MIXA2_LIGHT].value = (params[MIXA2_PARAM].getValue());
		lights[MIXA3_LIGHT].value = (params[MIXA3_PARAM].getValue());
		lights[MIXA4_LIGHT].value = (params[MIXA4_PARAM].getValue());
		lights[MIXB1_LIGHT].value = (params[MIXB1_PARAM].getValue());
		lights[MIXB2_LIGHT].value = (params[MIXB2_PARAM].getValue());
		lights[MIXB3_LIGHT].value = (params[MIXB3_PARAM].getValue());
		lights[MIXB4_LIGHT].value = (params[MIXB4_PARAM].getValue());
	

	if (params[MIXA1_PARAM].getValue()== 1.0 || params[MIXA2_PARAM].getValue() == 1.0 || params[MIXA3_PARAM].getValue() == 1.0 || params[MIXA4_PARAM].getValue() == 1.0){
	mix = ((lfo1) * params[MIXA1_PARAM].getValue()) + ((lfo2)  * params[MIXA2_PARAM].getValue()) + ((lfo3) * params[MIXA3_PARAM].getValue()) + ((lfo4) * params[MIXA4_PARAM].getValue());
			mix /= (params[MIXA1_PARAM].getValue()) + (params[MIXA2_PARAM].getValue()) + (params[MIXA3_PARAM].getValue()) + (params[MIXA4_PARAM].getValue());
			}
			outputs[OUT5_OUTPUT].setVoltage(mix);
			
	
		if (params[MIXB1_PARAM].getValue()== 1.0 || params[MIXB2_PARAM].getValue() == 1.0 || params[MIXB3_PARAM].getValue() == 1.0 || params[MIXB4_PARAM].getValue() == 1.0){
	mix2 = ((lfo1 ) * params[MIXB1_PARAM].getValue()) + ((lfo2 )  * params[MIXB2_PARAM].getValue()) + ((lfo3) * params[MIXB3_PARAM].getValue()) + ((lfo4 ) * params[MIXB4_PARAM].getValue());
			mix2 /= (params[MIXB1_PARAM].getValue()) + (params[MIXB2_PARAM].getValue()) + (params[MIXB3_PARAM].getValue()) + (params[MIXB4_PARAM].getValue());	
			}
			outputs[OUT6_OUTPUT].setVoltage(mix2);
	
	
	outputs[STEPLFO1_OUTPUT].setVoltage(sample1);
	outputs[STEPLFO2_OUTPUT].setVoltage(sample2);
	outputs[STEPLFO3_OUTPUT].setVoltage(sample3);
	outputs[STEPLFO4_OUTPUT].setVoltage(sample4);
	outputs[STEPLFO5_OUTPUT].setVoltage(sample5);
	outputs[STEPLFO6_OUTPUT].setVoltage(sample6);
	outputs[STEPLFO7_OUTPUT].setVoltage(sample7);
	outputs[STEPLFO8_OUTPUT].setVoltage(sample8);
	}

};


struct QuadModulatorWidget : ModuleWidget {
	QuadModulatorWidget(QuadModulator *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/QuadModulator.svg")));

		addParam(createParam<LFMSnapKnob>(Vec(10,  110), module, QuadModulator::DIV_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(28, 190), module, QuadModulator::SHAPEX_PARAM));
		addParam(createParam<MS>(Vec(8, 170), module, QuadModulator::EXP1_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(28, 245), module, QuadModulator::SHAPEXMODAMP_PARAM));
		addInput(createInput<MiniJackPort>(Vec(8, 230), module, QuadModulator::SHAPEXMOD_INPUT));
				
		addParam(createParam<LFMSnapKnob>(Vec(70,  110), module, QuadModulator::DIV2_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(88, 190), module, QuadModulator::SHAPEX2_PARAM));
		addParam(createParam<MS>(Vec(68, 170), module, QuadModulator::EXP2_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(88, 245), module, QuadModulator::SHAPEXMODAMP2_PARAM));
		addInput(createInput<MiniJackPort>(Vec(68, 230), module, QuadModulator::SHAPEXMOD2_INPUT));
		
		addParam(createParam<LFMSnapKnob>(Vec(130,  110), module, QuadModulator::DIV3_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(148, 190), module, QuadModulator::SHAPEX3_PARAM));
		addParam(createParam<MS>(Vec(128, 170), module, QuadModulator::EXP3_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(148, 245), module, QuadModulator::SHAPEXMODAMP3_PARAM));
		addInput(createInput<MiniJackPort>(Vec(128, 230), module, QuadModulator::SHAPEXMOD3_INPUT));
		
		addParam(createParam<LFMSnapKnob>(Vec(190,  110), module, QuadModulator::DIV4_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(208, 190), module, QuadModulator::SHAPEX4_PARAM));
		addParam(createParam<MS>(Vec(188, 170), module, QuadModulator::EXP4_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(208, 245), module, QuadModulator::SHAPEXMODAMP4_PARAM));
		addInput(createInput<MiniJackPort>(Vec(188, 230), module, QuadModulator::SHAPEXMOD4_INPUT));
		
		addParam(createParam<LFMTinyKnob>(Vec(78,  25), module, QuadModulator::RATE_PARAM));
		addInput(createInput<InJackPort>(Vec(17, 25), module, QuadModulator::RATE_INPUT));
		addInput(createInput<JackPort>(Vec(138, 25), module, QuadModulator::RESETG_INPUT));
		addInput(createInput<JackPort>(Vec(18, 60), module, QuadModulator::RESET_INPUT));
		addInput(createInput<JackPort>(Vec(78, 60), module, QuadModulator::RESET2_INPUT));
		addInput(createInput<JackPort>(Vec(138, 60), module, QuadModulator::RESET3_INPUT));
		addInput(createInput<JackPort>(Vec(198, 60), module, QuadModulator::RESET4_INPUT));
		
		addParam(createParam<ButtonLEDLatch>(Vec(250,  30), module, QuadModulator::MIXA1_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(251.4, 31.4), module, QuadModulator::MIXA1_LIGHT));
		addParam(createParam<ButtonLEDLatch>(Vec(250,  70), module, QuadModulator::MIXA2_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(251.4, 71.4), module, QuadModulator::MIXA2_LIGHT));
		addParam(createParam<ButtonLEDLatch>(Vec(250,  110), module, QuadModulator::MIXA3_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(251.4, 111.4), module, QuadModulator::MIXA3_LIGHT));
		addParam(createParam<ButtonLEDLatch>(Vec(250,  150), module, QuadModulator::MIXA4_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(251.4, 151.4), module, QuadModulator::MIXA4_LIGHT));
		addParam(createParam<ButtonLEDLatch>(Vec(295,  30), module, QuadModulator::MIXB1_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(296.4, 31.4), module, QuadModulator::MIXB1_LIGHT));
		addParam(createParam<ButtonLEDLatch>(Vec(295,  70), module, QuadModulator::MIXB2_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(296.4, 71.4), module, QuadModulator::MIXB2_LIGHT));
		addParam(createParam<ButtonLEDLatch>(Vec(295,  110), module, QuadModulator::MIXB3_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(296.4, 111.4), module, QuadModulator::MIXB3_LIGHT));
		addParam(createParam<ButtonLEDLatch>(Vec(295,  150), module, QuadModulator::MIXB4_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(296.4, 151.4), module, QuadModulator::MIXB4_LIGHT));
		
		
		addInput(createInput<JackPort>(Vec(248, 283), module, QuadModulator::ALTSHTRIG_INPUT));
		
		addOutput(createOutput<OutJackPort>(Vec(3, 290), module, QuadModulator::STEPLFO1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(33, 300), module, QuadModulator::OUT1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(63, 290), module, QuadModulator::STEPLFO2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(93, 300), module, QuadModulator::OUT2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(123, 290), module, QuadModulator::STEPLFO3_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(153, 300), module, QuadModulator::OUT3_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(183, 290), module, QuadModulator::STEPLFO4_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(213, 300), module, QuadModulator::OUT4_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(248, 190), module, QuadModulator::STEPLFO5_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(248, 235), module, QuadModulator::OUT5_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(293, 190), module, QuadModulator::STEPLFO6_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(293, 235), module, QuadModulator::OUT6_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(248, 310), module, QuadModulator::STEPLFO7_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(293, 310), module, QuadModulator::STEPLFO8_OUTPUT));
	}
};

Model *modelQuadModulator = createModel<QuadModulator, QuadModulatorWidget>("QuadModulator");
