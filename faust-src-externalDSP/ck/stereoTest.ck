// chuck stereoTest.ck

class impChan {
  Impulse imp;
  fun void go() {
    while (true) {
      imp.next (1.0);
      1::samp => now;
      imp.next (1.0);
      1::samp => now;
      imp.next (1.0);
      500::ms => now;
    }
  }
  fun void set(int ch) {
    imp => dac.chan(ch);
    spork ~ go();
  }
}
2 => int nChans;
impChan ic[nChans];
for (0 => int i; i < 2; i++) {
  ic[i].set(i);
  250::ms => now;
}
100::second => now;
<<<"done">>>;
