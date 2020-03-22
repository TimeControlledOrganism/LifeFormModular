#pragma once
#include <rack.hpp>


using namespace rack;


extern Plugin *pluginInstance;

struct LFMKnob : RoundKnob {
	LFMKnob() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMKnob.svg")));
		minAngle = -0.68*M_PI;
		maxAngle = 0.68*M_PI;
	}
};

struct LFMTinyKnob : RoundKnob {
	LFMTinyKnob() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMTinyKnob.svg")));
		minAngle = -0.68*M_PI;
		maxAngle = 0.68*M_PI;
	}
};

struct LFMSnapKnob : LFMKnob {
	LFMSnapKnob() {
		snap = true;
	}
};

struct LFMTinySnapKnob : LFMTinyKnob {
	LFMTinySnapKnob() {
		snap = true;
	}
};


struct LFMSlider : app::SvgSlider {
	LFMSlider() {
		maxHandlePos = app::mm2px(math::Vec(0, 0).plus(math::Vec(-2.5f, 0)));
		minHandlePos = app::mm2px(math::Vec(0, 23).plus(math::Vec(-2.5f, 0)));
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMSlider.svg")));
	}
};

struct LFMSliderWhite : LFMSlider {
	LFMSliderWhite() {
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMSliderWhiteHandle.svg")));
	}
};

struct LFMSwitch : app::SvgSwitch {
	LFMSwitch() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMSwitch_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMSwitch_1.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMSwitch_2.svg")));
	}
};

struct MS : app::SvgSwitch {
	MS() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MS_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MS_1.svg")));
		
	}
};


struct JackPort : SVGPort {
	JackPort() {
		setSVG(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Jack.svg")));
	}
};

struct OutJackPort : SVGPort {
	OutJackPort() {
		setSVG(APP->window->loadSvg(asset::plugin(pluginInstance, "res/OutJack.svg")));
	}
};

struct MiniJackPort : SVGPort {
	MiniJackPort() {
		setSVG(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MiniJack.svg")));
	}
};

struct InJackPort : SVGPort {
	InJackPort() {
		setSVG(APP->window->loadSvg(asset::plugin(pluginInstance, "res/InJack.svg")));
	}
};

struct ButtonLED : app::SvgSwitch {
	ButtonLED() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LEDButton.svg")));
	}
};

struct ButtonLEDLatch : app::SvgSwitch {
	ButtonLEDLatch() {
		momentary = false;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LEDButton.svg")));
	}
};


extern Model *modelTimeDiktat;
extern Model *modelSequenceModeler;
extern Model *modelPitchDiktat;
extern Model *modelPitchIntegrator;
extern Model *modelBurstIntegrator;
extern Model *modelQuadModulator;
extern Model *modelImpulseControl;
extern Model *modelQuadSteppedOffset;
extern Model *modelPercussiveVibration;
extern Model *modelQuadUtility;
extern Model *modelAdditiveVibration;
extern Model *modelComplexOsc;
extern Model *modelDriftgen;