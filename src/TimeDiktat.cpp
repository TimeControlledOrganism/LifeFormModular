#include "plugin.hpp"

//Based on JW modules clock : https://github.com/jeremywen/JW-Modules
//made to learn rudiments of coding and have fun

struct TimeDiktat : Module {
	enum ParamIds {
		RATE_PARAM,
		MANUALRESET_PARAM,
		RUN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		RESET_OUTPUT,
		CLOCKMULT16_OUTPUT,
		CLOCKMULT8_OUTPUT,
		CLOCKMULT4_OUTPUT,
		CLOCKMULT2_OUTPUT,
		CLOCKMULT1_OUTPUT,
		CLOCKDIV2_OUTPUT,
		CLOCKDIV4_OUTPUT,
		CLOCKDIV8_OUTPUT,
		CLOCKDIV16_OUTPUT,
		CLOCKDIV32_OUTPUT,
		RATECV_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		RUNNING_LIGHT,
		RESET_LIGHT,
		NUM_LIGHTS
	};
	
	bool running = true;
	float phase = 0.0;
	dsp::SchmittTrigger runningTrigger;
	dsp::PulseGenerator gatePulse;
	dsp::PulseGenerator lightPulse;
	dsp::SchmittTrigger manualresetTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::PulseGenerator resetPulse;	
	int stepCount = 0;
	
	TimeDiktat() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RATE_PARAM, -2.f, 6.f, 2.f, "Clock Rate");
		configParam(MANUALRESET_PARAM, 0.f, 1.f, 0.f, "Reset");
		configParam(RUN_PARAM, 0.f, 1.f, 0.f, "Run");
		}
		
	void process(const ProcessArgs &args) override;

	void resetClock() {
		phase = 0.0;
		resetPulse.trigger(0.001);
		stepCount = 0;
	}

		json_t *dataToJson() override {
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "running", json_boolean(running));
		
		return rootJ;
		}

	void dataFromJson(json_t *rootJ) override {
		// running
		json_t *runningJ = json_object_get(rootJ, "running");
		if (runningJ)
			running = json_is_true(runningJ);
	}

};


void TimeDiktat::process(const ProcessArgs &args)  {
	
	
	if (runningTrigger.process(params[RUN_PARAM].getValue())) {
		running = !running;
		resetClock();
		resetPulse.trigger(0.01);
		}
	
		if (running) {	
		if (manualresetTrigger.process(params[MANUALRESET_PARAM].getValue())){
		resetClock();
	}

		bool nextStep = false;
		
		float clockTime = powf(2.0, params[RATE_PARAM].getValue());			
		phase += clockTime / args.sampleRate;
		
		if (phase >= 1.0) {
			phase -= 1.0;
			nextStep = true;
		}


	if (nextStep) {
		stepCount = (stepCount + 1);
		gatePulse.trigger(0.01);
		}
	
	}

	bool rpulse = resetPulse.process(1.0 / args.sampleRate);
	bool gpulse = gatePulse.process(1.0 / args.sampleRate);

	outputs[CLOCKMULT16_OUTPUT].setVoltage(gpulse ? 10.0 : 0.0);
	outputs[CLOCKMULT8_OUTPUT].setVoltage(gpulse && (stepCount % 2 == 0)  ? 10.0 : 0.0 );
	outputs[CLOCKMULT4_OUTPUT].setVoltage(gpulse && (stepCount % 4 == 0) ? 10.0 : 0.0);
	outputs[CLOCKMULT2_OUTPUT].setVoltage(gpulse && (stepCount % 8 == 0) ? 10.0 : 0.0);
	outputs[CLOCKMULT1_OUTPUT].setVoltage(gpulse && (stepCount % 16 == 0) ? 10.0 : 0.0);
	outputs[CLOCKDIV2_OUTPUT].setVoltage(gpulse && (stepCount % 32 == 0) ? 10.0 : 0.0);
	outputs[CLOCKDIV4_OUTPUT].setVoltage(gpulse && (stepCount % 64 == 0) ? 10.0 : 0.0);
	outputs[CLOCKDIV8_OUTPUT].setVoltage(gpulse && (stepCount % 128 == 0) ? 10.0 : 0.0);
	outputs[CLOCKDIV16_OUTPUT].setVoltage(gpulse && (stepCount % 256 == 0) ? 10.0 : 0.0);
	outputs[CLOCKDIV32_OUTPUT].setVoltage(gpulse && (stepCount % 512 == 0) ? 10.0 : 0.0);
	outputs[RESET_OUTPUT].setVoltage(rpulse ? 10.0 : 0.0);
	outputs[RATECV_OUTPUT].setVoltage(params[RATE_PARAM].getValue());
	lights[RUNNING_LIGHT].value = (running);
	lights[RESET_LIGHT].setSmoothBrightness(rpulse && (stepCount % 1 == 0) ? 10.0 : 0.0, args.sampleTime);
	lights[BLINK_LIGHT].setSmoothBrightness(gpulse && (stepCount % 4 == 0) ? 10.0 : 0.0, args.sampleTime);
	
}	

	
struct TimeDiktatWidget : ModuleWidget {
	TimeDiktatWidget(TimeDiktat *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TimeDiktat.svg")));


		addParam(createParam<LFMKnob>(Vec(93.5f, 40), module, TimeDiktat::RATE_PARAM));
		addParam(createParam<ButtonLED>(Vec(13, 83), module, TimeDiktat::MANUALRESET_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(14.4f, 84.4), module, TimeDiktat::RESET_LIGHT));
		addParam(createParam<ButtonLED>(Vec(106, 145), module, TimeDiktat::RUN_PARAM));
		addChild(createLight<LargeLight<GreenLight>>(Vec(107.4f, 146.4), module, TimeDiktat::RUNNING_LIGHT));

		addOutput(createOutput<OutJackPort>(Vec(85, 191), module, TimeDiktat::CLOCKMULT16_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(85, 226), module, TimeDiktat::CLOCKMULT8_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(85, 261), module, TimeDiktat::CLOCKMULT4_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(85, 296), module, TimeDiktat::CLOCKMULT2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(85, 331), module, TimeDiktat::CLOCKMULT1_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(10, 191), module, TimeDiktat::CLOCKDIV2_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(10, 226), module, TimeDiktat::CLOCKDIV4_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(10, 261), module, TimeDiktat::CLOCKDIV8_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(10, 296), module, TimeDiktat::CLOCKDIV16_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(10, 331), module, TimeDiktat::CLOCKDIV32_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(10, 139), module, TimeDiktat::RESET_OUTPUT));
		addOutput(createOutput<OutJackPort>(Vec(85, 108), module, TimeDiktat::RATECV_OUTPUT));
		
		addChild(createLight<LargeLight<GreenLight>>(Vec(28, 40), module, TimeDiktat::BLINK_LIGHT));
	
	}
};


Model *modelTimeDiktat = createModel<TimeDiktat, TimeDiktatWidget>("TimeDiktat");