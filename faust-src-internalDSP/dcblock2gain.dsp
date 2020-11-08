declare name 		"dcblock2gain";
import("filter.lib");
gain      = 0.05; // hslider("gain",0.5, 0, 1, 0.025);
process = dcblocker*(gain),dcblocker*(gain);
