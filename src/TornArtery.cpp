#include "plugin.hpp"

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

static float shapeDelta2(float delta2, float tau2, float shape2) {
	float lin2 = sgn(delta2) * 10.f / tau2;
	if (shape2 < 0.f) {
		float log2 = sgn(delta2) * 40.f / tau2 / (std::fabs(delta2) + 1.f);
		return crossfade(lin2, log2, -shape2 * 0.95f);
	}
	else {
		float exp2 = M_E * delta2 / tau2;
		return crossfade(lin2, exp2, shape2 * 0.90f);
	}
}

struct TornArtery : Module {
	enum ParamId {
		SHAPE_A_PARAM,
		TRIGG_A_PARAM,
		RISE_A_PARAM,
		FALL_A_PARAM,
		RISE_B_PARAM,
		FALL_B_PARAM,
		CYCLE_A_PARAM,
		CV_A_DEST_PARAM,
		CV_A_MOD_PARAM,
		INV_PARAM,
		CROSS_PARAM,
		CROSS_MOD_PARAM,
		//THRESHOLD1_PARAM,
		//DMOD1_PARAM,
		//CV_A_MODB_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		CV_A_INPUT,
		AUDIO1_INPUT,
		AUDIO2_INPUT,
		TRIGG_A_INPUT,
		CROSS_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		OUT_A_OUTPUT,
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		NUM_LIGHTS
	};
	


	float out = {};
	bool gate = {};
	dsp::SchmittTrigger trigger;
	dsp::PulseGenerator endOfCyclePulse;
	float out2 = {};
	bool gate2 = {};
	dsp::SchmittTrigger trigger2;
	dsp::PulseGenerator endOfCyclePulse2;
	//bool outcomes1 = false;
	//dsp::SchmittTrigger gate1ATriggers;
	//bool toss1;


	
		TornArtery() {
		
			config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
			
configParam(CV_A_DEST_PARAM, 0.0, 2.0, 0.0, "Env mod destination");
configParam(CV_A_MOD_PARAM, -0.5, 0.5, 0.0, "Env mod amount");
			
		}
		
	void process(const ProcessArgs &args) override {
		

			//	outputs[GATE1A_OUTPUT].setVoltage(outcomes1 ? gate1A : 0.0);
		//float trig2 = ((outcomes1 ? gate1A : 0.0));
		
		
		float in = 0.0;
		float in2 = 0.0;
			if (trigger.process(inputs[TRIGG_A_INPUT].getVoltage())) {
				gate = true;
				gate2 = true;
			}
				//if (toss1) {
				//gate = true;
				//gate2 = true;
			//}
			
					//float gate1A = 0.f;
		//gate1A = (trigger.process(gate));
				//if (gate1ATriggers.process(gate)) {
//float r1 = random::uniform();
				//float threshold1 = clamp((params[THRESHOLD1_PARAM].getValue() + (((inputs[CV_A_INPUT].getVoltage()) / 10.0f) * params[CV_A_MODB_PARAM].getValue())), 0.f, 1.f);
				//toss1 = (r1 < threshold1);
				//outcomes1 = toss1;
				//}
			
			
			if (gate) {
				in = 10.0;
			}
			if (gate2) {
				in2 = 10.0;
			}
			
			float shape = 1.0;
			float delta = in - out;

			float minTime = 1e-2;

			bool rising = false;
			bool falling = false;


			float shape2 = 1.0;
			float delta2 = in2 - out2;

			float minTime2 = 1e-2;

			bool rising2 = false;
			bool falling2 = false;



			if (delta > 0) {
				
				float riseCv = params[RISE_A_PARAM].getValue(); 
				if (params[CV_A_DEST_PARAM].getValue() == 2.0) {
				riseCv = params[RISE_A_PARAM].getValue()+ (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0 ;
				}
				if (params[CV_A_DEST_PARAM].getValue() == 0.0) {
				riseCv = params[RISE_A_PARAM].getValue()+ (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0 ;
				}
				riseCv = clamp(riseCv, 0.0f, 1.0f);
				float rise = minTime * std::pow(2.0, riseCv * 10.0);
				out += shapeDelta(delta, rise, shape) * args.sampleTime;
				rising = (in - out > 1e-3);
				if (!rising) {
					gate = false;
				}
			}
			else if (delta < 0) {
				
				float fallCv = params[FALL_A_PARAM].getValue(); 
				if (params[CV_A_DEST_PARAM].getValue() == 2.0) {
				fallCv = params[FALL_A_PARAM].getValue() + (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0;
				}
				if (params[CV_A_DEST_PARAM].getValue() == 1.0) {
				fallCv = params[FALL_A_PARAM].getValue() + (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue())/ 10.0;
				}
				fallCv = clamp(fallCv, 0.0f, 1.0f);
				float fall = minTime * std::pow(2.0, fallCv * 10.0);
				out += shapeDelta(delta, fall, shape) * args.sampleTime;
				falling = (in - out < -1e-3);
				//if (!falling) {
					//endOfCyclePulse.trigger(1e-3);
					//if (params[CYCLE_A_PARAM].getValue()) {
						//gate = true;
					//}
				}
		//	}
			//else {
				//gate = false;
			//}

			if (!rising && !falling) {
				out = in;
			}

			float xfade = params[CROSS_PARAM].getValue() + ((inputs[CROSS_INPUT].getVoltage() / 10.0f) * params[CROSS_MOD_PARAM].getValue());
			xfade = clamp(xfade , 0.0, 1.0);
			//float mix = inputs[AUDIO_INPUT].getVoltage();
			//float xfade = params[CROSS_PARAM].getValue();
			
		

//outputs[AUDIO_OUTPUT].setVoltage(inputs[AUDIO_INPUT].getVoltage() * (out/10.0));




			if (delta2 > 0) {
				
				float riseCv2 = params[RISE_B_PARAM].getValue(); 
				if (params[CV_A_DEST_PARAM].getValue() == 2.0) {
				riseCv2 = params[RISE_B_PARAM].getValue()+ (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0 ;
				}
				if (params[CV_A_DEST_PARAM].getValue() == 0.0) {
				riseCv2 = params[RISE_B_PARAM].getValue()+ (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0 ;
				}
				riseCv2 = clamp(riseCv2, 0.0f, 1.0f);
				float rise2 = minTime2 * std::pow(2.0, riseCv2 * 10.0);
				out2 += shapeDelta2(delta2, rise2, shape2) * args.sampleTime;
				rising2 = (in2 - out2 > 1e-3);
				if (!rising2) {
					gate2 = false;
				}
			}
			else if (delta2 < 0) {
				
				float fallCv2 = params[FALL_B_PARAM].getValue(); 
				if (params[CV_A_DEST_PARAM].getValue() == 2.0) {
				fallCv2 = params[FALL_B_PARAM].getValue() + (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue()) / 10.0;
								if (params[INV_PARAM].getValue() == 1.0) {
		fallCv2 = (params[FALL_B_PARAM].getValue() + ((inputs[CV_A_INPUT].getVoltage() * -1.0) * params[CV_A_MOD_PARAM].getValue()) / 10.0);
				}
				}
				if (params[CV_A_DEST_PARAM].getValue() == 1.0) {
				fallCv2 = params[FALL_B_PARAM].getValue() + (inputs[CV_A_INPUT].getVoltage() * params[CV_A_MOD_PARAM].getValue())/ 10.0;
						if (params[INV_PARAM].getValue() == 1.0) {
		fallCv2 = (params[FALL_B_PARAM].getValue() + ((inputs[CV_A_INPUT].getVoltage() * -1.0) * params[CV_A_MOD_PARAM].getValue()) / 10.0);
				}
				}
				fallCv2 = clamp(fallCv2, 0.0f, 1.0f);
				float fall2 = minTime2 * std::pow(2.0, fallCv2 * 10.0);

				out2 += shapeDelta2(delta2, fall2, shape2) * args.sampleTime;
				falling2 = (in2 - out2 < -1e-3);
				if (!falling2) {
					endOfCyclePulse2.trigger(1e-3);
					if (params[CYCLE_A_PARAM].getValue()) {
						gate2 = true;
					}
				}
			}
			else {
				gate2 = false;
			}

			if (!rising2 && !falling2) {
				out2 = in2;
			}


outputs[OUT_A_OUTPUT].setVoltage(out2);
outputs[AUDIO_OUTPUT].setVoltage((crossfade((inputs[AUDIO1_INPUT].getVoltage()),(inputs[AUDIO2_INPUT].getVoltage()),xfade)) * (out/10.0));

		
	}	
	};


struct TornArteryWidget : ModuleWidget {
	TornArteryWidget(TornArtery *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TornArtery.svg")));
	
	addInput(createInput<JackPort>(Vec(43, 45), module, TornArtery::TRIGG_A_INPUT));
	addParam(createParam<LFMKnob>(Vec(110, 35), module, TornArtery::RISE_A_PARAM));
	addParam(createParam<LFMKnob>(Vec(180, 35), module, TornArtery::FALL_A_PARAM));
	addParam(createParam<LFMKnob>(Vec(110, 95), module, TornArtery::RISE_B_PARAM));
	addParam(createParam<LFMKnob>(Vec(180, 95), module, TornArtery::FALL_B_PARAM));
	addParam(createParam<LFMKnob>(Vec(40, 215), module, TornArtery::CROSS_PARAM));
	addOutput(createOutput<OutJackPort>(Vec(183, 282), module, TornArtery::OUT_A_OUTPUT));
	addOutput(createOutput<OutJackPort>(Vec(113, 282), module, TornArtery::AUDIO_OUTPUT));
	addParam(createParam<MS>(Vec(190, 170), module, TornArtery::CYCLE_A_PARAM));
	addParam(createParam<LFMSwitch>(Vec(178, 220), module, TornArtery::CV_A_DEST_PARAM));
	addParam(createParam<MS>(Vec(120, 170), module, TornArtery::INV_PARAM));
	addInput(createInput<MiniJackPort>(Vec(118, 218), module, TornArtery::CV_A_INPUT));
	addInput(createInput<JackPort>(Vec(43, 105), module, TornArtery::AUDIO1_INPUT));
	addInput(createInput<JackPort>(Vec(43, 165), module, TornArtery::AUDIO2_INPUT));
	addParam(createParam<LFMTinyKnob>(Vec(130, 232), module, TornArtery::CV_A_MOD_PARAM));
	
	addParam(createParam<LFMTinyKnob>(Vec(47, 282), module, TornArtery::CROSS_MOD_PARAM));
	addInput(createInput<MiniJackPort>(Vec(33, 268), module, TornArtery::CROSS_INPUT));
	//addParam(createParam<LFMTinyKnob>(Vec(18, 145), module, TornArtery::CV_A_MODB_PARAM));
	//addParam(createParam<LFMKnob>(Vec(10, 185), module, TornArtery::THRESHOLD1_PARAM));
	}
	
};

Model *modelTornArtery = createModel<TornArtery, TornArteryWidget>("TornArtery");	
		