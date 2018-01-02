#ifndef _PULSE_TIMER_H_
#define _PULSE_TIMER_H_


struct pulse_sensor
{
 
int BPM;                   // used to hold the pulse rate
int Signal;                // holds the incoming raw data
int IBI;           // holds the time between beats, must be seeded! 
bool Pulse;     // true when pulse wave is high, false when it's low
bool QS;        // becomes true when Arduoino finds a beat.
int rate[10];                    // array to hold last ten IBI values
unsigned long sampleCounter;          // used to determine pulse timing
unsigned long lastBeatTime;           // used to find IBI
int P;                      // used to find peak in pulse wave, seeded
int T;                     // used to find trough in pulse wave, seeded
int thresh;                // used to find instant moment of heart beat, seeded
int amp;                   // used to hold amplitude of pulse waveform, seeded
int Num;
bool firstBeat;        // used to seed rate array so we startup with reasonable BPM
bool secondBeat;      // used to seed rate array so we startup with reasonable BPM
char rtt_time[64];	
};

struct pulse_sensor PULSESENSOR;


void pulsesensor_init(void);
void pulse_timer_init(void);
void PULSE_PROCESS(void * pvParameters);
#endif