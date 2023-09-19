/***************************************************************
 * voices.c -   Synthesizer occillators, voices, filters, and effects.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <math.h>
#include "sqlizer.h"


/***************************************************************************
 *  - Limits and defines
 ***************************************************************************/
#define  FULLVOLUME ((1 << 15) -1)
#define  NSINES     1000
#define  LFSRINIT   0x11111111   // any non-zero value is good
#define  LFSRPOLY   0x46000000   // polynomial coefficients for lfsr


/***************************************************************************
 *  - Function prototypes and external references
 ***************************************************************************/
void   init_synth();
void   do_synth();
void   do_oscillator(int o);
void   do_voice(int v);
extern struct AMP_ENVELOPE ampenv[ENVELOPE_COUNT];
extern struct OSCILLATOR oscillators[OSCILLATOR_COUNT];
extern struct VOICE voices[VOICE_COUNT];


/***************************************************************************
 *  - File visible variable allocation
 ***************************************************************************/
static int64_t  oldnow;           // microseconds from epoch to previous now
static float    sinetbl[NSINES];  // Sine look-up table. First quadrant only
static uint32_t whitenoise;       // linear feedback shift register


/***************************************************************
 * init_synth(): - Initialize tables and static values
 *
 * Input:
 * Output:
 * Effects:      init index values in tables
 ***************************************************************/
void init_synth()
{
    struct timeval tv;         // "now" as the time since the epoch
    float  angle;              // angle for sine table init
    int    i;

    // Get old now as the number of milliseconds since the Epoch
    if (gettimeofday(&tv, 0) < 0) {
        // LOG(LOG_WARNING, TM, E_No_Date);
        exit(-1);
    }
    oldnow = (((long long) tv.tv_sec) * 1000000) + tv.tv_usec;

    // init the shared white noise generator
    whitenoise = LFSRINIT;

    // init the tables
    for (i = 0; i < ENVELOPE_COUNT; i++) {
        ampenv[i].idx = i;
    }
    for (i = 0; i < OSCILLATOR_COUNT; i++) {
        oscillators[i].idx = i;
        oscillators[i].otype = OTYPE_FREE;
        oscillators[i].freq = 440.0;
        oscillators[i].phaseaccumulator = 0.0;
        oscillators[i].symmetry = 0.5;
        oscillators[i].phasestep = 440.0 / SRATE;
        oscillators[i].phaseoffset = 0.0;
        oscillators[i].glidefreq = 1.0;
        oscillators[i].glidems = 0;
        oscillators[i].glidecount = 0;
        oscillators[i].glidestep = 0.0;
        oscillators[i].vibratoosc = -1;
        oscillators[i].hardsyncosc = -1;
        oscillators[i].sync = 0;
        oscillators[i].oscout = 0.0;
    }
    for (i = 0; i < VOICE_COUNT; i++) {
        voices[i].idx = i;
    }

    // init the sine look-up table (0 to pi/2 radians, 0-90 degrees)
    for (i = 0; i < NSINES; i++) {
        angle = 3.1415926 * (float)i / (2.0 * (float)NSINES);
        sinetbl[i] = sinf(angle);
    }
}


/***************************************************************
 * do_synth(): - Compute the number of samples to output and
 * for each sample update the oscillators and voices.  Write to
 * the output computed sample values.
 *
 * Input:
 * Output:
 * Effects:      does all synth work
 ***************************************************************/
void do_synth()
{
    struct timeval tv;         // "now" as the time since the epoch
    int64_t  now;              // now in microseconds since the epoch
    int64_t  dosamples;        // how many sample to add to the output
    int64_t  s, o, v;          // loop variables for Samples, Osc, Voice

    // Get "now" in milliseconds since the Epoch
    if (gettimeofday(&tv, 0) < 0) {
        // LOG(LOG_WARNING, TM, E_No_Date);
        exit(-1);
    }
    now = (((long long) tv.tv_sec) * 1000000) + tv.tv_usec;

    // We are not really interested in elapsed microseconds.  We want
    // to know how many samples to add to the output.  Roundoff error
    // means this is not just the elapsed seconds times the sample rate
    dosamples = (now * 44100 / 1000000) - (oldnow * 44100 / 1000000);
    oldnow = now;

    // for each sample period ...
    for (s = 0; s < dosamples; s++) {

        // process one sample time for each oscillator
        for (o = 0; o < OSCILLATOR_COUNT; o++) {
            do_oscillator(o);
        }

        // At this point all of the oscillators have been updated
        // so we can now process each of the voices
        for (v = 0; v < VOICE_COUNT; v++) {
            do_voice(v);
        }

        // Sum all voice outputs and send to audio output
char x[2];
int  val;
    val = (int)((float)FULLVOLUME * oscillators[0].oscout);
    x[1] = val & 0x0000ff;
    x[0] = (val>> 8) & 0x0000ff;
        write(1, x, 2);
    }

    return;
}


/***************************************************************
 * do_oscillator(): - Update the specified oscillator to process
 * one sample interval.
 *
 * Phase accumulates by "phasestep" at every sample time.  Phase
 * wraps around if the computed value is below zero or greater than
 * one.  Asymmetric waveforms scale phasestep to a higher or lower
 * value when the accumulated phase is less than or greater than 0.5.
 * The output is the value of the waveform (sine, square, triangle)
 * at the phase angle of the accumulator plus "phaseoffset".
 *
 * As a reminder, if phaseoffset is zero then a square wave goes to
 * one at a phase of zero and to -1 at a phase of 0.5.  Both triangle
 * and sine outputs are zero at a phase of zero, 1 at a phase of 0.25,
 * zero at a phase of 0.5, and -1 at a phase of 0.75.
 *
 * From the structure definition:
     int      idx;              // Index of this oscillator.  0 to OSCILLATOR_COUNT-1
    int      otype;            // Sine, square, triangle, noise, wave table
    float    freq;             // UI element. Used to computer phasestep below
    float    phaseaccumulator; // phase of output in range 0 to 1
    float    symmetry;         // Symmetry (0 to 1) for sine, square, triangle
    float    phaseoffset;      // Added to accumulator before computing waveform value
    int      vibratoosc;       // Index of oscillator that controls vibrato
    float    vibratodepth;     // Added/subtracted to phasestep based on vibratoosc
    float    glidefreq;        // Target frequency after a glide
    int      glidems;          // Number of milliseconds to take to get to glidefreq
    int      hardsyncosc;      // Index of osc that controls hardsync.  ==-1 if off
    // The following are not brought out to the UI
    float    phasestep;        // added to phase accumulator each step
    int      glidecount;       // Num samples to apply glidestep, decrement each sample
    float    glidestep;        // Add this to phasestep if glidecount is not zero
    int      sync;             // ==1 for one sample as output crosses zero.
    float    oscout;           // output value of oscillator in range of -1 to +1
*
 * Input:
 * Output:
 * Effects:      Oscillator output
 ***************************************************************/
void do_oscillator(
    int o)             // index of oscillator to update
{
    struct OSCILLATOR *posc;  // makes code easier to read
    float   phstep;    // the actual value to step the accumulator
    float   phout;     // Sum of accumulator and phasestep
    float   sineidx;   // Index into the sine table as a float

    // update the white noise generator whether or not we use this osc
    if (whitenoise & 0x80000000)
        whitenoise = ((whitenoise << 1) ^ LFSRPOLY) + 1;
    else
        whitenoise = whitenoise << 1;


    posc = &oscillators[o];
    // Check to see if oscillator is on
    if (posc->otype == OTYPE_FREE) {
        return;
    }

    // Adjust phase step based on glide
    // Reset glidems if done
    if (posc->glidecount != 0) {
        posc->phasestep += posc->glidestep;
        posc->glidecount--;
        if (posc->glidecount == 0)
            posc->glidems = 0;
    }

    // Apply vibrato if a valid osc is specified
    phstep = posc->phasestep;
    if ((posc->vibratoosc >= 0)
     && (posc->vibratoosc < OSCILLATOR_COUNT)) {
        phstep += oscillators[posc->vibratoosc].oscout * posc->vibratodepth;
    }

    // Adjust phase step based on symmetry
    if (posc->phaseaccumulator < 0.5) 
        phstep = 0.5 * phstep / posc->symmetry;
    else
        phstep = 0.5 * phstep / (1.0 - posc->symmetry);

    // Adjust the phase of the oscillator.  Subtract floor since phase might > 2.0!
    posc->phaseaccumulator += phstep;
    if (posc->phaseaccumulator > 1.0) {
        posc->phaseaccumulator -= floorf(posc->phaseaccumulator);
        posc->sync = 1;
    }
    else if (posc->phaseaccumulator < 0.0) {
        posc->phaseaccumulator -= floorf(posc->phaseaccumulator);
        posc->sync = 1;
    }
    else {
        posc->sync = 0;
    }

    // The phase accumulator is set.  Add the offset to it to compute output value
    phout = posc->phaseaccumulator + posc->phaseoffset;

    // Hard sync forces the phase to zero if a valid hardsyncosc is set
    if ((posc->hardsyncosc >= 0)
     && (posc->hardsyncosc < OSCILLATOR_COUNT)
     && (oscillators[posc->hardsyncosc].sync == 1)) {
        posc->phaseaccumulator = 0.0;
    }

    // compute output value based on waveform type
    if (posc->otype == OTYPE_SQUARE) {
        if (phout < 0.5)
            posc->oscout = 1.0;
        else
            posc->oscout = -1.0;
    }
    else if (posc->otype == OTYPE_SINE) {
        if (phout < 0.25)
            sineidx = phout * 4.0;     // table is just the first quadrant
        else if (phout < 0.5)
            sineidx = 2.0 - (phout * 4.0);  // goes 1 down to 0
        else if (phout < 0.75)
            sineidx = (phout - 0.5) * 4.0;
        else
            sineidx = 2.0 - ((phout - 0.5) * 4.0);  // goes 1 down to 0
        posc->oscout = sinetbl[(int)((float)(NSINES -1) * sineidx)];
        if (phout > 0.5)
            posc->oscout = -posc->oscout;   // negative in second half of cycle
    }
    else if (posc->otype == OTYPE_TRIANGLE) {
        if (phout < 0.25)
            posc->oscout = phout * 4.0; // max at 0.25
        else if (phout < 0.75)
            posc->oscout = (0.5 - phout) * 4.0; // min at 0.75
        else
            posc->oscout = (phout - 1.0) * 4.0; // min at 0.75
    }
    else if (posc->otype == OTYPE_NOISE) {
        // whitenoise is an unsigned 32 bit integer.  We need to map its value
        // into a float between -1.0 and 1.0.  First to 0-1 then sign using MSB
        posc->oscout = ((float) (whitenoise & 0x7ffffff) / (float)(1 << 27));
        posc->oscout = (whitenoise & 0x8000000) ? -posc->oscout : posc->oscout;
    }
    else if (posc->otype == OTYPE_WAVETBL) {
        posc->oscout = 0.0;
    }
//printf("%d %f %f %08x %f\n", posc->otype, sineidx, phout, whitenoise, posc->oscout);
}


/***************************************************************
 * do_voice(): - Update the specified voice to process one sample
 * interval.
 *
 * Input:
 * Output:
 * Effects:      Voice output
 ***************************************************************/
void do_voice(
    int v)          // index of voice to update
{
}


