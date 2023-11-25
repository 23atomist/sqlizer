/***************************************************************
 * sqlizer.h -- Table structure definitions for a music synthesizer 
 *
 * Copyright:   Copyright (C) 2023 by Atomlab, LLC
 *
 * License:     This program is free software; you can redistribute it and/or
 *              modify it under the terms of the Version 2 of the GNU General
 *              Public License as published by the Free Software Foundation.
 *              GPL2.txt in the docs directory is a copy of this license.
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 **************************************************************/
#include "librta.h"


// Sample rate.
#define SRATE 44100.0




/***************************************************************
 * the table of voices and associated constants.
 **************************************************************/
#define NOTEID_LEN         20      // UI annotation for a voice
#define CHORDID_LEN        20      // Used to turn all chord notes on at once
#define VSTATE_FREE        0       // Not in use.
#define VSTATE_INUSE       1       // Allocated but not yet on
#define VSTATE_ON          2       // In use and timer is running
#define VSTATE_SUSTAIN     3       // In continuous sustain (eg organ)
#define OTYPE_OFF          0       // Not in use
#define OTYPE_SINE         1
#define OTYPE_SQUARE       2
#define OTYPE_TRIANGLE     3
#define OTYPE_NOISE        4
#define OTYPE_WAVETBL      5       // not yet implemented
#define MIXMODE_NONE       0       // no mixing
#define MIXMODE_SUM        1       // sum osc1 and osc2
#define MIXMODE_AM         2       // amplitude modulate osc1 by osc2
#define MIXMODE_FM         3       // frequency modulate osc1 by osc2
#define MIXMODE_RING       4       // ring modulate osc1 and osc2)
#define MIXMODE_HARDSYNC   5       // force o1 phase to zero when o2 is 0
#define FILT_OFF           0       // voice filter is not used
#define FILT_LOW           1       // voice filter is low pass
#define FILT_HIGH          2       // voice filter is high pass
#define FILT_BAND          3       // voice filter is band pass
#define FILT_NOTCH         4       // voice filter is notch
#define MXADSRSTEP         7       // 8 ADSR steps in range of 0 to 7
#define VOICE_COUNT        20
#define SUSTAINVALUE       60000   // sustain if step time is one minute

struct VOICE
{
    int      idx;              // Index of this voice.  0 to VOICE_COUNT-1
    char     noteid[NOTEID_LEN]; // Unique ID assigned by the UI program
    char     chordid[CHORDID_LEN]; // Unique ID assigned by the UI program
    int      vstate;           // free, inuse, on, sustain, forced release
    int      ontime;           // number of sample ticks the note has played (not milliseconds)
    int      o1type;           // Sine, square, triangle, noise, wave table
    float    o1freq;           // Oscillator #1 frequency in range of 0.001 to 20000
    float    o1phasestep;      // Oscillator #1 phase step each sample
    float    o1phaseacc;       // Phase of output in range 0 to 1
    float    o1symmetry;       // Symmetry (0 to 1) for sine, square, triangle
    float    o1phaseoffset;    // Added to accumulator before computing waveform value
    float    o1gain;           // Output gain of oscillator #1
    float    o1out;            // Output value of oscillator #1
    // Vibrato and glide only affect oscillator #1
    int      vibtype;          // Vibrato waveform type (sine, square, ....)
    float    vibfreq;          // Vibrato frequency in range of 0.001 to 20000 Hz
    float    vibphasestep;     // Vibrato phase step each sample
    float    vibphaseacc;      // Vibrato phase in cycle as number in range 0 to 1
    float    vibsymmetry;      // Symmetry (0 to 1) for sine, square, triangle
    float    vibphaseoffset;   // Added to vib phase accumulator before computing waveform value
    float    vibdepth;         // A frequency to be added, at maximum, to o1 freq
    float    vibo1phase;       // Phase offset corresponding to vibdepth
    float    vibout;           // vib osc output.  This times vibo1phase is added to o1 phase step
    float    glidefreq;        // Target o1 frequency after a glide
    int      glidems;          // Number of milliseconds to take to get to glidefreq
    float    glidestep;        // Add this to phasestep if glidecount is not zero
    int      glidecount;       // Num samples to apply glidestep, decrement each sample
    int      o2type;           // Sine, square, triangle, noise, wave table
    float    o2freq;           // Oscillator #2 frequency in range of 0.001 to 20000
    float    o2phasestep;      // Oscillator #2 phase step each sample
    float    o2phaseacc;       // Phase of output in range 0 to 1
    float    o2symmetry;       // Symmetry (0 to 1) for sine, square, triangle
    float    o2phaseoffset;    // Added to accumulator before computing waveform value
    float    o2gain;           // Output gain of oscillator #2
    float    o2out;            // Output of oscillator #2
    int      mixmode;          // Mix o1 and o2 with sum, AM, FM, ring, hardsync, or none
    // Tremolo, filters, and amplitude ADSR affect the o1/o2 mixed signal
    int      tremtype;         // Tremolo waveform type (sine, square, ....)
    float    tremfreq;         // Tremolo frequency in range of 0.001 to 20000 Hz
    float    tremphasestep;    // Tremolo phase step each sample
    float    tremphaseacc;     // Tremolo phase in cycle as number in range 0 to 1
    float    tremdepth;        // A gain (0-1) to be applied to o1/o2 mix based on tremolo
    float    tremsymmetry;     // Symmetry (0 to 1) for sine, square, triangle
    float    tremphaseoffset;  // Added to tremolo phase acc before computing waveform value
    float    tremout;          // Output of the tremolo oscillator
    int      filttype;         // High pass, low pass, notch, band pass
    float    filtfreq;         // center frequency or 3db cutoff frequency
    int      filtrolloff;      // Filter rolloff.  Must be either 6 or 12
    float    q;                // Filter quality factor
    // An ADSR gain of 0 indicates that the note is complete.  There are
    // at most 8 steps in the envelope.  The gain linearly ramps from
    // the previous value to the value specified in the table.  The
    // first step always starts with a gain of zero.
    // The adsridx is an integer between 0 and 7 and is the current step
    // in the ADSR envelope.
    // A time value 60000 (1 minute) is the trigger for SUSTAIN.  This 
    // value will change vstate to VSTATE_SUSTAIN without incrementing 
    // adsridx.  The voice does no ADSR processing while in the suspend
    // state.  ADSR processing resumes when state is set again to ON by
    // the user.
    int      adsridx;          // current location in ADSR envelope
    int      step0time;        // ADSR number of milliseconds in step
    int      step1time;        // ADSR number of milliseconds in step
    int      step2time;        // ADSR number of milliseconds in step
    int      step3time;        // ADSR number of milliseconds in step
    int      step4time;        // ADSR number of milliseconds in step
    int      step5time;        // ADSR number of milliseconds in step
    int      step6time;        // ADSR number of milliseconds in step
    int      step7time;        // ADSR number of milliseconds in step
    float    step0gain;        // gain (0 to 1) at end of step. 0 to end
    float    step1gain;        // gain (0 to 1) at end of step. 0 to end
    float    step2gain;        // gain (0 to 1) at end of step. 0 to end
    float    step3gain;        // gain (0 to 1) at end of step. 0 to end
    float    step4gain;        // gain (0 to 1) at end of step. 0 to end
    float    step5gain;        // gain (0 to 1) at end of step. 0 to end
    float    step6gain;        // gain (0 to 1) at end of step. 0 to end
    float    step7gain;        // gain (0 to 1) at end of step. 0 to end

    float    outputgain;       // final gain applied after ADSR and filter
    // outputs
    int      sync;             // ==1 for one sample as output crosses zero.
    float    voiceout;         // intermediate value of voice output
    int      vout;             // integer version of final voiceout
};


/***************************************************************
 * table of UI connections and associated constants
 **************************************************************/
    // Maximum number of UI/Posgres connections
#define MX_UI          (20)
    // Max size of a Postgres packet from/to the UI's
#define MXCMD          (5000)
#define MXRSP          (50000)
typedef struct ui
{
    struct ui *prevconn;       // Points to previous conn in linked list
    struct ui *nextconn;       // Points to next conn in linked list
    int      fd;               // FD of TCP conn (=-1 if not in use)
    int      cmdindx;          // Index of next location in cmd buffer
    char     cmd[MXCMD];       // SQL command from UI program
    int      rspfree;          // Number of free bytes in rsp buffer
    char     rsp[MXRSP];       // SQL response to the UI // program
    int      o_port;           // Other-end TCP port number
    int      o_ip;             // Other-end IP address
    llong    nbytin;           // number of bytes read in
    llong    nbytout;          // number of bytes sent out
    int      ctm;              // connect time (==time();)
    int      cdur;             // duration time (== now()-ctm;)
} UI;

