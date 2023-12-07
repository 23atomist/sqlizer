/***************************************************************
 * tables.c --  Descriptions of the synthesizer component data structures
 *              in a way that allows librta to export them as Postgres
 *              tables.
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


/***************************************************************
 * apptables.c --
 *
 * Overview:
 *    The "librta" package provides a Postgres-like API into our
 * system tables and variables.  We need to describe each of our
 * tables as if it were a data base table.  We describe each
 * table in general in an array of RTA_TBLDEF structures with one
 * structure per table, and each column of each table in an
 * array of RTA_COLDEF strustures with one RTA_COLDEF structure per
 * column.
 **************************************************************/

#include <stdio.h>          /* for 'fprintf' */
#include <stddef.h>         /* for 'offsetof' */
#include <math.h>           /* for cosf,sinf, and Pi/2 */
#include "sqlizer.h"        /* for table definitions and sizes */

extern UI ui[];
extern struct VOICE voices[];
static int set_vstate(char *, char *, char *, void *, int,  void *);
static int set_o1freq(char *, char *, char *, void *, int,  void *);
static int set_o2freq(char *, char *, char *, void *, int,  void *);
static int set_vibfreq(char *, char *, char *, void *, int,  void *);
static int set_tremfreq(char *, char *, char *, void *, int,  void *);
static int get_o1freq(char *, char *, char *, void *, int);
static int get_o2freq(char *, char *, char *, void *, int);
static int set_glidefreq(char *, char *, char *, void *, int,  void *);
static int set_glidems(char *, char *, char *, void *, int,  void *);
static int set_o1symmetry(char *, char *, char *, void *, int,  void *);
static int set_o2symmetry(char *, char *, char *, void *, int,  void *);
static int set_vibsymmetry(char *, char *, char *, void *, int,  void *);
static int set_vibdepth(char *, char *, char *, void *, int,  void *);
static int set_tremsymmetry(char *, char *, char *, void *, int,  void *);
static int set_flttype(char *, char *, char *, void *, int,  void *);

/*INDENT-OFF*/

/***************************************************************
 *   Column definitions for the voices table
 **************************************************************/
RTA_COLDEF voicecols[] = {
    {
        "voices",           /* the table name */
        "idx",              /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, idx), /* location in struct */
        RTA_READONLY,       /* set at init time */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Numeric index of this row in the table."},
    {
        "voices",           /* the table name */
        "noteid",           /* the column name */
        RTA_STR,            /* it is a float */
        NOTEID_LEN,         /* number of bytes */
        offsetof(struct VOICE, noteid), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "An identified for this note.  Assigned by the UI that added the note."},
    {
        "voices",           /* the table name */
        "chordid",          /* the column name */
        RTA_STR,            /* it is a float */
        NOTEID_LEN,         /* number of bytes */
        offsetof(struct VOICE, chordid), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "All notes in a chord are given an ID so all the notes can be started at the same time.\
  Assigned by the UI that added the note."},
    {
        "voices",           /* the table name */
        "vstate",           /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, vstate), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_vstate,         /* called after write */
        "State of voice as one of off(0), inuse(1), on(2), sustain(3), or forced release(4).\
  Forced release is after a continuous sustain (eg, organ) goes from sustain to release.\
  The UI must explicitly change the state from sustain to forced release."},
    {
        "voices",           /* the table name */
        "ontime",           /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, ontime), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The number of milliseconds the tone has been on.  Set to zero at tone start."},
    {
        "voices",           /* the table name */
        "o1type",           /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, o1type), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Type of oscillator output as one of off(0), sine(1), square(2), triangle(3)\
 or noise(4)."},
    {
        "voices",           /* the table name */
        "o1freq",           /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o1freq), /* location in struct */
        0,                  /* no flags */
        get_o1freq,         /* called before read */
        set_o1freq,         /* called after write */
        "Oscillator frequency in Hertz.  Range is 0.001 to 9000."},
    {
        "voices",           /* the table name */
        "o1phaseacc",       /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o1phaseacc), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This is the phase of the output in the range of 0 to 1.  Multiply by 360 to get\
 degrees or by 2 pi to get radians."},
    {
        "voices",           /* the table name */
        "o1symmetry",       /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o1symmetry), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_o1symmetry,     /* called after write */
        "Scale the phase step by this value before the phase accumulator reaches 0.5\
 (180 degrees, or the first half of the cycle), and by one minus this value during\
 the second half of the cycle.  A value of 0.5 makes the output symmetric, that is,\
 a sine wave, square wave, or a triangle wave.  Setting the value to, say, 0.1\
 makes a square wave into a pulse, and a triangle into a ramp up.  A value of\
 0.99 makes a fast rising sawtooth with a descending ramp.  Asymmetric sine\
 waves are one frequency in the positive half cycle and a different frequency in\
 the negative half cycle."},
    {
        "voices",           /* the table name */
        "o1phaseoffset",    /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o1phaseoffset), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This value plus the phase accumulator is used to compute the phase\
 of the output waveform.  This helps make an asymmetric triangle waveform into\
 a ramp."},
    {
        "voices",           /* the table name */
        "o1gain",           /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o1gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The gain (attenuation) applied to the output from osc1."},
    {
        "voices",           /* the table name */
        "vibtype",          /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, vibtype), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Type of oscillator output as one of off(0), sine(1), square(2), triangle(3)\
 or noise(4)."},
    {
        "voices",           /* the table name */
        "vibfreq",          /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, vibfreq), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_vibfreq,        /* called after write */
        "Oscillator frequency in Hertz.  Range is 0.001 to 9000."},
    {
        "voices",           /* the table name */
        "vibphaseacc",      /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, vibphaseacc), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This is the phase of the vibrato oscillator in the range of 0 to 1. \
 Multiply by 360 to get degrees or by 2 pi to get radians."},
    {
        "voices",           /* the table name */
        "vibdepth",         /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, vibdepth), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_vibdepth,       /* called after write */
        "This is the maximum frequency that is added to o1 as part of vibrato.  The o1\
 output frequency varies between o1freq and (o1freq + vibdepth)"},
    {
        "voices",           /* the table name */
        "vibsymmetry",       /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, vibsymmetry), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_vibsymmetry,    /* called after write */
        "Scale the phase step by this value before the phase accumulator reaches 0.5\
 (180 degrees, or the first half of the cycle), and by one minus this value during\
 the second half of the cycle.  A value of 0.5 makes the output symmetric, that is,\
 a sine wave, square wave, or a triangle wave.  Setting the value to, say, 0.1\
 makes a square wave into a pulse, and a triangle into a ramp up.  A value of\
 0.99 makes a fast rising sawtooth with a descending ramp.  Asymmetric sine\
 waves are one frequency in the positive half cycle and a different frequency in\
 the negative half cycle."},
    {
        "voices",           /* the table name */
        "vibphaseoffset",    /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, vibphaseoffset), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This value plus the phase accumulator is used to compute the phase\
 of the output waveform.  This helps make an asymmetric triangle waveform into\
 a ramp."},
    {
        "voices",           /* the table name */
        "glidefreq",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, glidefreq), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_glidefreq,      /* called after write */
        "Target frequency at completion of glide.  Range is 0.001 to 9000."},
    {
        "voices",           /* the table name */
        "glidems",          /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, glidems), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_glidems,        /* called after write */
        "Number of millisecond to move from current frequency to glide frequency."},
    {
        "voices",           /* the table name */
        "o2type",           /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, o2type), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Type of oscillator output as one of off(0), sine(1), square(2), triangle(3)\
 or noise(4)."},
    {
        "voices",           /* the table name */
        "o2freq",           /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o2freq), /* location in struct */
        0,                  /* no flags */
        get_o2freq,         /* called before read */
        set_o2freq,         /* called after write */
        "Oscillator frequency in Hertz.  Range is 0.001 to 9000."},
    {
        "voices",           /* the table name */
        "o2phaseacc",       /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o2phaseacc), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This is the phase of the output in the range of 0 to 1.  Multiply by 360 to get\
 degrees or by 2 pi to get radians."},
    {
        "voices",           /* the table name */
        "o2symmetry",       /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o2symmetry), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_o2symmetry,     /* called after write */
        "Scale the phase step by this value before the phase accumulator reaches 0.5\
 (180 degrees, or the first half of the cycle), and by one minus this value during\
 the second half of the cycle.  A value of 0.5 makes the output symmetric, that is,\
 a sine wave, square wave, or a triangle wave.  Setting the value to, say, 0.1\
 makes a square wave into a pulse, and a triangle into a ramp up.  A value of\
 0.99 makes a fast rising sawtooth with a descending ramp.  Asymmetric sine\
 waves are one frequency in the positive half cycle and a different frequency in\
 the negative half cycle."},
    {
        "voices",           /* the table name */
        "o2phaseoffset",    /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o2phaseoffset), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This value plus the phase accumulator is used to compute the phase\
 of the output waveform.  This helps make an asymmetric triangle waveform into\
 a ramp."},
    {
        "voices",           /* the table name */
        "o2gain",           /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, o2gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The gain (attenuation) applied to the output from osc2."},
    {
        "voices",           /* the table name */
        "mixmode",          /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, mixmode), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "How to mix osc1 and osc2.  Must be one of none (0), sum (1),\
AM o1 by o2, FM o1 by o2, ring, hardsync of o1 by o2."},
    {
        "voices",           /* the table name */
        "tremtype",          /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, tremtype), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Tremolo waveform as one of off(0), sine(1), square(2), triangle(3)\
 or noise(4)."},
    {
        "voices",           /* the table name */
        "tremfreq",          /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, tremfreq), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_tremfreq,       /* called after write */
        "Tremolo frequency in Hertz.  Range is 0.001 to 9000."},
    {
        "voices",           /* the table name */
        "tremphaseacc",     /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, tremphaseacc), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This is the phase of the tremelo output in the range of 0 to 1. \
 Multiply by 360 to get degrees or by 2 pi to get radians."},
    {
        "voices",           /* the table name */
        "tremdepth",         /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, tremdepth), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This is the maximum gain that is added to o1gain as part of tremolo.  The o1\
 output gain varies between o1gain and (o1gain + tremdepth)"},
    {
        "voices",           /* the table name */
        "tremsymmetry",       /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, tremsymmetry), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_tremsymmetry,   /* called after write */
        "Scale the phase step by this value before the phase accumulator reaches 0.5\
 (180 degrees, or the first half of the cycle), and by one minus this value during\
 the second half of the cycle.  A value of 0.5 makes the output symmetric, that is,\
 a sine wave, square wave, or a triangle wave.  Setting the value to, say, 0.1\
 makes a square wave into a pulse, and a triangle into a ramp up.  A value of\
 0.99 makes a fast rising sawtooth with a descending ramp.  Asymmetric sine\
 waves are one frequency in the positive half cycle and a different frequency in\
 the negative half cycle."},
    {
        "voices",           /* the table name */
        "tremphaseoffset",    /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, tremphaseoffset), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This value plus the phase accumulator is used to compute the phase\
 of the output waveform.  This helps make an asymmetric triangle waveform into\
 a ramp."},
    {
        "voices",           /* the table name */
        "step0time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, step0time), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude. \
 Set to 60000 (1 minute) to enter SUSTAIN mode."},
    {
        "voices",           /* the table name */
        "step0gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, step0gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1. \
 Set to 0 to end the note."},
    {
        "voices",           /* the table name */
        "step1time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, step1time), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude \
 Set to 60000 (1 minute) to enter SUSTAIN mode.."},
    {
        "voices",           /* the table name */
        "step1gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, step1gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1. \
 Set to 0 to end the note."},
    {
        "voices",           /* the table name */
        "step2time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, step2time), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude \
 Set to 60000 (1 minute) to enter SUSTAIN mode.."},
    {
        "voices",           /* the table name */
        "step2gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, step2gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1. \
 Set to 0 to end the note."},
    {
        "voices",           /* the table name */
        "step3time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, step3time), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude \
 Set to 60000 (1 minute) to enter SUSTAIN mode.."},
    {
        "voices",           /* the table name */
        "step3gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, step3gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1. \
 Set to 0 to end the note."},
    {
        "voices",           /* the table name */
        "step4time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, step4time), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude \
 Set to 60000 (1 minute) to enter SUSTAIN mode.."},
    {
        "voices",           /* the table name */
        "step4gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, step4gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1. \
 Set to 0 to end the note."},
    {
        "voices",           /* the table name */
        "step5time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, step5time), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude \
 Set to 60000 (1 minute) to enter SUSTAIN mode.."},
    {
        "voices",           /* the table name */
        "step5gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, step5gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1. \
 Set to 0 to end the note."},
    {
        "voices",           /* the table name */
        "step6time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, step6time), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude \
 Set to 60000 (1 minute) to enter SUSTAIN mode.."},
    {
        "voices",           /* the table name */
        "step6gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, step6gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1. \
 Set to 0 to end the note."},
    {
        "voices",           /* the table name */
        "step7time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, step7time), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude \
 Set to 60000 (1 minute) to enter SUSTAIN mode."},
    {
        "voices",           /* the table name */
        "step7gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, step7gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1. \
 Set to 0 to end the note."},
    {
        "voices",           /* the table name */
        "flttype",          /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, flttype), /* location in struct */
        0,                  /* no flags */ 
        (int (*)()) 0,      /* called before read */
        set_flttype,        /* called after write */
        "The type of output filter: none (0), low-pass (1), high-pass(2),\
 band-pass(3), or band-reject(4)"},
    {
        "voices",           /* the table name */
        "fltfreq1",         /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, fltf1), /* location in struct */
        0,                  /* no flags */ 
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Output filter #1 cutoff frequency in range of 1 to 20000 Hz."},
    {
        "voices",           /* the table name */
        "fltfreq2",         /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, fltf2), /* location in struct */
        0,                  /* no flags */ 
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Output filter #2 cutoff frequency in range of 1 to 20000 Hz.."},
    {
        "voices",           /* the table name */
        "fltrolloff",       /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, fltrolloff), /* location in struct */
        0,                  /* no flags */ 
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Output filter rolloff in dB.  Must be either 6 or 12.  Band pass\
 and band stop filters always have 6 dB rolloff"},
    {
        "voices",           /* the table name */
        "fltQ",             /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, fltq), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The Q for the output filter in range of 0.1 to 25."},
    {
        "voices",           /* the table name */
        "outputgain",       /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, outputgain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The gain (attenuation) applied to the final output.  Must be between zero and one."},
    {
        "voices",           /* the table name */
        "vout",             /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, vout), /* location in struct */
        RTA_READONLY,       /* read only */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The instantaneous value of the output after being processed by the ADSR\
 envelope and the final output gain. This is a 16 bit signed value."},
};

/***************************************************************
 *   We defined all of the data structure (column defintions)
 * for the tables above.  Now define the tables themselves.
 **************************************************************/
RTA_TBLDEF UITables[] = {
    {
        "voices",           /* table name */
        voices,             /* address of table */
        sizeof(struct VOICE), /* length of each row */
        VOICE_COUNT,        /* number of rows */
        (void *) NULL,      /* iterator function */
        (void *) NULL,      /* iterator callback data */
        (void *) NULL,      /* INSERT callback */
        (void *) NULL,      /* DELETE callback */
        voicecols,          /* array of column defs */
        sizeof(voicecols) / sizeof(RTA_COLDEF), /* number of cols */
        "",                 /* save file name */
        "Table of voices and their parameters"},
};
int      nuitables = (sizeof(UITables) / sizeof(RTA_TBLDEF));
/*INDENT-ON*/



/***************************************************************
 * set_oXsymmetry(): - Validate a new oscillator value for
 * symmetry.  Valid values are in the range of 0.001 to 0.999.
 * return 1 if error and 0 if valid
 * 
 * Output:       0 if valid
 * Effects:      Oscillator table
 ***************************************************************/
int set_o1symmetry (
    char *tbl,          // "voices"
    char *column,       // "symmetry"
    char *SQL,          // UI command that changed symmetry
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    // Comparing floats is not exactly _exact_
    if (posc->o1symmetry < 0.0099)
        posc->o1symmetry = 0.01;
    else if (posc->o1symmetry > 0.9991)
        posc->o1symmetry = 0.999;
    return 0;
}
int set_o2symmetry (
    char *tbl,          // "voices"
    char *column,       // "symmetry"
    char *SQL,          // UI command that changed symmetry
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    // Comparing floats is not exactly _exact_
    if (posc->o2symmetry < 0.0099)
        posc->o2symmetry = 0.01;
    else if (posc->o2symmetry > 0.9991)
        posc->o2symmetry = 0.999;
    return 0;
}
int set_vibsymmetry (
    char *tbl,          // "voices"
    char *column,       // "symmetry"
    char *SQL,          // UI command that changed symmetry
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    // Comparing floats is not exactly _exact_
    if (posc->vibsymmetry < 0.0099)
        posc->vibsymmetry = 0.01;
    else if (posc->vibsymmetry > 0.9991)
        posc->vibsymmetry = 0.999;
    return 0;
}
int set_tremsymmetry (
    char *tbl,          // "voices"
    char *column,       // "symmetry"
    char *SQL,          // UI command that changed symmetry
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    // Comparing floats is not exactly _exact_
    if (posc->tremsymmetry < 0.0099)
        posc->tremsymmetry = 0.01;
    else if (posc->tremsymmetry > 0.9991)
        posc->tremsymmetry = 0.999;
    return 0;
}


/***************************************************************
 * set_XXXXfreq(): - Validate a new oscillator value for
 * frequency.  Valid values are in the range of 0.001 to MX_FREQ
 * return 1 if error and 0 if valid
 * 
 * Output:       0 if valid
 * Effects:      oscillator phasestep
 ***************************************************************/
int set_o1freq (
    char *tbl,          // "voices"
    char *column,       // "freq"
    char *SQL,          // UI command that changed freq
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    // Comparing floats is not exactly _exact_
    if (posc->o1freq < 0.0099)
        posc->o1freq = 0.01;
    if (posc->o1freq > MX_FREQ)
        posc->o1freq = MX_FREQ;

    // Valid frequency.  Recompute phasestep
    posc->o1phasestep = posc->o1freq / SRATE;
    return 0;
}
int set_o2freq (
    char *tbl,          // "voices"
    char *column,       // "freq"
    char *SQL,          // UI command that changed freq
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    // Comparing floats is not exactly _exact_
    if (posc->o2freq < 0.0099)
        posc->o2freq = 0.01;
    if (posc->o2freq > MX_FREQ)
        posc->o2freq = MX_FREQ;

    // Valid frequency.  Recompute phasestep
    posc->o2phasestep = posc->o2freq / SRATE;
    return 0;
}
int set_vibfreq (
    char *tbl,          // "voices"
    char *column,       // "vibratofreq"
    char *SQL,          // UI command that changed glidems
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    if (posc->vibfreq < 0.0099)
        posc->vibfreq = 0.01;
    if (posc->vibfreq > MX_FREQ)
        posc->vibfreq = MX_FREQ;

    // Set vibphasestep based on the frequncy
    // Use float to prevent integer overflow
    posc->vibphasestep = posc->vibfreq / SRATE;
    return 0;
}
int set_tremfreq (
    char *tbl,          // "voices"
    char *column,       // "tremratofreq"
    char *SQL,          // UI command that changed glidems
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    if (posc->tremfreq < 0.0099)
        posc->tremfreq = 0.01;
    if (posc->tremfreq > MX_FREQ)
        posc->tremfreq = MX_FREQ;

    // Set tremphasestep based on the frequncy
    // Use float to prevent integer overflow
    posc->tremphasestep = posc->tremfreq / SRATE;
    return 0;
}


/***************************************************************
 * set_vibdepth(): - Convert depth as a frequency to the maximum
 * phase offset to be applied to oscillator #1
 * 
 * Output:       0 if valid
 * Effects:      osc #1 frequency range per vibrato
 ***************************************************************/
int set_vibdepth (
    char *tbl,          // "voices"
    char *column,       // "vibratofreq"
    char *SQL,          // UI command that changed glidems
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;

    // Set o1 max phase offset based on the vibrato depth
    posc->vibo1phase = posc->vibdepth / SRATE;
    return 0;
}

/***************************************************************
 * get_XXXXfreq(): - Convert phasestep to frequency.  This is needed
 * since glide can change the set frequency of the oscillator.
 * 
 * Output:       0 if valid
 * Effects:      oscillator phasestep
 ***************************************************************/
int get_o1freq (
    char *tbl,          // "voices"
    char *column,       // "freq"
    char *SQL,          // UI command that reads freq
    void *pr,           // pointer to the row
    int row_num)        // zero index of row in table
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;

    // Valid frequency.  Recompute phasestep
    posc->o1freq = SRATE * posc->o1phasestep;
    return 0;
}
int get_o2freq (
    char *tbl,          // "voices"
    char *column,       // "freq"
    char *SQL,          // UI command that reads freq
    void *pr,           // pointer to the row
    int row_num)        // zero index of row in table
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;

    // Valid frequency.  Recompute phasestep
    posc->o2freq = SRATE * posc->o2phasestep;
    return 0;
}


/***************************************************************
 * set_glidefreq(): - Validate a new oscillator value for
 * glidefrequency.  Valid values are in the range of 0.001 to MX_FREQ
 * return 1 if error and 0 if valid
 * 
 * Output:       0 if valid
 * Effects:      oscillator glidestep
 ***************************************************************/
int set_glidefreq (
    char *tbl,          // "voices"
    char *column,       // "glidefreq"
    char *SQL,          // UI command that changed glidefreq
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    // Comparing floats is not exactly _exact_
    if (posc->glidefreq < 0.0099)
        posc->glidefreq = 0.01;
    if (posc->glidefreq > MX_FREQ)
        posc->glidefreq = MX_FREQ;
    return 0;
}


/***************************************************************
 * set_glidems(): - Validate a new oscillator value for
 * glide timing.  Valid values are non-negative and less than 10M.
 * Return 1 if error and 0 if valid
 * 
 * Output:       0 if valid
 * Effects:      Oscillator table
 ***************************************************************/
int set_glidems (
    char *tbl,          // "voices"
    char *column,       // "glidems"
    char *SQL,          // UI command that changed glidems
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct VOICE *posc;

    posc = (struct VOICE *) pr;
    if (posc->glidems < 0)
        posc->glidems = 0;
    else if (posc->glidems > 10000000)
        posc->glidems = 10000000;

    // Set glidecount with the number audio samples in glidems
    // Use float to prevent integer overflow
    posc->glidecount = (int)((float)SRATE * (float)posc->glidems / 1000.0);

    // We want to step from the current phasestep to the phasestep
    // set by glidefrequency in glidecount steps.
    if (posc->glidecount == 0)
        posc->glidestep = 0.0;
    else
        posc->glidestep = ((posc->glidefreq / SRATE) - posc->o1phasestep) / (float) posc->glidecount;
    return 0;
}


/***************************************************************
 * set_vstate(): - set the state of the voice.  IN-USE reserves
 * the voice for further configuration before use.  ON turns 
 * voice on and starts output.  SUSTAIN is similar to ON except
 * that if a ADSR envelope state has a time value or -1 then 
 * the voice remains on and ADSR is suspended.  Changing from 
 * SUSPEND to ON set the ADSR envelop time to 1 ms effectively 
 * moving the voice to the next state in the ADSR.
 * 
 * Output:       0 if valid
 * Effects:      voice state
 ***************************************************************/
int set_vstate (
    char *tbl,          // "voices"
    char *column,       // "vstate"
    char *SQL,          // UI command that changed state
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    int    newstate;
    int    oldstate;
    struct VOICE *pvoc;

    pvoc = (struct VOICE *) pr;
    newstate = pvoc->vstate;
    oldstate = ((struct VOICE *) poldrow)->vstate;

    // If going from OFF to ON, clear the ADSR note timer
    if ((newstate == VSTATE_ON) && (oldstate == VSTATE_FREE)) {
        pvoc->ontime = 0;
        pvoc->adsridx = 0;
    }

    // if going from SUSTAIN to ON increment to the next step in ADSR
    else if ((newstate == VSTATE_ON) && (oldstate == VSTATE_SUSTAIN)) {
        if (pvoc->adsridx == MXADSRSTEP) {
            // can't go past last step.  Turn voice off.
            pvoc->vstate = VSTATE_FREE;
            pvoc->voiceout = 0.0;
            pvoc->vout = 0;
        }
    }

    return 0;
}


/***************************************************************
 * set_flttype(): - Validate and limit the parameters for a filter
 * and compute the filter parameters.
 * We do all filter processing here even though, logically, we should
 * have callbacks for Q, rolloff, and frequencies.
 * 
 * Output:       0 if valid
 * Effects:      filter parameters
 ***************************************************************/
int set_flttype (
    char *tbl,          // "voices"
    char *column,       // "flttype
    char *SQL,          // UI command that changed glidefreq
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    float  d, g;                     // to simplify coefficient calculations
    struct VOICE *pvoc;

    pvoc = (struct VOICE *) pr;
    // Comparing floats is not exactly _exact_
    if (pvoc->fltf1 < 1)             // validate/limit cutoff frequency
        pvoc->fltf1 = 1;
    else if (pvoc->fltf1 > 20000)
        pvoc->fltf1 = 20000;
    if (pvoc->fltf2 < 1)
        pvoc->fltf2 = 1;
    else if (pvoc->fltf2 > 20000)
        pvoc->fltf2 = 20000;
    if (pvoc->fltq < 0.1)          // validate/limit filter Q factor
        pvoc->fltq = 0.1;
    else if (pvoc->fltq > 25.0)
        pvoc->fltq = 25.0;
    if (pvoc->fltrolloff < 6)       // validate/limit filter rolloff
        pvoc->fltrolloff = 6;
    else if (pvoc->fltrolloff > 12)
        pvoc->fltrolloff = 12;
    pvoc->fltrolloff = 6 * (pvoc->fltrolloff / 6);  // forces value to 6 or 12

    // Just return if off
    if (pvoc->flttype == FILT_OFF)
        return 0;

    // Computer the digital filter coefficients based on type, freq, rolloff, and Q
    // Filter #1 is low pass for low-pass and band-stop filters.  
    g = tan(M_PI * pvoc->fltf1 / (float) SRATE);
    d = (pvoc->fltq * g * g) + g + pvoc->fltq;
    if ((pvoc->flttype == FILT_LOW) || (pvoc->flttype == FILT_STOP)) {
        pvoc->flt1b0 = pvoc->fltq * g * g / d;
        pvoc->flt1b1 = 2 * pvoc->flt1b0;
        pvoc->flt1b2 = pvoc->flt1b0;
        pvoc->flt1a1 = 2 * pvoc->fltq * ((g * g) -1) / d;
        pvoc->flt1a2 = ((pvoc->fltq * g * g) - g + pvoc->fltq) / d;
    }
    else {
        // filter # 1 is high pass
        pvoc->flt1b0 = pvoc->fltq / d;
        pvoc->flt1b1 = -2 * pvoc->flt1b0;
        pvoc->flt1b2 = pvoc->flt1b0;
        pvoc->flt1a1 = 2 * pvoc->fltq * ((g * g) -1) / d;
        pvoc->flt1a2 = ((pvoc->fltq * g * g) - g + pvoc->fltq) / d;
    }
    // Filter #2 is identical to filter #1 for 12 dB low and high pass filters
    if (((pvoc->flttype == FILT_LOW) || (pvoc->flttype == FILT_HIGH)) && (pvoc->fltrolloff == 12)) {
        pvoc->fltf2 = pvoc->fltf1;
        pvoc->flt2b0 = pvoc->flt1b0;
        pvoc->flt2b1 = pvoc->flt1b1;
        pvoc->flt2b2 = pvoc->flt1b2;
        pvoc->flt2a1 = pvoc->flt1a1;
        pvoc->flt2a2 = pvoc->flt1a2;
    }
    // Filter #2 is low pass for band pass, and high pass for band-stop.
    else if (pvoc->flttype == FILT_BAND) {
        g = tan(M_PI * pvoc->fltf2 / (float) SRATE);
        d = (pvoc->fltq * g * g) + g + pvoc->fltq;
        pvoc->flt2b0 = pvoc->fltq * g * g / d;
        pvoc->flt2b1 = 2 * pvoc->flt2b0;
        pvoc->flt2b2 = pvoc->flt2b0;
        pvoc->flt2a1 = 2 * pvoc->fltq * ((g * g) -1) / d;
        pvoc->flt2a2 = ((pvoc->fltq * g * g) - g + pvoc->fltq) / d;
    }
    else if (pvoc->flttype == FILT_STOP) {
        g = tan(M_PI * pvoc->fltf2 / (float) SRATE);
        d = (pvoc->fltq * g * g) + g + pvoc->fltq;
        pvoc->flt2b0 = pvoc->fltq / d;
        pvoc->flt2b1 = -2 * pvoc->flt2b0;
        pvoc->flt2b2 = pvoc->flt2b0;
        pvoc->flt2a1 = 2 * pvoc->fltq * ((g * g) -1) / d;
        pvoc->flt2a2 = ((pvoc->fltq * g * g) - g + pvoc->fltq) / d;
    }

    return 0;
}


