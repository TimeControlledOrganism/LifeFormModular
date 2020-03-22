#include "plugin.hpp"


Plugin *pluginInstance;


void init(Plugin *p) {
	pluginInstance = p;

	p->addModel(modelTimeDiktat);
	p->addModel(modelSequenceModeler);
	p->addModel(modelPitchDiktat);
	p->addModel(modelPitchIntegrator);
	p->addModel(modelBurstIntegrator);
	p->addModel(modelQuadModulator);
	p->addModel(modelImpulseControl);
	p->addModel(modelQuadSteppedOffset);
	p->addModel(modelPercussiveVibration);
	p->addModel(modelQuadUtility);
	p->addModel(modelAdditiveVibration);
	p->addModel(modelComplexOsc);
	p->addModel(modelDriftgen);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
