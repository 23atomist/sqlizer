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
 * the table of oscillators and associated constants.
 **************************************************************/
#define OTYPE_FREE         0    // Not in use
#define OTYPE_SINE         1
#define OTYPE_SQUARE       2
#define OTYPE_TRIANGLE     3
#define OTYPE_NOISE        4
#define OTYPE_WAVETBL      5    // not yet implemented
#define OSCILLATOR_COUNT 100
struct OSCILLATOR
{
    int      idx;              // Index of this oscillator.  0 to OSCILLATOR_COUNT-1
    int      otype;            // Sine, square, triangle, noise, wave table
    float    freq;             // UI element. Used to computer phasestep below
    float    phaseaccumulator; // Phase of output in range 0 to 1
    float    symmetry;         // Symmetry (0 to 1) for sine, square, triangle
    float    phaseoffset;      // Added to accumulator before computing waveform value
    int      vibratoosc;       // Index of oscillator that controls vibrato
    float    vibratofreq;      // Added to phasestep based on vibratoosc (+0 to +1)
    float    glidefreq;        // Target frequency after a glide
    int      glidems;          // Number of milliseconds to take to get to glidefreq
    int      hardsyncosc;      // Index of osc that controls hardsync.  ==-1 if off
    // The following are not brought out to the UI
    float    phasestep;        // Added to phase accumulator each step
    float    vibratostep;      // Phase step corresponding to vibratofreq
    int      glidecount;       // Num samples to apply glidestep, decrement each sample
    float    glidestep;        // Add this to phasestep if glidecount is not zero
    int      sync;             // ==1 for one sample as output crosses zero. 
    float    oscout;           // Output value of oscillator in range of -1 to +1
};

/***************************************************************
 * the amplitude envelopes and associated constants.
 * A gain of -1 indicates that the note is complete.  There are
 * at most 8 steps in the envelope.  The gain linearly ramps from
 * the previous value to the value specified in the table.  The
 * first step always starts with a gain of zero.
 *  Amplitude envelopes are given a name to make reuse a little
 * easier for the UI designer
 **************************************************************/
#define ENVNAME_LEN     20
#define ENVELOPE_COUNT  20
struct AMP_ENVELOPE
{
    int      idx;              // Indes of this envelope.  0 to ENVELOPE_COUNT-1
    char     envname[ENVNAME_LEN]; // Unique name assigned by the UI program
    int      step1time;        // number of milliseconds in step
    float    step1gain;        // gain (0 to 1) at end of step.  -1 to end
    int      step2time;        // number of milliseconds in step
    float    step2gain;        // gain (0 to 1) at end of step.  -1 to end
    int      step3time;        // number of milliseconds in step
    float    step3gain;        // gain (0 to 1) at end of step.  -1 to end
    int      step4time;        // number of milliseconds in step
    float    step4gain;        // gain (0 to 1) at end of step.  -1 to end
    int      step5time;        // number of milliseconds in step
    float    step5gain;        // gain (0 to 1) at end of step.  -1 to end
    int      step6time;        // number of milliseconds in step
    float    step6gain;        // gain (0 to 1) at end of step.  -1 to end
    int      step7time;        // number of milliseconds in step
    float    step7gain;        // gain (0 to 1) at end of step.  -1 to end
    int      step8time;        // number of milliseconds in step
    float    step8gain;        // gain (0 to 1) at end of step.  -1 to end
};

/***************************************************************
 * the table of voices and associated constants.
 **************************************************************/
#define NOTEID_LEN      20
#define CHORDID_LEN     20
#define VSTATE_FREE     0       // Not in use.
#define VSTATE_INUSE    1       // Allocated but not yet on
#define VSTATE_ON       2       // In use and timer is running
#define VSTATE_SUSTAIN  3       // In continuous sustain (eg organ)
#define MIXMODE_SUM     0       // sum osc1 and osc2
#define MIXMODE_AMPMOD  1       // amplitude modulate osc1 by osc2
#define MIXMODE_RINGMOD 2       // ring modulate osc1 and osc2)
                                // sustain (eg organ)
#define VOICE_COUNT     20
struct VOICE
{
    int      idx;              // Index of this voice.  0 to VOICE_COUNT-1
    char     noteid[NOTEID_LEN]; // Unique ID assigned by the UI program
    char     chordid[CHORDID_LEN]; // Unique ID assigned by the UI program
    int      vstate;           // free, inuse, on, sustain, forced release
    int      ontime;           // number of milliseconds the note has played
    int      ampenvidx;        // index of which amplitude envelope is used
    int      osc1;             // index of main oscillator
    float    osc1gain;         // input gain (0 to 1) of osc1
    int      osc2;             // index of secondary oscillator
    float    osc2gain;         // input gain (0 to 1) of osc2
    int      mixmode;          // one of sum, tremolo, or ring
    float    outputgain;       // attenuation at final output
    int      vout;             // output value of voice 
};

/***************************************************************
 * table of UI connections and associated constants
 **************************************************************/
    // Maximum number of UI/Posgres connections
#define MX_UI          (20)
    // Max size of a Postgres packet from/to the UI's
#define MXCMD          (1000)
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

