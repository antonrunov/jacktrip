put a FAUST .dsp plugin here

compile with jtDSPpluginsTest1.2.2/jtHeaderMake <name>.dsp to produce <name>.dsp.h

e.g., 
jtDSPpluginsTest1.2.2/jtHeaderMake gainStereo.dsp

FAUST utility classes here are included in the .pro project file, same with a few individual Stk classes

but the file <name>.dsp.h also needs to be added to the project

e.g.,
HEADERS +=  \
            ../faust-src-internalDSP/gainStereo.dsp.h \


to change this behavior, or to compile under a different architecture, make a new
faust2jacktrip/faust2jacktrip
by 
cd faust2jacktrip/
rm faust2jacktrip
make
