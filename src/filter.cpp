
/* Code automatically generated by Vult https://github.com/modlfo/vult */
#include "filter.h"

float Filter_calc_g(float cv, float sr){
   float pitch;
   pitch = Filter_cvToPitch(cv);
   float f;
   f = (8.17579891564f * expf((0.0577622650467f * pitch)));
   float pi;
   pi = 3.14159265359f;
   float wd;
   wd = (2.f * f * pi);
   float T;
   T = (1.f / sr);
   float wa;
   wa = (tanf((0.5f * T * wd)) * (2.f / T));
   float g;
   g = (0.5f * T * wa);
   return g;
}

void Filter__ctx_type_7_init(Filter__ctx_type_7 &_output_){
   Filter__ctx_type_7 _ctx;
   _ctx.z2 = 0.0f;
   _ctx.z1 = 0.0f;
   _ctx.inv_den = 0.0f;
   _ctx.g = 0.0f;
   Filter__ctx_type_5_init(_ctx._inst6);
   Filter__ctx_type_5_init(_ctx._inst5);
   _ctx.R = 0.0f;
   _output_ = _ctx;
   return ;
}

float Filter_process(Filter__ctx_type_7 &_ctx, float x, float cv, float q, int sel, float sr){
   q = (0.5f + q);
   if(Filter_change(_ctx._inst5,cv) || Filter_change(_ctx._inst6,q)){
      _ctx.g = Filter_calc_g(cv,sr);
      _ctx.R = (1.f / (2.f * (1e-018f + q)));
      _ctx.inv_den = (1.f / (1.f + (2.f * _ctx.R * _ctx.g) + (_ctx.g * _ctx.g)));
   }
   float high;
   high = (_ctx.inv_den * (x + (- _ctx.z2) + (- (_ctx.z1 * (_ctx.g + (2.f * _ctx.R))))));
   float band;
   band = (_ctx.z1 + (_ctx.g * high));
   float low;
   low = (_ctx.z2 + (_ctx.g * band));
   float notch;
   notch = (high + low);
   _ctx.z1 = (band + (_ctx.g * high));
   _ctx.z2 = (low + (_ctx.g * band));
   float output;
   switch(sel) {
      case 0:
         output = low;
      break;
      case 1:
         output = high;
      break;
      case 2:
         output = band;
      break;
    default: 
      output = notch;
   }
   return Filter_tanh_table(output);
}


