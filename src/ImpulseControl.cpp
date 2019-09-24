#include "plugin.hpp"

//Based On Fundamental Seq3
//https://github.com/VCVRack/Fundamental
//Dedicated to triggers with probability outs from Audible Bernouilli Gate
//https://github.com/VCVRack/AudibleInstruments

struct ImpulseControl : Module {
	enum ParamId {
		CLOCK1_PARAM,
		RESET1_PARAM,
		STEPS1_PARAM,
		THRESHOLD1_PARAM,
		DMOD1_PARAM,
		ENUMS(GATE1_PARAM, 8),
		ENUMS(GATE2_PARAM, 8),
		NUM_PARAMS
	};
	enum InputId {
		EXT_CLOCK1_INPUT,
		RESET1_INPUT,
		RESET2_INPUT,
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
		NUM_OUTPUTS
	};
	enum LightId {
		RESET1_LIGHT,
		RESET2_LIGHT,
		ENUMS(GATE1_LIGHTS, 16),
		ENUMS(GATE2_LIGHTS, 16),
		NUM_LIGHTS
	};

	bool running = true;
	dsp::SchmittTrigger clockTrigger1;
	dsp::SchmittTrigger runningTrigger;
	dsp::SchmittTrigger reset1Trigger;
	dsp::SchmittTrigger reset2Trigger;
	dsp::SchmittTrigger gate1Triggers[8];
	dsp::SchmittTrigger gate1ATriggers;
	dsp::SchmittTrigger gate1BTriggers;
	dsp::SchmittTrigger gate2Triggers[8];
	dsp::SchmittTrigger gate2ATriggers;
	int index1 = 0;
	bool gates1[8] = {};
	bool outcomes1 = false;
	bool outcomes1b = false;
	bool gates2[8] = {};
	bool outcomes2 = false;
	
	ImpulseControl() {



		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RESET1_PARAM, 0.f, 1.f, 0.f,"Reset");
		configParam(STEPS1_PARAM, 1.f, 8.f, 8.f,"Number of steps");
		configParam(THRESHOLD1_PARAM, 0.f, 1.f, 1.f,"Probabilty Threshold");
		configParam(DMOD1_PARAM, -1.f, 1.f, 0.0f,"Probability Mod ");
		for (int i = 0; i < 8; i++) {
			configParam(GATE1_PARAM + i, 0.f, 1.f, 0.f,"Trig 1");
			configParam(GATE2_PARAM + i, 0.f, 1.f, 0.f,"Trig 2");
		}
		
		
	}

	void onReset() override {
		for (int i = 0; i < 8; i++) {
			gates1[i] = true;
			gates2[i] = true;
		}
	}

	void onRandomize() override {
		for (int i = 0; i < 8; i++) {
			gates1[i] = (random::uniform() > 0.5f);
			gates2[i] = (random::uniform() > 0.5f);
		}
	}
		
		json_t *dataToJson() override {
		json_t *rootJ = json_object();

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

		return rootJ;
			
	}

	void dataFromJson(json_t *rootJ) override {

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
		
		
	}
	
		void setIndex1(int index1) {
		int numSteps1 = (int) clamp(std::round(params[STEPS1_PARAM].getValue()), 1.f, 8.f);
		this->index1 = index1;
		if (this->index1 >= numSteps1)
			this->index1 = 0;
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

		
		if (reset1Trigger.process(params[RESET1_PARAM].getValue() + inputs[RESET1_INPUT].getVoltage() + inputs[RESET2_INPUT].getVoltage())) {
			setIndex1(0);
		}


		for (int i = 0; i < 8; i++) {
			if (gate1Triggers[i].process(params[GATE1_PARAM + i].getValue())) {
				gates1[i] = !gates1[i];
			}
			lights[GATE1_LIGHTS + i].setSmoothBrightness((gate1In && i == index1) ? (gates1[i] ? 1.f : 0.33) : (gates1[i] ? 0.66 : 0.0), args.sampleTime);
		}
		
		for (int i = 0; i < 8; i++) {
			if (gate2Triggers[i].process(params[GATE2_PARAM + i].getValue())) {
				gates2[i] = !gates2[i];
			}
			lights[GATE2_LIGHTS + i].setSmoothBrightness((gate2In && i == index1) ? (gates2[i] ? 1.f : 0.33) : (gates2[i] ? 0.66 : 0.0), args.sampleTime);
		}
		
		float gate1A = 0.f;
		gate1A = ((gate1In && gates1[index1]) ? 10.f : 0.f);
		if (gate1ATriggers.process(gate1A)) {
				float r1 = random::uniform();
				float threshold1 = clamp((params[THRESHOLD1_PARAM].getValue() + (((inputs[DMOD1_INPUT].getVoltage()) / 10.0f) * params[DMOD1_PARAM].getValue()) + ((gate1In && gates1[index1]) ? 1.f : 0.f) / 10.f), 0.f, 1.f);
				bool toss1 = (r1 < threshold1);
				outcomes1 = toss1;
				}
				outputs[GATE1A_OUTPUT].setVoltage(outcomes1 ? gate1A : 0.0);
				
		float gate1B = 0.f;
		gate1B = ((gate1In && gates1[index1]) ? 10.f : 0.f);
		if (gate1BTriggers.process(gate1B)) {
				float r1b = random::uniform();
				float threshold1b = clamp((0.15) + ((gate1In && gates1[index1]) ? 1.f : 0.f) / 10.f, 0.f, 1.f);
				bool toss1b = (r1b < threshold1b);
				outcomes1b = toss1b;
				}
				outputs[GATE1B_OUTPUT].setVoltage(outcomes1b ? gate1B : 0.0);
				
		float gate2A = 0.f;
		gate2A = ((gate2In && gates2[index1]) ? 10.f : 0.f);
		if (gate2ATriggers.process(gate2A)) {
				float r2 = random::uniform();
				float threshold2 = clamp((params[THRESHOLD1_PARAM].getValue()) + ((gate2In && gates2[index1]) ? 1.f : 0.f) / 10.f, 0.f, 1.f);
				bool toss2 = (r2 < threshold2);
				outcomes2 = toss2;
				}
				outputs[GATE2A_OUTPUT].setVoltage(outcomes2 ? gate2A : 0.0);

		outputs[GATE1_OUTPUT].setVoltage((gate1In && gates1[index1]) ? 10.f : 0.f);
		outputs[GATE2_OUTPUT].setVoltage((gate2In && gates2[index1]) ? 10.f : 0.f);

	}
};

struct ImpulseControlWidget : ModuleWidget {
	ImpulseControlWidget(ImpulseControl *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ImpulseControl.svg")));

		addParam(createParam<LFMSnapKnob>(Vec(10, 162), module, ImpulseControl::STEPS1_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(18, 230), module, ImpulseControl::THRESHOLD1_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(23, 300), module, ImpulseControl::DMOD1_PARAM));

		static const float portY[16] = {45, 75, 105, 135, 165, 195, 225, 255, 45, 75, 105, 135, 165, 195, 225, 255 };
		
		addInput(createInput<JackPort>(Vec(18, 45), module, ImpulseControl::EXT_CLOCK1_INPUT));
		addInput(createInput<JackPort>(Vec(5, 95), module, ImpulseControl::RESET1_INPUT));
		addInput(createInput<JackPort>(Vec(35, 95), module, ImpulseControl::RESET2_INPUT));
		addInput(createInput<MiniJackPort>(Vec(5, 285), module, ImpulseControl::DMOD1_INPUT));

		addOutput(createOutput<OutJackPort>(Vec(67, 280), module, ImpulseControl::GATE1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(67, 330), module, ImpulseControl::GATE1A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(112, 280), module, ImpulseControl::GATE2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(112, 330), module, ImpulseControl::GATE2A_OUTPUT));


		for (int i = 0; i < 8; i++) {
			addParam(createParam<ButtonLED>(Vec(70,portY[i]-3.5f), module, ImpulseControl::GATE1_PARAM + i));
			addParam(createParam<ButtonLED>(Vec(115,portY[i]-3.5f), module, ImpulseControl::GATE2_PARAM + i));
			addChild(createLight<LargeLight<GreenLight>>(Vec(71.4,portY[i]-3.5f+1.4f), module, ImpulseControl::GATE1_LIGHTS + i));
			addChild(createLight<LargeLight<GreenLight>>(Vec(116.4,portY[i]-3.5f+1.4f), module, ImpulseControl::GATE2_LIGHTS + i));
		}
		
	}

};

Model *modelImpulseControl = createModel<ImpulseControl, ImpulseControlWidget>("ImpulseControl");
