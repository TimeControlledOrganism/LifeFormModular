#include "plugin.hpp"

struct Driftgen : Module {
	enum ParamId {
		RATE_PARAM,
		DEPTH_PARAM,
		RATE2_PARAM,
		DEPTH2_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		CVIN_INPUT,
		CVIN2_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightId {
		NUM_LIGHTS
	};
	
	float phase = 0.0;
	float phase2 = 0.0;
	
	dsp::PulseGenerator gatePulse;
	dsp::PulseGenerator gatePulse2;
	
	float sample = 0.0;
	float sample2 = 0.0;
	
	float out = 0.0;
	float out2 = 0.0;
	
		Driftgen() {
		
			config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
			
			configParam(RATE_PARAM, 0.0, 1.0, 0.0, "Rate");
			configParam(DEPTH_PARAM, 0.0, 1.0, 0.0, "Depth");
			configParam(RATE2_PARAM, 0.0, 1.0, 0.0, "Rate2");
			configParam(DEPTH2_PARAM, 0.0, 1.0, 0.0, "Depth2");
			
		}
		
	void process(const ProcessArgs &args) override {
		
		const float slewMin = 0.1;
		const float slewMax = 10000.f;

		const float shapeScale = 1/10.f;
		
		float range = (params[DEPTH_PARAM].getValue());
		float range2 = (params[DEPTH2_PARAM].getValue());
		
		if (inputs[CVIN_INPUT].isConnected()){
		
		float noise = 2.0 * random::normal();
		bool nextStep = false;
		float clockTime = powf(2.0, (params[RATE_PARAM].getValue() * 3.0));			
		phase += clockTime / args.sampleRate;
	
		if (phase >= 1.0) {
			phase -= 1.0;
			nextStep = true;
		}
		
		if (nextStep) {
		gatePulse.trigger(0.01);
		}
		
		bool gpulse = gatePulse.process(1.0 / args.sampleRate);

		float sandh = (gpulse ? 10.0 : 0.0);		
		
		if (sandh) {
			sample = noise;
		}
		
		float in = sample;

		if (in > out) {
			float rise = (1.0 - ((params[RATE_PARAM].getValue() * 0.15) + 0.3));
			float slew = slewMax * std::pow(slewMin / slewMax, rise);
			out += slew * crossfade(1.f, shapeScale * (in - out), 1.0) * args.sampleTime;
			if (out > in)
				out = in;
		}

		else if (in < out) {
			float fall = (1.0 - ((params[RATE_PARAM].getValue() * 0.15) + 0.3));
			float slew = slewMax * std::pow(slewMin / slewMax, fall);
			out -= slew * crossfade(1.f, shapeScale * (out - in), 1.0) * args.sampleTime;
			if (out < in)
				out = in;
		}
		
		}
		
		if (inputs[CVIN2_INPUT].isConnected()){
			
		float noise2 = 2.0 * random::normal();
		bool nextStep2 = false;
		
		float clockTime2 = powf(2.0, (params[RATE2_PARAM].getValue() * 3.0));			
		phase2 += clockTime2 / args.sampleRate;
		
		if (phase2 >= 1.0) {
			phase2 -= 1.0;
			nextStep2 = true;
		}
		
		if (nextStep2) {
		gatePulse2.trigger(0.01);
		}
		
		bool gpulse2 = gatePulse2.process(1.0 / args.sampleRate);
		
		float sandh2 = (gpulse2 ? 10.0 : 0.0);
		
		if (sandh2) {
			sample2 = noise2;
		}
		
		float in2 = sample2;
		
		if (in2 > out2) {
			float rise2 = (0.8 - ((params[RATE2_PARAM].getValue() * 0.15) + 0.3));
			float slew2 = slewMax * std::pow(slewMin / slewMax, rise2);
			out2 += slew2 * crossfade(1.f, shapeScale * (in2 - out2), 1.0) * args.sampleTime;
			if (out2 > in2)
				out2 = in2;
		}

		else if (in2 < out2) {
			float fall2 = (0.8 - ((params[RATE2_PARAM].getValue() * 0.15) + 0.3));
			float slew2 = slewMax * std::pow(slewMin / slewMax, fall2);
			out2 -= slew2 * crossfade(1.f, shapeScale * (out2 - in2), 1.0) * args.sampleTime;
			if (out2 < in2)
				out2 = in2;
		}
		
		}

		outputs[OUT1_OUTPUT].setVoltage(inputs[CVIN_INPUT].getVoltage() + (out * (range *0.018)));
		outputs[OUT2_OUTPUT].setVoltage(inputs[CVIN2_INPUT].getVoltage() + (out2 * (range2 *0.018)));	

		
	}	
	};


struct DriftgenWidget : ModuleWidget {
	DriftgenWidget(Driftgen *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Driftgen.svg")));
	
	addInput(createInput<JackPort>(Vec(27, 65), module, Driftgen::CVIN_INPUT));	
	addParam(createParam<LFMKnob>(Vec(20,  185), module, Driftgen::DEPTH_PARAM));
	addParam(createParam<LFMKnob>(Vec(20,  115), module, Driftgen::RATE_PARAM));
	addOutput(createOutput<OutJackPort>(Vec(27, 255), module, Driftgen::OUT1_OUTPUT));
	
	addInput(createInput<JackPort>(Vec(92, 65), module, Driftgen::CVIN2_INPUT));	
	addParam(createParam<LFMKnob>(Vec(85,  185), module, Driftgen::DEPTH2_PARAM));
	addParam(createParam<LFMKnob>(Vec(85,  115), module, Driftgen::RATE2_PARAM));
	addOutput(createOutput<OutJackPort>(Vec(92, 255), module, Driftgen::OUT2_OUTPUT));
	

		
	}
	
};

Model *modelDriftgen = createModel<Driftgen, DriftgenWidget>("Driftgen");	
		
		
		
		
		
		
		