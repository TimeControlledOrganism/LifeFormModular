#include "plugin.hpp"
//Based On Fundamental Seq3
//2*8 step sequencers with 2 trigger lines each
//added probabilty outs based on Audible bernouilli gate

struct SequenceModeler : Module {
	enum ParamId {
		CLOCK1_PARAM,
		CLOCK2_PARAM,
		RUN_PARAM,
		RESET1_PARAM,
		RESET2_PARAM,
		STEPS1_PARAM,
		STEPS2_PARAM,
		THRESHOLD1_PARAM,
		DMOD1_PARAM,
		DMOD2_PARAM,
		THRESHOLD2_PARAM,
		ENUMS(ROW1_PARAM, 8),
		ENUMS(ROW2_PARAM, 8),
		ENUMS(GATE1_PARAM, 8),
		ENUMS(GATE2_PARAM, 8),
		ENUMS(GATE3_PARAM, 8),
		ENUMS(GATE4_PARAM, 8),
		SCALE_PARAM,
		SCALEINV_PARAM,
		SCALE2_PARAM,
		SCALE2INV_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		EXT_CLOCK1_INPUT,
		EXT_CLOCK2_INPUT,
		RESET1_INPUT,
		RESET2_INPUT,
		RESET3_INPUT,
		RESET4_INPUT,
		DMOD1_INPUT,
		DMOD2_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		GATE1_OUTPUT,
		GATE1A_OUTPUT,
		GATE1B_OUTPUT,
		GATE2_OUTPUT,
		GATE2A_OUTPUT,
		GATE3_OUTPUT,
		GATE3A_OUTPUT,
		GATE3B_OUTPUT,
		GATE4_OUTPUT,
		GATE4A_OUTPUT,
		ROW1_OUTPUT,
		ROW2_OUTPUT,
		SCALE1_OUTPUT,
		SCALE2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		RUNNING_LIGHT,
		RESET1_LIGHT,
		RESET2_LIGHT,
		ENUMS(GATE1_LIGHTS, 16),
		ENUMS(GATE2_LIGHTS, 16),
		ENUMS(GATE3_LIGHTS, 16),
		ENUMS(GATE4_LIGHTS, 16),
		NUM_LIGHTS
	};

	bool running = true;
	dsp::SchmittTrigger clockTrigger1;
	dsp::SchmittTrigger clockTrigger2;
	dsp::SchmittTrigger runningTrigger;
	dsp::SchmittTrigger reset1Trigger;
	dsp::SchmittTrigger reset2Trigger;
	dsp::SchmittTrigger gate1Triggers[8];
	dsp::SchmittTrigger gate1ATriggers;
	dsp::SchmittTrigger gate1BTriggers;
	dsp::SchmittTrigger gate2Triggers[8];
	dsp::SchmittTrigger gate2ATriggers;
	dsp::SchmittTrigger gate3Triggers[8];
	dsp::SchmittTrigger gate3ATriggers;
	dsp::SchmittTrigger gate3BTriggers;
	dsp::SchmittTrigger gate4Triggers[8];
	dsp::SchmittTrigger gate4ATriggers;

	int index1 = 0;
	int index2 = 0;
	bool gates1[8] = {};
	bool outcomes1 = false;
	bool outcomes1b = false;
	bool gates2[8] = {};
	bool outcomes2 = false;
	bool gates3[8] = {};
	bool outcomes3 = false;
	bool outcomes3b = false;
	bool gates4[8] = {};
	bool outcomes4 = false;
	
	SequenceModeler() {


		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RUN_PARAM, 0.f, 1.f, 0.f, "Run");
		configParam(RESET1_PARAM, 0.f, 1.f, 0.f,"Reset Seq1");
		configParam(RESET2_PARAM, 0.f, 1.f, 0.f,"Reset Seq2");
		configParam(STEPS1_PARAM, 1.f, 8.f, 8.f,"Steps Seq1");
		configParam(STEPS2_PARAM, 1.f, 8.f, 8.f,"Steps Seq2");
		configParam(THRESHOLD1_PARAM, 0.f, 1.f, 1.0f,"Prob Trig1 seq1");
		configParam(DMOD1_PARAM, -1.f, 1.f, 0.0f,"Prob Mod Trig1 seq1");
		configParam(THRESHOLD2_PARAM, 0.f, 1.f, 1.f,"Prob Trig1 seq2");
		configParam(DMOD2_PARAM, -1.f, 1.f, 0.0f,"Prob Mod Trig1 seq2");
		configParam(SCALE_PARAM, 0.0, 2.0, 0.0,"Scale Seq1");
		configParam(SCALEINV_PARAM, 0.0, 1.0, 0.0,"Invert Seq1");
		configParam(SCALE2_PARAM, 0.0, 2.0, 0.0,"Scale Seq2");
		configParam(SCALE2INV_PARAM, 0.0, 1.0, 0.0,"Invert Seq2");
		for (int i = 0; i < 8; i++) {
			configParam(ROW1_PARAM + i, 0.f, 10.f, 0.f,"Pitch Seq1");
			configParam(ROW2_PARAM + i, 0.f, 10.f, 0.f,"Pitch Seq2");
			configParam(GATE1_PARAM + i, 0.f, 1.f, 0.f,"Trig 1 Seq1");
			configParam(GATE2_PARAM + i, 0.f, 1.f, 0.f,"Trig 2 Seq1");
			configParam(GATE3_PARAM + i, 0.f, 1.f, 0.f,"Trig 1 Seq2");
			configParam(GATE4_PARAM + i, 0.f, 1.f, 0.f,"Trig 2 Seq2");
		}
		
	}

	void onReset() override {
		for (int i = 0; i < 8; i++) {
			gates1[i] = true;
			gates2[i] = true;
			gates3[i] = true;
			gates4[i] = true;
		}
	}

	void onRandomize() override {
		for (int i = 0; i < 8; i++) {
			gates1[i] = (random::uniform() > 0.5f);
			gates2[i] = (random::uniform() > 0.5f);
			gates3[i] = (random::uniform() > 0.5f);
			gates4[i] = (random::uniform() > 0.5f);
		}
	}
			
		json_t *dataToJson() override {
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "running", json_boolean(running));

		json_t *gates1J = json_array();
		for (int i = 0; i < 8; i++) {
			json_array_insert_new(gates1J, i, json_integer((int) gates1[i]));
		}
		json_object_set_new(rootJ, "gates1", gates1J);
		
		json_t *gates2J = json_array();
		for (int i = 0; i < 8; i++) {
			json_array_insert_new(gates2J, i, json_integer((int) gates2[i]));
		}
		json_object_set_new(rootJ, "gates2", gates2J);
		
		json_t *gates3J = json_array();
		for (int i = 0; i < 8; i++) {
			json_array_insert_new(gates3J, i, json_integer((int) gates3[i]));
		}
		json_object_set_new(rootJ, "gates3", gates3J);
		
		json_t *gates4J = json_array();
		for (int i = 0; i < 8; i++) {
			json_array_insert_new(gates4J, i, json_integer((int) gates4[i]));
		}
		json_object_set_new(rootJ, "gates4", gates4J);

		return rootJ;
		
	}

	void dataFromJson(json_t *rootJ) override {

		json_t *runningJ = json_object_get(rootJ, "running");
		if (runningJ)
			running = json_is_true(runningJ);


		json_t *gates1J = json_object_get(rootJ, "gates1");
		if (gates1J) {
			for (int i = 0; i < 8; i++) {
				json_t *gate1J = json_array_get(gates1J, i);
				if (gate1J)
					gates1[i] = !!json_integer_value(gate1J);
			}
		}
		
		json_t *gates2J = json_object_get(rootJ, "gates2");
		if (gates2J) {
			for (int i = 0; i < 16; i++) {
				json_t *gate2J = json_array_get(gates2J, i);
				if (gate2J)
					gates2[i] = !!json_integer_value(gate2J);
			}
		}
		
		json_t *gates3J = json_object_get(rootJ, "gates3");
		if (gates3J) {
			for (int i = 0; i < 8; i++) {
				json_t *gate3J = json_array_get(gates3J, i);
				if (gate3J)
					gates3[i] = !!json_integer_value(gate3J);
			}
		}
		
		json_t *gates4J = json_object_get(rootJ, "gates4");
		if (gates4J) {
			for (int i = 0; i < 8; i++) {
				json_t *gate4J = json_array_get(gates4J, i);
				if (gate4J)
					gates4[i] = !!json_integer_value(gate4J);
			}
		}
		
	}
	
		void setIndex1(int index1) {
		int numSteps1 = (int) (std::round(params[STEPS1_PARAM].getValue()));
		this->index1 = index1;
		if (this->index1 >= numSteps1)
			this->index1 = 0;
	}
	
		void setIndex2(int index2) {
		int numSteps2 = (int) (std::round(params[STEPS2_PARAM].getValue()));
		this->index2 = index2;
		if (this->index2 >= numSteps2)
			this->index2 = 0;
	}

	void process(const ProcessArgs &args) override {
		

		bool gate1In = false;
	
		
				if (clockTrigger1.process(inputs[EXT_CLOCK1_INPUT].getVoltage())) {
					setIndex1(index1 + 1);
				}
				gate1In = clockTrigger1.isHigh();
		
			
		bool gate2In = false;
	
		
				if (clockTrigger1.process(inputs[EXT_CLOCK1_INPUT].getVoltage())) {
					setIndex1(index1 + 1);
				}
				gate2In = clockTrigger1.isHigh();		
		
		
		bool gate3In = false;
		
				if (clockTrigger2.process(inputs[EXT_CLOCK2_INPUT].getVoltage())) {
					setIndex2(index2 + 1);
				}
				gate3In = clockTrigger2.isHigh();
	
		
		bool gate4In = false;
		

				if (clockTrigger2.process(inputs[EXT_CLOCK2_INPUT].getVoltage())) {
					setIndex2(index2 + 1);
				}
				gate4In = clockTrigger2.isHigh();
		


		if (reset1Trigger.process(params[RESET1_PARAM].getValue() + inputs[RESET1_INPUT].getVoltage() + inputs[RESET2_INPUT].getVoltage())) {
			setIndex1(0);
		}

		if (reset2Trigger.process(params[RESET2_PARAM].getValue() + inputs[RESET3_INPUT].getVoltage() + inputs[RESET4_INPUT].getVoltage())) {
			setIndex2(0);
		}

		for (int i = 0; i < 8; i++) {
			if (gate1Triggers[i].process(params[GATE1_PARAM + i].getValue())) {
				gates1[i] = !gates1[i];
			}
			lights[GATE1_LIGHTS + i].setSmoothBrightness((gate1In && i == index1) ? (gates1[i] ? 1.f : 0.33) : (gates1[i] ? 0.66 : 0.0), args.sampleTime);
		
			if (gate2Triggers[i].process(params[GATE2_PARAM + i].getValue())) {
				gates2[i] = !gates2[i];
			}
			lights[GATE2_LIGHTS + i].setSmoothBrightness((gate2In && i == index1) ? (gates2[i] ? 1.f : 0.33) : (gates2[i] ? 0.66 : 0.0), args.sampleTime);
		
			if (gate3Triggers[i].process(params[GATE3_PARAM + i].getValue())) {
				gates3[i] = !gates3[i];
			}
			lights[GATE3_LIGHTS + i].setSmoothBrightness((gate3In && i == index2) ? (gates3[i] ? 1.f : 0.33) : (gates3[i] ? 0.66 : 0.0), args.sampleTime);
		
			if (gate4Triggers[i].process(params[GATE4_PARAM + i].getValue())) {
				gates4[i] = !gates4[i];
			}
			lights[GATE4_LIGHTS + i].setSmoothBrightness((gate4In && i == index2) ? (gates4[i] ? 1.f : 0.33) : (gates4[i] ? 0.66 : 0.0), args.sampleTime);
		}
		
		float gate1A = 0.f;
		gate1A = ((gate1In && gates1[index1]) ? 10.f : 0.f);
		if (gate1ATriggers.process(gate1A)) {
				float r1 = random::uniform();
				float threshold1 = clamp(params[THRESHOLD1_PARAM].getValue() + (((inputs[DMOD1_INPUT].getVoltage()) / 10.0f) * params[DMOD1_PARAM].getValue()) + ((gate1In && gates1[index1]) ? 1.f : 0.f) / 10.f, 0.f, 1.f);
				bool toss1 = (r1 < threshold1);
				outcomes1 = toss1;
				}
				outputs[GATE1A_OUTPUT].setVoltage(outcomes1 ? gate1A : 0.0);
				
		float gate1B = 0.f;
		gate1B = ((gate1In && gates1[index1]) ? 10.f : 0.f);
		if (gate1BTriggers.process(gate1B)) {
				float r1b = random::uniform();
				float threshold1b = clamp(0.15 + ((gate1In && gates1[index1]) ? 1.f : 0.f) / 10.f, 0.f, 1.f);
				bool toss1b = (r1b < threshold1b);
				outcomes1b = toss1b;
				}
				outputs[GATE1B_OUTPUT].setVoltage(outcomes1b ? gate1B : 0.0);
				
		float gate2A = 0.f;
		gate2A = ((gate2In && gates2[index1]) ? 10.f : 0.f);
		if (gate2ATriggers.process(gate2A)) {
				float r2 = random::uniform();
				float threshold2 = clamp(0.15 + ((gate2In && gates2[index1]) ? 1.f : 0.f) / 10.f, 0.f, 1.f);
				bool toss2 = (r2 < threshold2);
				outcomes2 = toss2;
				}
				outputs[GATE2A_OUTPUT].setVoltage(outcomes2 ? gate2A : 0.0);
				
		float gate3A = 0.f;
		gate3A = ((gate3In && gates3[index2]) ? 10.f : 0.f);
		if (gate3ATriggers.process(gate3A)) {
				float r3 = random::uniform();
				float threshold3 = clamp((params[THRESHOLD2_PARAM].getValue()) + ((inputs[DMOD2_INPUT].getVoltage() / 10.0f) * params[DMOD2_PARAM].getValue())  + ((gate3In && gates3[index2]) ? 1.f : 0.f) / 10.f, 0.f, 1.f);
				bool toss3 = (r3 < threshold3);
				outcomes3 = toss3;
				}
				outputs[GATE3A_OUTPUT].setVoltage(outcomes3 ? gate3A : 0.0);
				
		float gate3B = 0.f;
		gate3B = ((gate3In && gates3[index2]) ? 10.f : 0.f);
		if (gate3BTriggers.process(gate3B)) {
				float r3b = random::uniform();
				float threshold3b = clamp((0.15) + ((gate3In && gates3[index2]) ? 1.f : 0.f) / 10.f, 0.f, 1.f);
				bool toss3b = (r3b < threshold3b);
				outcomes3b = toss3b;
				}
				outputs[GATE3B_OUTPUT].setVoltage(outcomes3b ? gate3B : 0.0);
		
		float gate4A = 0.f;
		gate4A = ((gate4In && gates4[index2]) ? 10.f : 0.f);
		if (gate4ATriggers.process(gate4A)) {
				float r4 = random::uniform();
				float threshold4 = clamp(0.15 + ((gate4In && gates4[index2]) ? 1.f : 0.f) / 10.f, 0.f, 1.f);
				bool toss4 = (r4 < threshold4);
				outcomes4 = toss4;
				}
				outputs[GATE4A_OUTPUT].setVoltage(outcomes4 ? gate4A : 0.0);
	
	float scaleout = 0.0f;	
			
		if (params[SCALE_PARAM].getValue() == 2.0) {
			scaleout = (params[ROW1_PARAM + index1].getValue() * 0.1);
		}
		
		if (params[SCALE_PARAM].getValue() == 1.0) {
			scaleout = (params[ROW1_PARAM + index1].getValue() * 0.2);
		}

		if (params[SCALE_PARAM].getValue() == 0.0) {
			scaleout = (params[ROW1_PARAM + index1].getValue() * 0.5);	
		}	
		
		if (params[SCALEINV_PARAM].getValue()) {
		scaleout = (scaleout * -1.0);
		}	

	float scale2out = 0.0f;	
			
		if (params[SCALE2_PARAM].getValue() == 2.0) {
			scale2out = (params[ROW2_PARAM + index2].getValue() * 0.1);
		}
		
		if (params[SCALE2_PARAM].getValue() == 1.0) {
			scale2out = (params[ROW2_PARAM + index2].getValue() * 0.2);
		}

		if (params[SCALE2_PARAM].getValue() == 0.0) {
			scale2out = (params[ROW2_PARAM + index2].getValue() * 0.5);	
		}	
		
		if (params[SCALE2INV_PARAM].getValue()) {
		scale2out = (scale2out * -1.0);
		}	

		outputs[ROW1_OUTPUT].setVoltage(params[ROW1_PARAM + index1].getValue());
		outputs[SCALE1_OUTPUT].setVoltage(scaleout);
		outputs[ROW2_OUTPUT].setVoltage(params[ROW2_PARAM + index2].getValue());
		outputs[SCALE2_OUTPUT].setVoltage(scale2out);
		outputs[GATE1_OUTPUT].setVoltage((gate1In && gates1[index1]) ? 10.f : 0.f);
		outputs[GATE2_OUTPUT].setVoltage((gate2In && gates2[index1]) ? 10.f : 0.f);
		outputs[GATE3_OUTPUT].setVoltage((gate3In && gates3[index2]) ? 10.f : 0.f);
		outputs[GATE4_OUTPUT].setVoltage((gate4In && gates4[index2]) ? 10.f : 0.f);
		lights[RUNNING_LIGHT].value = (running);
		lights[RESET1_LIGHT].setSmoothBrightness(reset1Trigger.isHigh(), args.sampleTime);
		lights[RESET2_LIGHT].setSmoothBrightness(reset2Trigger.isHigh(), args.sampleTime);

	}
};

struct SequenceModelerWidget : ModuleWidget {
	SequenceModelerWidget(SequenceModeler *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SequenceModeler.svg")));

		addParam(createParam<LFMSnapKnob>(Vec(30, 137), module, SequenceModeler::STEPS1_PARAM));
		addParam(createParam<LFMSnapKnob>(Vec(30, 307), module, SequenceModeler::STEPS2_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(368, 50), module, SequenceModeler::THRESHOLD1_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(373, 110), module, SequenceModeler::DMOD1_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(368, 230), module, SequenceModeler::THRESHOLD2_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(373, 290), module, SequenceModeler::DMOD2_PARAM));
		addParam(createParam<LFMSwitch>(Vec(412,127), module, SequenceModeler::SCALE_PARAM));
		addParam(createParam<MS>(Vec(448,127), module, SequenceModeler::SCALEINV_PARAM));
		addParam(createParam<LFMSwitch>(Vec(412,307), module, SequenceModeler::SCALE2_PARAM));
		addParam(createParam<MS>(Vec(448,307), module, SequenceModeler::SCALE2INV_PARAM));

		static const float portX[16] = {100, 130, 160, 190, 220, 250, 280, 310, 100, 130, 160, 190, 220, 250, 280, 310 };
		addInput(createInput<JackPort>(Vec(20, 38), module, SequenceModeler::EXT_CLOCK1_INPUT));
		addInput(createInput<JackPort>(Vec(20, 208), module, SequenceModeler::EXT_CLOCK2_INPUT));
		addInput(createInput<JackPort>(Vec(20, 70), module, SequenceModeler::RESET1_INPUT));
		addInput(createInput<JackPort>(Vec(50, 70), module, SequenceModeler::RESET2_INPUT));
		addInput(createInput<JackPort>(Vec(20, 240), module, SequenceModeler::RESET3_INPUT));
		addInput(createInput<JackPort>(Vec(50, 240), module, SequenceModeler::RESET4_INPUT));
		addInput(createInput<MiniJackPort>(Vec(347, 100), module, SequenceModeler::DMOD1_INPUT));
		addInput(createInput<MiniJackPort>(Vec(347, 280), module, SequenceModeler::DMOD2_INPUT));
		addOutput(createOutput<OutJackPort>(Vec(336, 22), module, SequenceModeler::GATE1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(336, 162), module, SequenceModeler::GATE1A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(411, 22), module, SequenceModeler::GATE2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(411, 57), module, SequenceModeler::GATE2A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(411, 92), module, SequenceModeler::ROW1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(336, 202), module, SequenceModeler::GATE3_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(336, 342), module, SequenceModeler::GATE3A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(411, 202), module, SequenceModeler::GATE4_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(411, 237), module, SequenceModeler::GATE4A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(411, 272), module, SequenceModeler::ROW2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(411, 162), module, SequenceModeler::SCALE1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(411, 342), module, SequenceModeler::SCALE2_OUTPUT));

		for (int i = 0; i < 8; i++) {
			addParam(createParam<LFMSliderWhite>(Vec(portX[i], 45), module, SequenceModeler::ROW1_PARAM + i));
			addParam(createParam<ButtonLED>(Vec(portX[i]-3.5f, 135), module, SequenceModeler::GATE1_PARAM + i));
			addParam(createParam<ButtonLED>(Vec(portX[i]-3.5f, 160), module, SequenceModeler::GATE2_PARAM + i));
			addChild(createLight<LargeLight<GreenLight>>(Vec(portX[i]-3.5f+1.4f, 137), module, SequenceModeler::GATE1_LIGHTS + i));
			addChild(createLight<LargeLight<GreenLight>>(Vec(portX[i]-3.5f+1.4f, 162), module, SequenceModeler::GATE2_LIGHTS + i));
		
		}
		
		for (int i = 0; i < 8; i++) {
			addParam(createParam<LFMSliderWhite>(Vec(portX[i], 205), module, SequenceModeler::ROW2_PARAM + i));
			addParam(createParam<ButtonLED>(Vec(portX[i]-3.5f, 300), module, SequenceModeler::GATE3_PARAM + i));
			addParam(createParam<ButtonLED>(Vec(portX[i]-3.5f, 325), module, SequenceModeler::GATE4_PARAM + i));
			addChild(createLight<LargeLight<GreenLight>>(Vec(portX[i]-3.5f+1.4f, 302), module, SequenceModeler::GATE3_LIGHTS + i));
			addChild(createLight<LargeLight<GreenLight>>(Vec(portX[i]-3.5f+1.4f, 327), module, SequenceModeler::GATE4_LIGHTS + i));
		
		}
		
	}

};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelSequenceModeler = createModel<SequenceModeler, SequenceModelerWidget>("SequenceModeler");