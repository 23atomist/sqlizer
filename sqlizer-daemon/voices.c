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
#define  LFSRINIT   0x11111111   // any non-zero value is good random seed
#define  LFSRPOLY   0x46000000   // polynomial coefficients for lfsr


/***************************************************************************
 *  - Function prototypes and external references
 ***************************************************************************/
void   init_synth();
void   do_synth();
void   do_voice(int v);
extern struct VOICE voices[VOICE_COUNT];


/***************************************************************************
 *  - Variable allocation for this file
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
    for (i = 0; i < VOICE_COUNT; i++) {
        voices[i].idx = i;
        voices[i].noteid[0] = (char) 0;
        voices[i].chordid[0] = (char) 0;
        voices[i].vstate = VSTATE_FREE;
        voices[i].ontime = 0;
        voices[i].o1type = OTYPE_OFF;
        voices[i].o1freq = 440.0;
        voices[i].o1phasestep = 0.0;
        voices[i].o1phaseacc = 0.0;
        voices[i].o1symmetry = 0.5;
        voices[i].o1phaseoffset = 0.0;
        voices[i].o1gain = 0.0;
        voices[i].vibtype = OTYPE_OFF;
        voices[i].vibfreq = 0.0;
        voices[i].vibo1phase = 0.0;
        voices[i].vibphasestep = 0.0;
        voices[i].vibdepth = 0.0;
        voices[i].vibsymmetry = 0.5;
        voices[i].vibphaseoffset = 0.0;
        voices[i].glidefreq = 0.0;
        voices[i].glidems = 0;
        voices[i].glidecount = 0;
        voices[i].glidestep = 0.0;
        voices[i].o2type = OTYPE_OFF;
        voices[i].o2freq = 440.0;
        voices[i].o2phasestep = 0.0;
        voices[i].o2phaseacc = 0.0;
        voices[i].o2symmetry = 0.5;
        voices[i].o2phaseoffset = 0.0;
        voices[i].o2gain = 0.0;
        voices[i].mixmode = MIXMODE_NONE;
        voices[i].tremtype = OTYPE_OFF;
        voices[i].tremfreq = 0.0;
        voices[i].tremphasestep = 0.0;
        voices[i].tremdepth = 0.0;
        voices[i].tremsymmetry = 0.5;
        voices[i].tremphaseoffset = 0.0;
        voices[i].filttype = FILT_OFF;
        voices[i].filtfreq = 0.0;
        voices[i].filtrolloff = 6;
        voices[i].q = 1.0;
        voices[i].adsridx = 0;
        voices[i].step0time = 1;
        voices[i].step1time = 60000;
        voices[i].step2time = 1;
        voices[i].step3time = 1;
        voices[i].step4time = 1;
        voices[i].step5time = 1;
        voices[i].step6time = 1;
        voices[i].step7time = 1;
        voices[i].step0gain = 1.0;
        voices[i].step1gain = 1.0;
        voices[i].step2gain = 1.0;
        voices[i].step3gain = 1.0;
        voices[i].step4gain = 1.0;
        voices[i].step5gain = 1.0;
        voices[i].step6gain = 1.0;
        voices[i].step7gain = 1.0;
        voices[i].flttype  = FILT_OFF;
        voices[i].fltq = 1.0;
        voices[i].fltrolloff = 6;
        voices[i].fltf1 = 440;
        voices[i].flt1b0 = 1.0;
        voices[i].flt1b1 = 0.0;
        voices[i].flt1b2 = 0.0;
        voices[i].flt1a1 = 0.0;
        voices[i].flt1a2 = 0.0;
        voices[i].flt1in1 = 0.0;
        voices[i].flt1in2 = 0.0;
        voices[i].flt1out0 = 0.0;
        voices[i].flt1out1 = 0.0;
        voices[i].flt1out2 = 0.0;
        voices[i].fltf2 = 440;
        voices[i].flt2b0 = 1.0;
        voices[i].flt2b1 = 0.0;
        voices[i].flt2b2 = 0.0;
        voices[i].flt2a1 = 0.0;
        voices[i].flt2a2 = 0.0;
        voices[i].flt2in1 = 0.0;
        voices[i].flt2in2 = 0.0;
        voices[i].flt2out0 = 0.0;
        voices[i].flt2out1 = 0.0;
        voices[i].flt2out2 = 0.0;

        voices[i].sync = 0;
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
    int64_t  s, v;             // loop variables for Samples, Voice
    float    sampleoutput;     // output for one sample

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
    sampleoutput = 0.0;
    for (s = 0; s < dosamples; s++) {
        // process each of the voices
        for (v = 0; v < VOICE_COUNT; v++) {
            do_voice(v);
            sampleoutput += voices[v].voiceout;
        }

        // send to audio output
        static int val = 0;
        char x[2];
        val = (int) (voices[0].voiceout * (float)FULLVOLUME);
        x[1] = val & 0x0000ff;
        x[0] = (val >> 8) & 0x0000ff;
        write(1, x, 2);
    }

    return;
}


/***************************************************************
 * do_voice(): - Update the specified voice to process
 * one sample interval.
 *
 * The oscillators phase accumulates by "oXphasestep" at every
 * sample time.  Phase wraps around if the computed value is below
 * zero or greater than one.
 * Asymmetry scales the phasestep to a higher or lower value when
 * the accumulated phase is less than or greater than the symmetry
 * value.
 * The output of the oscillator is the value of the waveform (sine,
 * square, triangle) at the phase angle of the phase accumulator
 * plus any "phaseoffset".
 *
 * Input:        index of voice to process
 * Output:       output value of the voice in range -1 to +1
 * Effects:      internal voice state
 ***************************************************************/
void do_voice(
    int v)             // index of voice to update
{
    struct  VOICE  *pvoc;  // makes code easier to read
    float   phstep;    // the actual value to step the accumulator
    float   phout;     // Sum of accumulator and phasestep
    float   sineidx;   // Index into the sine table as a float
    float   prevgain;  // Gain of previous ADSR step
    float   targetgain; // Target gain in current ADSR step
    int     steptime;  // duration of this step in milliseconds
    int     ontimems;  // pvoc->ontime in ms instead of sample ticks
    float   flt2input; // filter #2 input == Filter #1 out or same input as #1

    // update the white noise generator whether or not we use this voice
    if (whitenoise & 0x80000000)
        whitenoise = ((whitenoise << 1) ^ LFSRPOLY) + 1;
    else
        whitenoise = whitenoise << 1;

    pvoc = &voices[v];

    // Check to see if voice is in use
    if ((pvoc->vstate == VSTATE_FREE) || (pvoc->vstate == VSTATE_INUSE)) {
        return;
    }

    // oscillator #2 affects oscillator #1 if they are to be mixed.
    if (pvoc->mixmode != MIXMODE_NONE) {
        phstep = pvoc->o2phasestep;

        // Adjust o2 phase step based on symmetry
        if (pvoc->o2phaseacc < 0.5) 
            phstep = 0.5 * phstep / (1.0 - pvoc->o2symmetry);
        else
            phstep = 0.5 * phstep / pvoc->o2symmetry;

        // Adjust the phase of the oscillator.  Subtract floor since phase might > 2.0!
        pvoc->o2phaseacc += phstep;
        if (pvoc->o2phaseacc > 1.0) {
            pvoc->o2phaseacc -= floorf(pvoc->o2phaseacc);
            pvoc->sync = 1;
        }
        else
            pvoc->sync = 0;

        // The o2 phase accumulator is set.  Add offset to compute output value
        phout = pvoc->o2phaseacc + pvoc->o2phaseoffset;
        if (phout > 1.0) {
            phout -= floorf(phout);
        }

        // compute o2 output value based on waveform type
        if (pvoc->o2type == OTYPE_SQUARE) {
            if (phout < 0.5)
                pvoc->o2out = 1.0;
            else
                pvoc->o2out = -1.0;
        }
        else if (pvoc->o2type == OTYPE_SINE) {
            if (phout < 0.25)
                sineidx = phout * 4.0;     // table is just the first quadrant
            else if (phout < 0.5)
                sineidx = 2.0 - (phout * 4.0);  // goes 1 down to 0
            else if (phout < 0.75)
                sineidx = (phout - 0.5) * 4.0;
            else
                sineidx = 2.0 - ((phout - 0.5) * 4.0);  // goes 1 down to 0
            pvoc->o2out = sinetbl[(int)((float)(NSINES -1) * sineidx)];
            if (phout > 0.5)
                pvoc->o2out = -pvoc->o2out;   // negative in second half of cycle
        }
        else if (pvoc->o2type == OTYPE_TRIANGLE) {
            if (phout < 0.25)
                pvoc->o2out = phout * 4.0;
            else if (phout < 0.75)
                pvoc->o2out = 2.0 - (phout * 4.0);
            else
                pvoc->o2out = (phout * 4.0) + -4.0;
        }
        else if (pvoc->o2type == OTYPE_NOISE) {
            // whitenoise is an unsigned 32 bit integer.  We need to map its value
            // into a float between -1.0 and 1.0.  First to 0-1 then sign using MSB
            pvoc->o2out = ((float) (whitenoise & 0x7ffffff) / (float)(1 << 27));
            pvoc->o2out = (whitenoise & 0x8000000) ? -pvoc->o2out : pvoc->o2out;
        }
        // else wavetable, which is not implemented
        else
            pvoc->o2out = 0.0;

        // Apply gain to oscillator #2 output
        pvoc->o2out = pvoc->o2out * pvoc->o2gain;
    }

    // Compute vibrato as an adjustment to the o1 phase step
    if ((pvoc->vibtype != OTYPE_OFF) && (pvoc->vibtype != OTYPE_WAVETBL)) {
        phstep = pvoc->vibphasestep;

        // Adjust vib phase step based on symmetry
        if (pvoc->vibphaseacc < 0.5) 
            phstep = 0.5 * phstep / (1.0 - pvoc->vibsymmetry);
        else
            phstep = 0.5 * phstep / pvoc->vibsymmetry;

        // Adjust the phase of the oscillator.  Subtract floor since phase might > 2.0!
        pvoc->vibphaseacc += phstep;
        if (pvoc->vibphaseacc > 1.0) {
            pvoc->vibphaseacc -= floorf(pvoc->vibphaseacc);
        }

        // The vib phase accumulator is set.  Add offset to compute output value
        phout = pvoc->vibphaseacc + pvoc->vibphaseoffset;
        if (phout > 1.0) {
            phout -= floorf(phout);
        }

        // compute vib output value based on waveform type
        if (pvoc->vibtype == OTYPE_SQUARE) {
            if (phout < 0.5)
                pvoc->vibout = 1.0;
            else
                pvoc->vibout = -1.0;
        }
        else if (pvoc->vibtype == OTYPE_SINE) {
            if (phout < 0.25)
                sineidx = phout * 4.0;     // table is just the first quadrant
            else if (phout < 0.5)
                sineidx = 2.0 - (phout * 4.0);  // goes 1 down to 0
            else if (phout < 0.75)
                sineidx = (phout - 0.5) * 4.0;
            else
                sineidx = 2.0 - ((phout - 0.5) * 4.0);  // goes 1 down to 0
            pvoc->vibout = sinetbl[(int)((float)(NSINES -1) * sineidx)];
            if (phout > 0.5)
                pvoc->vibout = -pvoc->vibout;   // negative in second half of cycle
        }
        else if (pvoc->vibtype == OTYPE_TRIANGLE) {
            if (phout < 0.25)
                pvoc->vibout = phout * 4.0;
            else if (phout < 0.75)
                pvoc->vibout = 2.0 - (phout * 4.0);
            else
                pvoc->vibout = (phout * 4.0) + -4.0;
        }
        else if (pvoc->vibtype == OTYPE_NOISE) {
            // whitenoise is an unsigned 32 bit integer.  We need to map its value
            // into a float between -1.0 and 1.0.  First to 0-1 then sign using MSB
            pvoc->vibout = ((float) (whitenoise & 0x7ffffff) / (float)(1 << 27));
            pvoc->vibout = (whitenoise & 0x8000000) ? -pvoc->vibout : pvoc->vibout;
        }
        else   // should not get here
            pvoc->vibout = 0.0;
    }

    // Adjust oscillator #1 phase step based on glide
    if (pvoc->glidecount != 0) {
        pvoc->o1phasestep += pvoc->glidestep;
        pvoc->glidecount--;
        // If done, reset glidems and set phase step to correct value
        if (pvoc->glidecount == 0) {
            pvoc->glidems = 0;
            pvoc->o1phasestep = pvoc->glidefreq / SRATE;
        }
    }
    // Compute osc #1 phase based on vibrato, osc #2, and symmetry
    if (pvoc->vibtype == OTYPE_OFF) {
        phstep = pvoc->o1phasestep;
    } else {
        phstep = pvoc->o1phasestep + (pvoc->vibo1phase * pvoc->vibout);
        if (phstep > 1.0) {
            phstep -= floorf(phstep);
        }
    }
    // Adjust o1 phase based on FM mixing and osc #2 output
    if ((pvoc->o2type != OTYPE_OFF) && (pvoc->mixmode == MIXMODE_FM)) {
        phstep = phstep + (pvoc->o1phasestep * pvoc->o2out);
        if (phstep > 1.0) {
            phstep -= floorf(phstep);
        }
    }
    // Adjust o1 phase step based on symmetry
    if (pvoc->o1phaseacc < 0.5) 
        phstep = 0.5 * phstep / (1.0 - pvoc->o1symmetry);
    else
        phstep = 0.5 * phstep / pvoc->o1symmetry;

    // Adjust the phase of the oscillator.  Subtract floor since phase might > 2.0!
    pvoc->o1phaseacc += phstep;
    if (pvoc->o1phaseacc > 1.0) {
        pvoc->o1phaseacc -= floorf(pvoc->o1phaseacc);
    }

    // The o1 phase accumulator is set.  Add offset to compute output value
    phout = pvoc->o1phaseacc + pvoc->o1phaseoffset;
    if (phout > 1.0) {
        phout -= floorf(phout);
    }

    // Hard sync forces the phase to zero if enabled and osc #2 crosses zero
    if ((pvoc->sync == 1) && (pvoc->mixmode == MIXMODE_HARDSYNC)) {
        pvoc->o1phaseacc = 0.0;
    }

    // compute o1 output value based on waveform type
    if (pvoc->o1type == OTYPE_SQUARE) {
        if (phout < 0.5)
            pvoc->o1out = 1.0;
        else
            pvoc->o1out = -1.0;
    }
    else if (pvoc->o1type == OTYPE_SINE) {
        if (phout < 0.25)
            sineidx = phout * 4.0;     // table is just the first quadrant
        else if (phout < 0.5)
            sineidx = 2.0 - (phout * 4.0);  // goes 1 down to 0
        else if (phout < 0.75)
            sineidx = (phout - 0.5) * 4.0;
        else
            sineidx = 2.0 - ((phout - 0.5) * 4.0);  // goes 1 down to 0
        pvoc->o1out = sinetbl[(int)((float)(NSINES -1) * sineidx)];
        if (phout > 0.5)
            pvoc->o1out = -pvoc->o1out;   // negative in second half of cycle
    }
    else if (pvoc->o1type == OTYPE_TRIANGLE) {
            if (phout < 0.25)
                pvoc->o1out = phout * 4.0;
            else if (phout < 0.75)
                pvoc->o1out = 2.0 - (phout * 4.0);
            else
                pvoc->o1out = (phout * 4.0) + -4.0;
    }
    else if (pvoc->o1type == OTYPE_NOISE) {
        // whitenoise is an unsigned 32 bit integer.  We need to map its value
        // into a float between -1.0 and 1.0.  First to 0-1 then sign using MSB
        pvoc->o1out = ((float) (whitenoise & 0x7ffffff) / (float)(1 << 27));
        pvoc->o1out = (whitenoise & 0x8000000) ? -pvoc->o1out : pvoc->o1out;
    }
    else   // should not get here
        pvoc->o1out = 0.0;
    // apply the gain for osc #1
    pvoc->o1out = pvoc->o1out * pvoc->o1gain;


    // Mix oscillator #1 and oscillator #2
    if (pvoc->mixmode == MIXMODE_SUM) {
        pvoc->voiceout = pvoc->o1out + pvoc->o2out;
    }
    else if (pvoc->mixmode == MIXMODE_AM) {
        pvoc->voiceout = pvoc->o1out * (pvoc->o2out + 1.0);
    }
    else if (pvoc->mixmode == MIXMODE_RING) {
        pvoc->voiceout = pvoc->o1out * pvoc->o2out;
    }
    else   // mixmode == MIXMODE_NONE
        pvoc->voiceout = pvoc->o1out;


    // Compute tremolo as an adjustment to the mixed signal amplitude
    if ((pvoc->tremtype != OTYPE_OFF) && (pvoc->tremtype != OTYPE_WAVETBL)) {
        phstep = pvoc->tremphasestep;

        // Adjust trem phase step based on symmetry
        if (pvoc->tremphaseacc < 0.5) 
            phstep = 0.5 * phstep / (1.0 - pvoc->tremsymmetry);
        else
            phstep = 0.5 * phstep / pvoc->tremsymmetry;

        // Adjust the phase of the oscillator.  Subtract floor since phase might > 2.0!
        pvoc->tremphaseacc += phstep;
        if (pvoc->tremphaseacc > 1.0) {
            pvoc->tremphaseacc -= floorf(pvoc->tremphaseacc);
        }

        // The trem phase accumulator is set.  Add offset to compute output value
        phout = pvoc->tremphaseacc + pvoc->tremphaseoffset;
        if (phout > 1.0) {
            phout -= floorf(phout);
        }

        // compute trem output value based on waveform type
        if (pvoc->tremtype == OTYPE_SQUARE) {
            if (phout < 0.5)
                pvoc->tremout = 1.0;
            else
                pvoc->tremout = -1.0;
        }
        else if (pvoc->tremtype == OTYPE_SINE) {
            if (phout < 0.25)
                sineidx = phout * 4.0;     // table is just the first quadrant
            else if (phout < 0.5)
                sineidx = 2.0 - (phout * 4.0);  // goes 1 down to 0
            else if (phout < 0.75)
                sineidx = (phout - 0.5) * 4.0;
            else
                sineidx = 2.0 - ((phout - 0.5) * 4.0);  // goes 1 down to 0
            pvoc->tremout = sinetbl[(int)((float)(NSINES -1) * sineidx)];
            if (phout > 0.5)
                pvoc->tremout = -pvoc->tremout;   // negative in second half of cycle
        }
        else if (pvoc->tremtype == OTYPE_TRIANGLE) {
            if (phout < 0.25)
                pvoc->tremout = phout * 4.0;
            else if (phout < 0.75)
                pvoc->tremout = 2.0 - (phout * 4.0);
            else
                pvoc->tremout = (phout * 4.0) + -4.0;
        }
        else if (pvoc->tremtype == OTYPE_NOISE) {
            // whitenoise is an unsigned 32 bit integer.  We need to map its value
            // into a float between -1.0 and 1.0.  First to 0-1 then sign using MSB
            pvoc->tremout = ((float) (whitenoise & 0x7ffffff) / (float)(1 << 27));
            pvoc->tremout = (whitenoise & 0x8000000) ? -pvoc->tremout : pvoc->tremout;
        }
        else   // should not get here
            pvoc->tremout = 0.0;

        // apply tremolo up to depth
        pvoc->voiceout = pvoc->voiceout * (1.0 - (pvoc->tremdepth * pvoc->tremout));
    }

    // Voiceout now has the new generated value. Pass it through the filters
    // and the ADSR amplitude envelope.
    //  The filter is second order with both poles and zeros. This filter is
    // of Type I so has separate stores for past inputs and past outputs.
    if (pvoc->flttype != FILT_OFF) {
        // Filter #1 always runs if filters are enabled
        pvoc->flt1out0 = (pvoc->flt1b0 * pvoc->voiceout) +
                         (pvoc->flt1b1 * pvoc->flt1in1) +
                         (pvoc->flt1b2 * pvoc->flt1in2) +
                         (-pvoc->flt1a1 * pvoc->flt1out1) +
                         (-pvoc->flt1a2 * pvoc->flt1out2);
        pvoc->flt1out2 = pvoc->flt1out1;
        pvoc->flt1out1 = pvoc->flt1out0;
        pvoc->flt1in2  = pvoc->flt1in1;
        pvoc->flt1in1  = pvoc->voiceout;
        // Filter #2 runs if 12 dB or band-pass or band-stop filters
        if ((pvoc->fltrolloff == 12) || (pvoc->flttype == FILT_BAND) || (pvoc->flttype == FILT_STOP)) {
            // The input to the second filter is the same as filter #1's input if
            // the type is STOP.  Else it is the output of filter #1.
            flt2input = (pvoc->flttype == FILT_STOP) ? pvoc->voiceout : pvoc->flt1out0;
            pvoc->flt2out0 = (pvoc->flt2b0 * flt2input) +
                             (pvoc->flt2b1 * pvoc->flt2in1) +
                             (pvoc->flt2b2 * pvoc->flt2in2) +
                             (-pvoc->flt2a1 * pvoc->flt2out1) +
                             (-pvoc->flt2a2 * pvoc->flt2out2);
            pvoc->flt2out2 = pvoc->flt2out1;
            pvoc->flt2out1 = pvoc->flt2out0;
            pvoc->flt2in2  = pvoc->flt2in1;
            pvoc->flt2in1  = flt2input;
        }
        // Output of the filters is filter #2's output for low, high, and band
        // pass filters, and the average of both filters for band reject
        if (pvoc->flttype == FILT_STOP)
            pvoc->voiceout = (pvoc->flt1out0 + pvoc->flt2out0) / 2.0;
        else
            pvoc->voiceout = pvoc->flt2out0;
    }

    // The ADSR envelope has eight steps.  Librta does not do tables-of-tables
    // so we compute the step time and gain using the index multiplied by the
    // sizeof int or float. 
    //  Compute the current gain as a linear interpolation of the previous gain
    // up (down) to this step's target gain.  Base this on the fraction of ontime
    // divided by this step's steptime.
    //  Start by getting the previous gain.
    if (pvoc->adsridx == 0) {
        prevgain = 0.0;
    } else {
        prevgain = *(float *)((long long int)&(pvoc->step0gain) + (long long int)(sizeof(float) * (pvoc->adsridx -1)));
    }

    // If in SUSTAIN mode use just the previous gain to compute output
    if (pvoc->vstate == VSTATE_SUSTAIN) {
        pvoc->voiceout = pvoc->voiceout * prevgain;
    }
    else {
        //  Get the target gain
        if (pvoc->adsridx == MXADSRSTEP) {
            targetgain = 0.0;
        } else {
            targetgain = *(float *)((long long int)&(pvoc->step0gain) + (sizeof(float) * (pvoc->adsridx)));
        }
        // if target gain is zero then the note is finished
        if (targetgain == 0.0) {
            pvoc->voiceout = 0.0;
            pvoc->vstate = VSTATE_FREE;
            return;
        }
        // Get this step's duration
        steptime = *(int *)((long long int)&(pvoc->step0time) + (long long int)(sizeof(int) * (pvoc->adsridx)));
        steptime = (steptime == 0) ? 1 : steptime;
        ontimems = (1000 * pvoc->ontime) / SRATE;  // ms to seconds since SRATE is in seconds

        // multiply voiceout by scaled gain value going from prevgain to target gain
        pvoc->voiceout = pvoc->voiceout * (prevgain + ((targetgain - prevgain) * ((float)ontimems / (float)steptime)));

        // Increment to next ADSR step if at end of this step
        if (steptime == ontimems) {
            pvoc->adsridx++;
            pvoc->ontime = 0;
            // done if we just passed the maximum ADSR step
            if (pvoc->adsridx > MXADSRSTEP) {
                pvoc->voiceout = 0.0;
                pvoc->vstate = VSTATE_FREE;
            }
        }
        else if (steptime == SUSTAINVALUE) {
            // If steptime is the SUSTAIN value then set vstate to SUSTAIN and
            // increment to the next step (so adsridx will be correct when we leave sustain).
            pvoc->adsridx++;
            pvoc->vstate = VSTATE_SUSTAIN;
        }
        else {
            pvoc->ontime++;
        }
    }
    return;
}


