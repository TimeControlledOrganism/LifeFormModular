#include "plugin.hpp"

//Based on ML Quantum v 0.6
//https://github.com/martin-lueders/ML_modules
//Added noise for quantized random and glide from Befaco Slew
//https://github.com/VCVRack/Befaco

struct PitchDiktat : Module {
	enum ParamIds {
		SCALE_PARAM,
		SCALEON_PARAM,
		SEMI1_PARAM,
		SEMI2_PARAM,
		SEMI3_PARAM,
		SEMI4_PARAM,
		SEMI5_PARAM,
		SEMI6_PARAM,
		SEMI7_PARAM,
		SEMI8_PARAM,
		SEMI9_PARAM,
		SEMI0_PARAM,
		SEMI11_PARAM,
		SEMI12_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		TRIG_INPUT,
		TRANSPOSE_INPUT,
		OFFSET_INPUT,
		VAR_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		SEMI1_LIGHT,
		SEMI2_LIGHT,
		SEMI3_LIGHT,
		SEMI4_LIGHT,
		SEMI5_LIGHT,
		SEMI6_LIGHT,
		SEMI7_LIGHT,
		SEMI8_LIGHT,
		SEMI9_LIGHT,
		SEMI10_LIGHT,
		SEMI11_LIGHT,
		SEMI12_LIGHT,
		NUM_LIGHTS
	};

	PitchDiktat() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		
				for(int i=0; i<12; i++) {
		 configParam(SEMI1_PARAM, 0.0f, 1.0f, 0.0f,"Pitch");
		}
		
		configParam(SCALE_PARAM, 0.0f, 2.0f, 0.0f,"Scale");
		
	}
		
		
	void process(const ProcessArgs &args) override;
	
	dsp::SchmittTrigger gateTrigger1;
	dsp::PulseGenerator on1;
	
	bool gate1=false, gate2=false, gatev=false;
	
	bool semiState[12] = {};
	
	float semiLight[12] = {};
	
	dsp::PulseGenerator pulse;
	dsp::SchmittTrigger trigger; 

	int last_octave=0, last_semi=0;

	float sample1 = 0.0;
	
	float var=0.0;
	
	float outgl = 0.0f;	

	dsp::SchmittTrigger semiTriggers[12];

	int modulo(int a, int b) {
		int r = a % b;
		return r < 0 ? r + b : r;
	}
	
	dsp::SchmittTrigger gatevTrigger;
	dsp::PulseGenerator on1v;	

		
	void reset()  {
        for (int i = 0; i < 12; i++) {
            semiState[i] = false;
			semiLight[i] = 0.0;
        }
		last_octave = 0;
		last_semi   = 0;
	}
	
	json_t *dataToJson() override {
    json_t *rootJ = json_object();

		
    json_t *scaleJ = json_array();
        for (int i = 0; i < 12; i++) {
            json_t *semiJ = json_integer( (int) semiState[i]);
            json_array_append_new(scaleJ, semiJ);
            }
		json_object_set_new(rootJ, "scale", scaleJ);

        return rootJ;
		
	}
	
	
	void dataFromJson(json_t *rootJ) override  {
        json_t *scaleJ = json_object_get(rootJ, "scale");
		for (int i = 0; i < 12; i++) {
            json_t *semiJ = json_array_get(scaleJ, i);
            semiState[i] = !!json_integer_value(semiJ);
			semiLight[i] = semiState[i]?1.0:0.0;
        }

	}
	

};


void PitchDiktat::process(const ProcessArgs &args)  {
	
	
	float gSampleTime = args.sampleTime;
	
	float noise = (random::normal() + 2.5);
	
	for(int i=0; i<12; i++) {

		if (semiTriggers[i].process(params[PitchDiktat::SEMI1_PARAM + i].getValue())) {
                        semiState[i] = !semiState[i];
                }
		lights[i].value = semiState[i]?1.0f:0.0f;
		
	}
	
	float v = noise + inputs[TRANSPOSE_INPUT].getVoltage();
			
	float quantized = 0.f;
	
	if (params[SCALE_PARAM].getValue() == 0.0){
	v = (noise * 0.5) + inputs[TRANSPOSE_INPUT].getVoltage();
	}
	
	if (params[SCALE_PARAM].getValue() == 1.0){
	v = (noise * 0.2) + inputs[TRANSPOSE_INPUT].getVoltage();
	}
	
	if (params[SCALE_PARAM].getValue() == 2.0){
	v = (noise * 0.1) + inputs[TRANSPOSE_INPUT].getVoltage();
	}
	
	
	if (inputs[PITCH_INPUT].isConnected()){
		v = (inputs[PITCH_INPUT].getVoltage() + inputs[TRANSPOSE_INPUT].getVoltage());
		
		if (params[SCALEON_PARAM].getValue()){
		if (params[SCALE_PARAM].getValue() == 0.0){
		v = (inputs[PITCH_INPUT].getVoltage() * 0.5 + inputs[TRANSPOSE_INPUT].getVoltage());
		}
		if (params[SCALE_PARAM].getValue() == 1.0){
		v = (inputs[PITCH_INPUT].getVoltage() * 0.2 + inputs[TRANSPOSE_INPUT].getVoltage());
		}
		if (params[SCALE_PARAM].getValue() == 2.0){
		v = (inputs[PITCH_INPUT].getVoltage() * 0.1	+ inputs[TRANSPOSE_INPUT].getVoltage());
		}
		}
	}
		
	int semi_full   = round( 12.0f*v );

	int octave   = semi_full/12;

	int semi = semi_full % 12;

	int i_up   = 0;
	int i_down = 0;

			while( !semiState[ modulo(semi+i_up,  12) ] && i_up   < 12 ) i_up++;
		 	while( !semiState[ modulo(semi-i_down,12) ] && i_down < 12 ) i_down++;
			
			if (i_up<12 && i_down<12) semi = (i_up > i_down) ? (semi - i_down) : (semi + i_up); 
			else {semi = last_semi; octave = last_octave;}
			
			bool changed = !( (octave==last_octave)&&(semi==last_semi));
			quantized = 1.0f*octave + semi/12.0f + inputs[OFFSET_INPUT].getVoltage() ;
			if(changed) pulse.trigger(0.001f);
			last_octave = octave;
			last_semi   = semi;
		
	
	if (gateTrigger1.process(on1.process(gSampleTime) ? 10.0 : 0.0)) {
		sample1 = quantized;
	}
	
outputs[OUT1_OUTPUT].setVoltage(sample1);
	
		if(trigger.process( inputs[TRIG_INPUT].getVoltage() ) ) {
			gate1 = true;
	}
	
		if(gate1) {		
		on1.trigger(0.1);
		gate1 = false;
	}
		
		
if( inputs[VAR_INPUT].isConnected() ) {
	       
		if(gatevTrigger.process(inputs[VAR_INPUT].getVoltage())) {
			gatev = true;
	};

	if(gatev) {	
		on1v.trigger(0.1);
		gatev = false;
	};
	
	var=(on1v.process(gSampleTime) ? 10.0 : 0.0);
	
	if (var){
		outputs[OUT1_OUTPUT].setVoltage(outgl);
	}
	
};

	float in = (sample1);
	float shape = 0.66f;	
	const float slewMin = 0.1;
	const float slewMax = 10000.f;
	const float shapeScale = 1/10.f;
	
			if (in > outgl) {
			float slew = slewMax * std::pow(slewMin / slewMax, 0.5f);
			outgl += slew * crossfade(1.f, shapeScale * (in - outgl), shape) * args.sampleTime;
			if (outgl > in)
				outgl = in;
		}
	
		else if (in < outgl) {
			float slew = slewMax * std::pow(slewMin / slewMax, 0.5);
			outgl -= slew * crossfade(1.f, shapeScale * (outgl - in), shape) * args.sampleTime;
			if (outgl < in)
				outgl = in;
		}
		
		
};
		

struct PitchDiktatWidget : ModuleWidget {
	PitchDiktatWidget(PitchDiktat *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PitchDiktat.svg")));

		addParam(createParam<LFMSwitch>(Vec(25, 32), module, PitchDiktat::SCALE_PARAM));
		addParam(createParam<MS>(Vec(25, 72), module, PitchDiktat::SCALEON_PARAM));
		
		for(int i=0; i<12; i++) {
		addParam(createParam<ButtonLED>(Vec(65,30 + (24*i)), module, PitchDiktat::SEMI1_PARAM + i));
		addChild(createLight<LargeLight<GreenLight>>(Vec(66.4f,30 + (24*i+1.4f)), module, PitchDiktat::SEMI1_LIGHT+i));
		}

		addInput(createInput<JackPort>(Vec(22,105), module, PitchDiktat::PITCH_INPUT));
		addInput(createInput<JackPort>(Vec(22,150), module, PitchDiktat::TRIG_INPUT));
		addInput(createInput<JackPort>(Vec(22,240), module, PitchDiktat::TRANSPOSE_INPUT));
		addInput(createInput<JackPort>(Vec(22,285), module, PitchDiktat::OFFSET_INPUT));
			
		addInput(createInput<JackPort>(Vec(22,195), module, PitchDiktat::VAR_INPUT));

				
		addOutput(createOutput<OutJackPort>(Vec(48, 330), module, PitchDiktat::OUT1_OUTPUT));

	}
	

};

Model *modelPitchDiktat = createModel<PitchDiktat, PitchDiktatWidget>("PitchDiktat");