declare name        "ap8x2";
declare version     "1.0";
declare author      "Grame";
declare license     "BSD";
declare copyright   "(c) GRAME 2006";
declare reference   "https://ccrma.stanford.edu/~jos/pasp/Freeverb.html";

//======================================================
//
//                      Freeverb
//        Faster version using fixed delays (20% gain)
//
//======================================================

// Constant Parameters
//--------------------

fixedgain   = 0.015; //value of the gain of fxctrl
scalewet    = 3.0;
scaledry    = 2.0;
scaledamp   = 0.4;
scaleroom   = 0.28;
offsetroom  = 0.7;
initialroom = 0.5;
initialdamp = 0.5;
initialwet  = 1.0/scalewet;
initialdry  = 0;
initialwidth= 1.0;
initialmode = 0.0;
freezemode  = 0.5;
stereospread= 23;
allpassfeed = 0.5; //feedback of the delays used in allpass filters


// Filter Parameters
//------------------

combtuningL1    = 1116;
combtuningL2    = 1188;
combtuningL3    = 1277;
combtuningL4    = 1356;
combtuningL5    = 1422;
combtuningL6    = 1491;
combtuningL7    = 1557;
combtuningL8    = 1617;

allpasstuningL1 = 556;
allpasstuningL2 = 441;
allpasstuningL3 = 341;
allpasstuningL4 = 225;


// Control Sliders
//--------------------
// Damp : filters the high frequencies of the echoes (especially active for great values of RoomSize)
// RoomSize : size of the reverberation room
// Dry : original signal
// Wet : reverberated signal

dampSlider      = hslider("Damp",0.5, 0, 1, 0.025)*scaledamp;
roomsizeSlider  = hslider("RoomSize", 0.5, 0, 1, 0.025)*scaleroom + offsetroom;
wetSlider       = 1.0; // hslider("Wet", 0.3333, 0, 1, 0.025);
combfeed        = roomsizeSlider;


// Comb and Allpass filters
//-------------------------

allpass(dt,fb) = (_,_ <: (*(fb),_:+:@(dt)), -) ~ _ : (!,_);

comb(dt, fb, damp) = (+:@(dt)) ~ (*(1-damp) : (+ ~ *(damp)) : *(fb));


// Reverb components
//------------------
srScl = 48000/44100; // original tunings were for 44.1kHz
newTune(fv) = fv * srScl;

monoReverb(fb1, fb2, damp, spread)
    = 
            allpass (newTune(allpasstuningL1+spread), fb2)
        :   allpass (newTune(allpasstuningL2+spread), fb2)
        :   allpass (newTune(allpasstuningL3+spread), fb2)
        :   allpass (newTune(allpasstuningL4+spread), fb2)
        : *(5.0); // gain tuned by ear

stereoReverb(fb1, fb2, damp, spread)
    = monoReverb(fb1, fb2, damp, 0), monoReverb(fb1, fb2, damp, spread);


// fxctrl : add an input gain and a wet-dry control to a stereo FX
//----------------------------------------------------------------

fxctrl(g,w,Fx) =  _,_ <: (*(g),*(g) : Fx : *(w),*(w)), *(1-w), *(1-w) +> _,_;


// Freeverb
//---------

freeverb = vgroup("Freeverb", fxctrl(fixedgain, wetSlider, stereoReverb(combfeed, allpassfeed, dampSlider, stereospread)));

process = freeverb;
