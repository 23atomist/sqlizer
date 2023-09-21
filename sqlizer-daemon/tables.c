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
#include "sqlizer.h"        /* for table definitions and sizes */

extern UI ui[];
extern struct OSCILLATOR oscillators[];
extern struct AMP_ENVELOPE ampenv[];
extern struct VOICE voices[];
static int set_freq(char *, char *, char *, void *, int,  void *);
static int get_freq(char *, char *, char *, void *, int);
static int set_glidefreq(char *, char *, char *, void *, int,  void *);
static int set_glidems(char *, char *, char *, void *, int,  void *);
static int set_symmetry(char *, char *, char *, void *, int,  void *);
static int set_vibratofreq(char *, char *, char *, void *, int,  void *);

/*INDENT-OFF*/
/***************************************************************
 *   Column definitions for the oscillator table
 **************************************************************/
RTA_COLDEF osccols[] = {
    {
        "oscillators",      /* the table name */
        "idx",              /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct OSCILLATOR, idx), /* location in struct */
        RTA_READONLY,       /* set at init time */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Numeric index of row in this table."},
    {
        "oscillators",      /* the table name */
        "otype",            /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct OSCILLATOR, otype), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Type of oscillator output as one of off(0), sine(1), square(2), or triangle(3)\
 or noise(4)."},
    {
        "oscillators",      /* the table name */
        "freq",             /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct OSCILLATOR, freq), /* location in struct */
        0,                  /* no flags */
        get_freq,           /* called before read */
        set_freq,           /* called after write */
        "Oscillator frequency in Hertz.  Range is 0.001 to 20000."},
    {
        "oscillators",      /* the table name */
        "phaseaccumulator", /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct OSCILLATOR, phaseaccumulator), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This is the phase of the output in the range of 0 to 1.  Multiply by 360 to get\
 degrees or by 2 pi to get radians."},
    {
        "oscillators",      /* the table name */
        "glidefreq",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct OSCILLATOR, glidefreq), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_glidefreq,      /* called after write */
        "Target frequency at completion of glide.  Range is 0.001 to 20000."},
    {
        "oscillators",      /* the table name */
        "glidems",          /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct OSCILLATOR, glidems), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_glidems,        /* called after write */
        "Number of millisecond to move from current frequency to glide frequency."},
    {
        "oscillators",      /* the table name */
        "symmetry",         /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct OSCILLATOR, symmetry), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_symmetry,       /* called after write */
        "Scale the phase step by this value before the phase accumulator reaches 0.5\
 (180 degrees, or the first half of the cycle), and by one minus this value during\
 the second half of the cycle.  A value of 0.5 makes the output symmetric, that is,\
 a sine wave, square wave, or a triangle wave.  Setting the value to, say, 0.1\
 makes a square wave into a pulse, and a triangle into a ramp up.  A value of\
 0.99 makes a fast rising sawtooth with a descending ramp.  Asymmetric sine\
 waves are one frequency in the positive half cycle and a different frequency in\
 the negative half cycle."},
    {
        "oscillators",      /* the table name */
        "phaseoffset",      /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct OSCILLATOR, phaseoffset), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "This value plus the phase accumulator is used to compute the phase\
 of the output waveform.  This helps make an asymmetric triangle waveform into\
 a ramp."},
    {
        "oscillators",      /* the table name */
        "vibratoosc",       /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct OSCILLATOR, vibratoosc), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The index of the oscillator that controls vibrato"},
    {
        "oscillators",      /* the table name */
        "vibratofreq",      /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct OSCILLATOR, vibratofreq), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        set_vibratofreq,    /* called after write */
        "Add up to this frequency to the output based on the vibrato oscallator. \
 To vary the output between 420 and 460 Hertz set the base frequency to 420 and\
 the vibrato frequency to 40."},
    {
        "oscillators",      /* the table name */
        "hardsyncosc",      /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct OSCILLATOR, hardsyncosc), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The index of the oscillator that controls hardsync on this oscillator.\
  A value of -1 turns off hardsync."},
};


/***************************************************************
 *   Column definitions for the amplitude envelopes table
 **************************************************************/
RTA_COLDEF ampenvcols[] = {
    {
        "ampenv",           /* the table name */
        "idx",              /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, idx), /* location in struct */
        RTA_READONLY,       /* set at init time */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Numeric index of this row in the table."},
    {
        "ampenv",           /* the table name */
        "envname",          /* the column name */
        RTA_STR,            /* it is a float */
        ENVNAME_LEN,        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, envname), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "A unique identified for this amplitude envelope."},
    {
        "ampenv",           /* the table name */
        "step1time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step1time), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude."},
    {
        "ampenv",           /* the table name */
        "step1gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step1gain), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1."},
    {
        "ampenv",           /* the table name */
        "step2time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step2time), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude."},
    {
        "ampenv",           /* the table name */
        "step2gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step2gain), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1."},
    {
        "ampenv",           /* the table name */
        "step3time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step3time), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude."},
    {
        "ampenv",           /* the table name */
        "step3gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step3gain), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1."},
    {
        "ampenv",           /* the table name */
        "step4time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step4time), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude."},
    {
        "ampenv",           /* the table name */
        "step4gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step4gain), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1."},
    {
        "ampenv",           /* the table name */
        "step5time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step5time), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude."},
    {
        "ampenv",           /* the table name */
        "step5gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step5gain), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1."},
    {
        "ampenv",           /* the table name */
        "step6time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step6time), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude."},
    {
        "ampenv",           /* the table name */
        "step6gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step6gain), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1."},
    {
        "ampenv",           /* the table name */
        "step7time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step7time), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude."},
    {
        "ampenv",           /* the table name */
        "step7gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step7gain), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1."},
    {
        "ampenv",           /* the table name */
        "step8time",        /* the column name */
        RTA_INT,            /* it is a float */
        sizeof(int),        /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step8time), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Number of milliseconds to apply this step/gain to the voice amplitude."},
    {
        "ampenv",           /* the table name */
        "step8gain",        /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct AMP_ENVELOPE, step8gain), /* location in struct */
        RTA_DISKSAVE,       /* save to disk on change */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "Amount of gain to apply during this step. Must be between 0 and 1."},
};


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
        "A unique identified for this note.  Assigned by the UI that added the note."},
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
        (int (*)()) 0,      /* called after write */
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
        "ampenvidx",        /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, ampenvidx), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The index in amplitude envelope table of which envelope to\
 use for this voice."},
    {
        "voices",           /* the table name */
        "osc1",             /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, osc1), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The index in the oscillator table for the main oscillator for this voice."},
    {
        "voices",           /* the table name */
        "osc1gain",         /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, osc1gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The input gain (attenuation) applied to the data from osc1."},
    {
        "voices",           /* the table name */
        "osc2",             /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, osc2), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The index in the oscillator table of the seconary oscillator for\
 this voice."},
    {
        "voices",           /* the table name */
        "osc2gain",         /* the column name */
        RTA_FLOAT,          /* it is a float */
        sizeof(float),      /* number of bytes */
        offsetof(struct VOICE, osc2gain), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "The input gain (attenuation) applied to the data from osc2."},
    {
        "voices",           /* the table name */
        "mixmode",          /* the column name */
        RTA_INT,            /* it is an integer */
        sizeof(int),        /* number of bytes */
        offsetof(struct VOICE, mixmode), /* location in struct */
        0,                  /* no flags */
        (int (*)()) 0,      /* called before read */
        (int (*)()) 0,      /* called after write */
        "How to mix osc1 and osc2.  Must be one of sum (0), amplitude osc1 by\
 osc2 (1), or ring mix osc1 and osc2 (2)."},
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
        "oscillators",      /* table name */
        oscillators,        /* address of table */
        sizeof(struct OSCILLATOR), /* length of each row */
        OSCILLATOR_COUNT,   /* number of rows */
        (void *) NULL,      /* iterator function */
        (void *) NULL,      /* iterator callback data */
        (void *) NULL,      /* INSERT callback */
        (void *) NULL,      /* DELETE callback */
        osccols,            /* array of column defs */
        sizeof(osccols) / sizeof(RTA_COLDEF), /* number of cols */
        "",                 /* save file name */
        "Table of oscillators and their parameters"},
    {
        "ampenv",           /* table name */
        ampenv,             /* address of table */
        sizeof(struct AMP_ENVELOPE), /* length of each row */
        ENVELOPE_COUNT,     /* number of rows */
        (void *) NULL,      /* iterator function */
        (void *) NULL,      /* iterator callback data */
        (void *) NULL,      /* INSERT callback */
        (void *) NULL,      /* DELETE callback */
        ampenvcols,         /* array of column defs */
        sizeof(ampenvcols) / sizeof(RTA_COLDEF), /* number of cols */
        "ampenv.sql",       /* save file name */
        "Table of amplitude envelopes and their parameters"},
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
 * set_symmetry(): - Validate a new oscillator value for
 * symmetry.  Valid values are in the range of 0.001 to 0.999.
 * return 1 if error and 0 if valid
 * 
 * Output:       0 if valid
 * Effects:      Oscillator table
 ***************************************************************/
int set_symmetry (
    char *tbl,          // "oscillators"
    char *column,       // "symmetry"
    char *SQL,          // UI command that changed symmetry
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct OSCILLATOR *posc;

    posc = (struct OSCILLATOR *) pr;
    // Comparing floats is not exactly _exact_
    if ((posc->symmetry < 0.0099) || (posc->symmetry > 0.9991))
        return 1;
    else
        return 0;
}


/***************************************************************
 * set_freq(): - Validate a new oscillator value for
 * frequency.  Valid values are in the range of 0.001 to 20000
 * return 1 if error and 0 if valid
 * 
 * Output:       0 if valid
 * Effects:      oscillator phasestep
 ***************************************************************/
int set_freq (
    char *tbl,          // "oscillators"
    char *column,       // "freq"
    char *SQL,          // UI command that changed freq
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct OSCILLATOR *posc;

    posc = (struct OSCILLATOR *) pr;
    // Comparing floats is not exactly _exact_
    if ((posc->freq < 0.0099) || (posc->freq > 20000.1))
        return 1;

    // Valid frequency.  Recompute phasestep
    posc->phasestep = posc->freq / SRATE;
    return 0;
}


/***************************************************************
 * get_freq(): - Convert phasestep to frequency.  This is needed
 * since glide can change the set frequency of the oscillator.
 * 
 * Output:       0 if valid
 * Effects:      oscillator phasestep
 ***************************************************************/
int get_freq (
    char *tbl,          // "oscillators"
    char *column,       // "freq"
    char *SQL,          // UI command that reads freq
    void *pr,           // pointer to the row
    int row_num)        // zero index of row in table
{
    struct OSCILLATOR *posc;

    posc = (struct OSCILLATOR *) pr;

    // Valid frequency.  Recompute phasestep
    if (posc->phasestep != 0.0)
        posc->freq = SRATE * posc->phasestep;
    else
        posc->freq = 0.0;
    return 0;
}


/***************************************************************
 * set_glidefreq(): - Validate a new oscillator value for
 * glidefrequency.  Valid values are in the range of 0.001 to 20000
 * return 1 if error and 0 if valid
 * 
 * Output:       0 if valid
 * Effects:      oscillator glidestep
 ***************************************************************/
int set_glidefreq (
    char *tbl,          // "oscillators"
    char *column,       // "glidefreq"
    char *SQL,          // UI command that changed glidefreq
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct OSCILLATOR *posc;

    posc = (struct OSCILLATOR *) pr;
    // Comparing floats is not exactly _exact_
    if ((posc->glidefreq < 0.0099) || (posc->glidefreq > 20000.1))
        return 1;

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
    char *tbl,          // "oscillators"
    char *column,       // "glidems"
    char *SQL,          // UI command that changed glidems
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct OSCILLATOR *posc;

    posc = (struct OSCILLATOR *) pr;
    if ((posc->glidems < 0) || (posc->glidems > 10000000))
        return 1;

    // Set glidecount with the number audio samples in glidems
    // Use float to prevent integer overflow
    posc->glidecount = (int)((float)SRATE * (float)posc->glidems / 1000.0);

    // We want to step from the current phasestep to the phasestep
    // set by glidefrequency in glidecount steps.
    posc->glidestep = ((posc->glidefreq / SRATE) - posc->phasestep) / (float) posc->glidecount;

    return 0;
}


/***************************************************************
 * set_vibratofreq(): - Validate a new vibrato frequency and set
 * the vibrato phase step for that frequency.
 * Return 1 if error and 0 if valid
 * 
 * Output:       0 if valid
 * Effects:      Oscillator table
 ***************************************************************/
int set_vibratofreq (
    char *tbl,          // "oscillators"
    char *column,       // "vibratofreq"
    char *SQL,          // UI command that changed glidems
    void *pr,           // pointer to the new row
    int row_num,        // zero index of row in table
    void *poldrow)      // row before any updates
{
    struct OSCILLATOR *posc;

    posc = (struct OSCILLATOR *) pr;
    if ((posc->vibratofreq < 0.0) || (posc->vibratofreq > 20000.1))
        return 1;

    // Set vibratostep based on the frequncy
    // Use float to prevent integer overflow
    posc->vibratostep = posc->vibratofreq / SRATE;

    return 0;
}


