![sqlizer](https://felined.cat/sqlizer/sqlizer.jpeg)

# SQLIZER

Sqlizer is a software based music synthesizer that organizes its components
into *tables*.  There is a table of oscillators, a table of voices, and a
table of instrument notes.  

Components are defined in the program by a C-style data structure.  An array
or linked list of these structures is a table.

Each row in a table is an instance of that component and each column is a
parameter that describes the component.  For example, the table of
oscillators has columns that specify the frequency and waveform to use
for that oscillator.

Structured Query Language, SQL, is the standard way client programs read
and set values in the tables of a database.  Sqlizer uses the librta library
to make its tables of components *look like* they are in a Postgres database.
This lets you control the synthesizer using SQL from UI client programs.
The synthesizer daemon serializes requests from the UI clients and so it can
support dozens of simultaneous UI client programs.

## Why a Database API ?
It may seem strange to have SQL as the interface language for a synthesizer
but this has several advantages.  First, this greatly simplifies the design
of the synthesizer daemon since we do not have to define and build our own
API as part of it.  Second, Postgres has client side libraries for dozens of
programming languages.  This lets developers use any programming language
they want to write the user interface programs, and the UI programs do not
need to be built at the same time or by the same group that does the daemon.
Third, tables are very clearly defined and in debugging client-daemon problems
it is easy to see if the SQL is correct.  If the SQL is correct the problem
is in the daemon.  If the SQL is not correct the problem is in the UI program.

## Tables or Objects ?
You might think of a table approach as a very constrained object-oriented
approach.  Database INSERT and DELETE correspond to the constructors and
destructors of an OOD.  UPDATE and SELECT callbacks correspond to object
accessor functions.  Not all members of the underlying row structure need
to be visible to the API.  This corresponds to public and private class
members.

A table/data focused design is much better than an object oriented design.
Forcing the data into tables is more constraining then creating objects but
this actually simplifies a design.  Tables and a database API bring all of
the data in the program to the surface where a user or developer can see
it and change it.  This has made debugging the synthesizer daemon almost
trivial.

Programmers and non-programmers alike are used to dealing with tables of
data.  Programmers will probably find it easer and faster to review data 
in a table compared to tracking through a tree of objects and their
necessarily custom input and output methods.

![](sqlizerarch.svg)

## Tables
We anticipate using several tables in the design.  These include:
- Oscillators : with frequency, waveform, vibrato, and glide
- Voices : with two oscillators, mixer, gain, filter, and amplitude envelope
- Notes : oscillator and voice values sorted by musical instrument and note

Still to be determined is how much to include in the voices table.  For
example, maybe the oscillators should be part of the voices and in a stand-alone
table.  Other tables might include one for multiple outputs and one for
audio effects.

## Goals
The near term goal is to built the synthesizer daemon along with a sequencer
and MIDI interface for it.

A possible longer term goal is to convert from floating point to integer
arithmetic and then convert to Verilog for use on an FPGA.

## Status
The oscillators are working and have the following parameters:
- Frequency
- Waveform (sine, square, triangle, noise)
- Waveform symmetry
- Vibrato (index of controlling oscillator)
- Vibrato frequency
- Glide target frequency
- Glide duration in milliseconds
- Hard sync (index of controlling oscillator)

