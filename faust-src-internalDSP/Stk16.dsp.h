//-----------------------------------------------------
// name: "Stk16"
// version: "1.0"
// author: "CC"
// license: "BSD"
// copyright: "(c)GRAME 2006"
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

#ifndef __Stk16_H__
#define __Stk16_H__

#include <iostream>
#include <unistd.h>
//cc for random()
#include <cstdlib>

// any QT classes
#include <QTimer>

#include "ProcessPlugin.h"
#include "Faust.h"
#include "Delay.h"
#include "OnePole.h"
using namespace stk;
#include <QDebug>
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
#define mydsp Stk16dsp

/*
 * FAUST class
 */
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#define fvCombs 8
#define nDelays fvCombs*2 // stereo
class mydsp : public dsp {
private:
    Delay 	delay[nDelays];
    OnePole 	onePole[nDelays];
    double roomSize_ = 0.85;
public:
    static void metadata(Meta* m) 	{
        m->declare("name", "Stk16");
        m->declare("version", "1.0");
        m->declare("author", "CC");
        m->declare("license", "BSD");
        m->declare("copyright", "(c)GRAME 2006");
    }

    virtual int getNumInputs() 	{ return 16; }
    virtual int getNumOutputs() 	{ return 16; }
    static void classInit(int samplingFreq) {
    }
    virtual void instanceInit(int samplingFreq) {
        fSamplingFreq = samplingFreq;
    }
    void combInit(int delLen, double rs) {
        roomSize_ = rs;
        qDebug() << delLen << roomSize_;
//        int cDelayLengths[] = {1617, 1557, 1491, 1422, 1356, 1277, 1188, 1116}; // so for now, -1100
        int cDelayLengths[] = {517, 457, 391, 322, 256, 177, 88, 16}; // 1100 = 23ms rtt
        for (int i=0; i<nDelays; i++) {
            int tmp = delLen + cDelayLengths[i % fvCombs];
            if (i%2) tmp += 23; // stereoSpread every other channel
//            delay[i].setMaximumDelay( tmp + 1 );
            qDebug() << tmp << i;
            delay[i].setDelay( tmp );
//            delay[i].clear();
            double damp_ = 0.5;
            onePole[i].setCoefficients(1.0 - damp_, -damp_);
//            onePole[i].clear();
        }

    }
    virtual void init(int samplingFreq) {
        classInit(samplingFreq);
        instanceInit(samplingFreq);
    }
//    virtual void buildUserInterface(UI* interface) {
//        interface->openVerticalBox("0x00");
//        interface->closeBox();
//    }
    virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
        FAUSTFLOAT* input0 = input[0];
        FAUSTFLOAT* input1 = input[1];
        FAUSTFLOAT* input2 = input[2];
        FAUSTFLOAT* input3 = input[3];
        FAUSTFLOAT* input4 = input[4];
        FAUSTFLOAT* input5 = input[5];
        FAUSTFLOAT* input6 = input[6];
        FAUSTFLOAT* input7 = input[7];
        FAUSTFLOAT* input8 = input[8];
        FAUSTFLOAT* input9 = input[9];
        FAUSTFLOAT* input10 = input[10];
        FAUSTFLOAT* input11 = input[11];
        FAUSTFLOAT* input12 = input[12];
        FAUSTFLOAT* input13 = input[13];
        FAUSTFLOAT* input14 = input[14];
        FAUSTFLOAT* input15 = input[15];
        FAUSTFLOAT* output0 = output[0];
        FAUSTFLOAT* output1 = output[1];
        FAUSTFLOAT* output2 = output[2];
        FAUSTFLOAT* output3 = output[3];
        FAUSTFLOAT* output4 = output[4];
        FAUSTFLOAT* output5 = output[5];
        FAUSTFLOAT* output6 = output[6];
        FAUSTFLOAT* output7 = output[7];
        FAUSTFLOAT* output8 = output[8];
        FAUSTFLOAT* output9 = output[9];
        FAUSTFLOAT* output10 = output[10];
        FAUSTFLOAT* output11 = output[11];
        FAUSTFLOAT* output12 = output[12];
        FAUSTFLOAT* output13 = output[13];
        FAUSTFLOAT* output14 = output[14];
        FAUSTFLOAT* output15 = output[15];
        for (int i=0; i<count; i++) {
                output0[i] = roomSize_ * onePole[0].tick( delay[0].tick((FAUSTFLOAT)(float)input0[i]));
                output1[i] = roomSize_ * onePole[1].tick( delay[1].tick((FAUSTFLOAT)(float)input1[i]));
                output2[i] = roomSize_ * onePole[2].tick( delay[2].tick((FAUSTFLOAT)(float)input2[i]));
                output3[i] = roomSize_ * onePole[3].tick( delay[3].tick((FAUSTFLOAT)(float)input3[i]));
                output4[i] = roomSize_ * onePole[4].tick( delay[4].tick((FAUSTFLOAT)(float)input4[i]));
                output5[i] = roomSize_ * onePole[5].tick( delay[5].tick((FAUSTFLOAT)(float)input5[i]));
                output6[i] = roomSize_ * onePole[6].tick( delay[6].tick((FAUSTFLOAT)(float)input6[i]));
                output7[i] = roomSize_ * onePole[7].tick( delay[7].tick((FAUSTFLOAT)(float)input7[i]));
                output8[i] = roomSize_ * onePole[8].tick( delay[8].tick((FAUSTFLOAT)(float)input8[i]));
                output9[i] = roomSize_ * onePole[9].tick( delay[9].tick((FAUSTFLOAT)(float)input9[i]));
                output10[i] = roomSize_ * onePole[10].tick( delay[10].tick((FAUSTFLOAT)(float)input10[i]));
                output11[i] = roomSize_ * onePole[11].tick( delay[11].tick((FAUSTFLOAT)(float)input11[i]));
                output12[i] = roomSize_ * onePole[12].tick( delay[12].tick((FAUSTFLOAT)(float)input12[i]));
                output13[i] = roomSize_ * onePole[13].tick( delay[13].tick((FAUSTFLOAT)(float)input13[i]));
                output14[i] = roomSize_ * onePole[14].tick( delay[14].tick((FAUSTFLOAT)(float)input14[i]));
                output15[i] = roomSize_ * onePole[15].tick( delay[15].tick((FAUSTFLOAT)(float)input15[i]));
        }
    }
};




// our jacktrip dsp class, inheriting from ProcessPlugin and Faust dsp
class Stk16 : public ProcessPlugin, public Stk16dsp
{
    Q_OBJECT
public:
    Stk16(int nChans) : mNumChannels(nChans){}
    virtual ~Stk16() {}
    int getNumInputs() {return Stk16dsp::getNumInputs();}
    int getNumOutputs() {return Stk16dsp::getNumOutputs();}
    void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output)
    {Stk16dsp::compute (count, input, output);}
    void instanceInit(int samplingFreq)
    {Stk16dsp::instanceInit (samplingFreq);}
    void initCombClient(int delLen, double rs) {
        Stk16dsp::combInit(delLen, rs);
    };
private:
    int mNumChannels;
};

// always undefine this stuff for compatibility with the rest of the project compilations
#undef private
#undef virtual
#undef mydsp

#endif // __Stk16_H__


