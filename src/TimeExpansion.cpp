#include "plugin.hpp"

struct TimeExpansion : Module {
	enum ParamId {
		THRESHOLD1_PARAM,
		DMOD1_PARAM,
		THRESHOLD2_PARAM,
		DMOD2_PARAM,
		THRESHOLD3_PARAM,
		DMOD3_PARAM,
		THRESHOLD4_PARAM,
		DMOD4_PARAM,
		THRESHOLD5_PARAM,
		DMOD5_PARAM,
		THRESHOLD6_PARAM,
		DMOD6_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		DMOD1_INPUT,
		DMOD2_INPUT,
		DMOD3_INPUT,
		DMOD4_INPUT,
		DMOD5_INPUT,
		DMOD6_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		STEPCLOCKMULT16_OUTPUT,
		STEPCLOCKMULT8_OUTPUT,
		STEPCLOCKMULT4_OUTPUT,
		STEPCLOCKMULT2_OUTPUT,
		STEPCLOCKMULT1_OUTPUT,
		STEPCLOCKMULT0_OUTPUT,
		GATE1A_OUTPUT,
		GATE2A_OUTPUT,
		GATE3A_OUTPUT,
		GATE4A_OUTPUT,
		GATE5A_OUTPUT,
		GATE6A_OUTPUT,
		HALFEIGHT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		BLINK_LIGHT,
		NUM_LIGHTS
	};
	
	float leftMessages[2][8] = {};
	bool outcomes1 = false;
	bool outcomes2 = false;
	bool outcomes3 = false;
	bool outcomes4 = false;
	bool outcomes5 = false;
	bool outcomes6 = false;
	dsp::SchmittTrigger gate1ATriggers;
	dsp::SchmittTrigger gate2ATriggers;
	dsp::SchmittTrigger gate3ATriggers;
	dsp::SchmittTrigger gate4ATriggers;
	dsp::SchmittTrigger gate5ATriggers;
	dsp::SchmittTrigger gate6ATriggers;
	
	//dsp::PulseGenerator gatePulse;
	
	
	TimeExpansion() {
		
			config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
			
		leftExpander.producerMessage = leftMessages[0];
		leftExpander.consumerMessage = leftMessages[1];
			
	}
	
	void process(const ProcessArgs &args) override {
		
		//bool nextStep2 = false;
		
		
			if (leftExpander.module && leftExpander.module->model == modelTimeDiktat) {
				
			
			// Get consumer message
			float *message = (float*) leftExpander.consumerMessage;
			for (int i = 0; i < 8; i++) {
				float sixteen = message[0];
				float eighth = message[1];
				float fourth = message[2];
				float half = message[3];
				float measure = message[4];
				float two = message[5];
				
				
				
				
				float gate1A = 0.f;
		gate1A = sixteen;
		if (gate1ATriggers.process(gate1A)) {
				float r1 = random::uniform();
				float threshold1 = clamp((params[THRESHOLD1_PARAM].getValue() + (((inputs[DMOD1_INPUT].getVoltage()) / 10.0f) * params[DMOD1_PARAM].getValue())), 0.f, 1.f);
				bool toss1 = (r1 < threshold1);
				outcomes1 = toss1;
				}
				outputs[GATE1A_OUTPUT].setVoltage(outcomes1 ? gate1A : 0.0);
				
				float gate2A = 0.f;
		gate2A = eighth;
		if (gate2ATriggers.process(gate2A)) {
				float r2 = random::uniform();
				float threshold2 = clamp((params[THRESHOLD2_PARAM].getValue() + (((inputs[DMOD2_INPUT].getVoltage()) / 10.0f) * params[DMOD2_PARAM].getValue())), 0.f, 1.f);
				bool toss2 = (r2 < threshold2);
				outcomes2 = toss2;
				}
				outputs[GATE2A_OUTPUT].setVoltage(outcomes2 ? gate2A : 0.0);
				
				float gate3A = 0.f;
		gate3A = fourth;
		if (gate3ATriggers.process(gate3A)) {
				float r3 = random::uniform();
				float threshold3 = clamp((params[THRESHOLD3_PARAM].getValue() + (((inputs[DMOD3_INPUT].getVoltage()) / 10.0f) * params[DMOD3_PARAM].getValue())), 0.f, 1.f);
				bool toss3 = (r3 < threshold3);
				outcomes3 = toss3;
				}
				outputs[GATE3A_OUTPUT].setVoltage(outcomes3 ? gate3A : 0.0);
				
				float gate4A = 0.f;
		gate4A = half;
		if (gate4ATriggers.process(gate4A)) {
				float r4 = random::uniform();
				float threshold4 = clamp((params[THRESHOLD4_PARAM].getValue() + (((inputs[DMOD4_INPUT].getVoltage()) / 10.0f) * params[DMOD4_PARAM].getValue())), 0.f, 1.f);
				bool toss4 = (r4 < threshold4);
				outcomes4 = toss4;
				}
				outputs[GATE4A_OUTPUT].setVoltage(outcomes4 ? gate4A : 0.0);
				
				float gate5A = 0.f;
		gate5A = measure;
		if (gate5ATriggers.process(gate5A)) {
				float r5 = random::uniform();
				float threshold5 = clamp((params[THRESHOLD5_PARAM].getValue() + (((inputs[DMOD5_INPUT].getVoltage()) / 10.0f) * params[DMOD5_PARAM].getValue())), 0.f, 1.f);
				bool toss5 = (r5 < threshold5);
				outcomes5 = toss5;
				}
				outputs[GATE5A_OUTPUT].setVoltage(outcomes5 ? gate5A : 0.0);
				
						float gate6A = 0.f;
		gate6A = two;
		if (gate6ATriggers.process(gate6A)) {
				float r6 = random::uniform();
				float threshold6 = clamp((params[THRESHOLD6_PARAM].getValue() + (((inputs[DMOD6_INPUT].getVoltage()) / 10.0f) * params[DMOD6_PARAM].getValue())), 0.f, 1.f);
				bool toss6= (r6 < threshold6);
				outcomes6 = toss6;
				}
				outputs[GATE6A_OUTPUT].setVoltage(outcomes6 ? gate6A : 0.0);
				
				
					//float beat = message[2];
					//float gphase = message[0];
					//float gclocktime = message[1];
					//float eighthth = message[0];
					//float bar = message[1];
					//float twobars = message[3];
					//float fourbars = message[4];
					//float eightbars = message[5];
					//float sixtennbar = message[6];
					//float thirtytwobar = message[7];
					//float step = message[8];
					//float phase2 = gphase;
					//float clockTime2 = powf(2.0, (gclocktime - 1.0));	
					//phase2 += clockTime2 / args.sampleRate;
					
		//if (phase2 >= 1.0) {
			//phase2 -= 1.0;
			//nextStep2 = true;
		//}

	//if (nextStep2) {
		//stepCount = (stepCount + 1);
		//gatePulse.trigger(0.1);
		//}
		
		//bool gpulse = gatePulse.process(1.0 / args.sampleRate);
					
				//lights[BLINK_LIGHT].setSmoothBrightness(beat, args.sampleTime);
				
				outputs[STEPCLOCKMULT16_OUTPUT].setVoltage(eighth != fourth);
				outputs[STEPCLOCKMULT8_OUTPUT].setVoltage(sixteen != fourth);
				outputs[STEPCLOCKMULT4_OUTPUT].setVoltage(sixteen != half);
				outputs[STEPCLOCKMULT2_OUTPUT].setVoltage(eighth != half);
				outputs[STEPCLOCKMULT1_OUTPUT].setVoltage(fourth != half);
				outputs[STEPCLOCKMULT0_OUTPUT].setVoltage(half != measure);
				
				bool halfeight = false;
				if (eighth != fourth) {
				halfeight = true;}
				outputs[HALFEIGHT_OUTPUT].setVoltage(halfeight);
				
				
			//}
		}
	}
		else {
			// No mother module is connected.
			// TODO Clear the lights.
		}
		
		//lights[BLINK_LIGHT].setSmoothBrightness(gpulse && (stepCount % 4 == 0) ? 10.0 : 0.0, args.sampleTime);
		
	}
	
};


struct TimeExpansionWidget : ModuleWidget {
	TimeExpansionWidget(TimeExpansion *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TimeExpansion.svg")));
		
		addParam(createParam<LFMTinyKnob>(Vec(96, 35), module, TimeExpansion::THRESHOLD1_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(103, 92), module, TimeExpansion::DMOD1_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(96, 195), module, TimeExpansion::THRESHOLD2_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(103, 252), module, TimeExpansion::DMOD2_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(161, 35), module, TimeExpansion::THRESHOLD3_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(168, 92), module, TimeExpansion::DMOD3_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(161, 195), module, TimeExpansion::THRESHOLD4_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(168, 252), module, TimeExpansion::DMOD4_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(226, 35), module, TimeExpansion::THRESHOLD5_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(233, 92), module, TimeExpansion::DMOD5_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(226, 195), module, TimeExpansion::THRESHOLD6_PARAM));
		addParam(createParam<LFMTinyKnob>(Vec(233, 252), module, TimeExpansion::DMOD6_PARAM));
		
		addInput(createInput<MiniJackPort>(Vec(89, 78), module, TimeExpansion::DMOD1_INPUT));
		addInput(createInput<MiniJackPort>(Vec(89, 238), module, TimeExpansion::DMOD2_INPUT));
		addInput(createInput<MiniJackPort>(Vec(155, 78), module, TimeExpansion::DMOD3_INPUT));
		addInput(createInput<MiniJackPort>(Vec(155, 238), module, TimeExpansion::DMOD4_INPUT));
		addInput(createInput<MiniJackPort>(Vec(219, 78), module, TimeExpansion::DMOD5_INPUT));
		addInput(createInput<MiniJackPort>(Vec(219, 238), module, TimeExpansion::DMOD6_INPUT));
		
		
		addOutput(createOutput<OutJackPort>(Vec(31, 35), module, TimeExpansion::STEPCLOCKMULT16_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(31, 85), module, TimeExpansion::STEPCLOCKMULT8_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(31, 135), module, TimeExpansion::STEPCLOCKMULT4_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(31, 185), module, TimeExpansion::STEPCLOCKMULT2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(31, 235), module, TimeExpansion::STEPCLOCKMULT1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(31, 285), module, TimeExpansion::STEPCLOCKMULT0_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(96, 135), module, TimeExpansion::GATE1A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(96, 295), module, TimeExpansion::GATE2A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(161, 135), module, TimeExpansion::GATE3A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(161, 295), module, TimeExpansion::GATE4A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(226, 135), module, TimeExpansion::GATE5A_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(226, 295), module, TimeExpansion::GATE6A_OUTPUT));
		
		//addOutput(createOutput<OutJackPort>(Vec(149, 320), module, TimeExpansion::HALFEIGHT_OUTPUT));
		
		//addChild(createLight<LargeLight<GreenLight>>(Vec(28, 50), module, TimeExpansion::BLINK_LIGHT));
		
	}
	
};

Model *modelTimeExpansion = createModel<TimeExpansion, TimeExpansionWidget>("TimeExpansion");