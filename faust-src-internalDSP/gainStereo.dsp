//======================================================
//
//        gainStereo
//        2ch gain
//
//======================================================

gain = 0.5;

process =  _,_ : (*(gain),*(gain) ): _,_;
