//-----------------------------------------------------
// name: "dcblock2gain"
//
// Code generated with Faust 0.9.73 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */
//
// notes from CC
// see faust2jacktrip
// this file generated from jacktrip_faust.template
// use make to update the template
// and use jtHeaderMake <file>.dsp to generate this file

#ifndef __dcblock2gain_H__
#define __dcblock2gain_H__

#include <iostream>
#include <unistd.h>
//cc for random()
#include <cstdlib>

// any QT classes
#include <QTimer>

#include "ProcessPlugin.h"
#include "Faust.h"

/*
 * FAUST intrinsic
 */

/*
 * FAUST defines UI values as private, but provides no getters/setters.
 * In our particular case it's way more convenient to access them directly
 * than to set up a complicated UI structure.  Also get rid of everything
 * being "virtual", since it may stop the compiler from inlining properly!
 */
#define private public
#define virtual

// definition of derived dsp class

/* Rename the class the name of our DSP. */
#define mydsp dcblock2gaindsp

/*
 * FAUST class
 */
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
	float 	fVec0[2];
	float 	fRec0[2];
	float 	fVec1[2];
	float 	fRec1[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "dcblock2gain");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/version", "1.29");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() 	{ return 2; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
//	virtual void buildUserInterface(UI* interface) {
//		interface->openVerticalBox("0x00");
//		interface->closeBox();
//	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			float fTemp0 = (float)input0[i];
			fVec0[0] = fTemp0;
			fRec0[0] = ((fVec0[0] + (0.995f * fRec0[1])) - fVec0[1]);
			output0[i] = (FAUSTFLOAT)(0.05f * fRec0[0]);
			float fTemp1 = (float)input1[i];
			fVec1[0] = fTemp1;
			fRec1[0] = ((fVec1[0] + (0.995f * fRec1[1])) - fVec1[1]);
			output1[i] = (FAUSTFLOAT)(0.05f * fRec1[0]);
			// post processing
			fRec1[1] = fRec1[0];
			fVec1[1] = fVec1[0];
			fRec0[1] = fRec0[0];
			fVec0[1] = fVec0[0];
		}
	}
};




// our jacktrip dsp class, inheriting from ProcessPlugin and Faust dsp
class dcblock2gain : public ProcessPlugin, public dcblock2gaindsp
{
    Q_OBJECT
public:
    dcblock2gain(int nChans) : mNumChannels(nChans){}
    virtual ~dcblock2gain() {}
    int getNumInputs() {return dcblock2gaindsp::getNumInputs();}
    int getNumOutputs() {return dcblock2gaindsp::getNumOutputs();}
    void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output)
      {dcblock2gaindsp::compute (count, input, output);}
    void instanceInit(int samplingFreq)
    {dcblock2gaindsp::instanceInit (samplingFreq);}
private:
    int mNumChannels;
};

// always undefine this stuff for compatibility with the rest of the project compilations
#undef private
#undef virtual
#undef mydsp

  #endif // __dcblock2gain_H__


