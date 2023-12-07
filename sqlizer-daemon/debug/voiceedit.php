<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2//EN">
<html>
<!-- ----------------------------------------------------------------- -->
<!--  SQLizer                                                          -->
<!--  Copyright (C) 2023 Robert W Smith (bsmith@linuxtoys.org)         -->
<!--                                                                   -->
<!--   This program is distributed under the terms of the MIT          -->
<!--   License.  See the file COPYING file.                            -->
<!-- ----------------------------------------------------------------- -->
<head>
<title>Edit or Add Row</title>
</head>
<body>
<?php
    // The user has selected a row in a table to edit.
    // We want to display the name, help, and value of
    // each field in the row.  If the row is editable,
    // we want to add it to a form which lets them 
    // change the value.
    $vidx  = htmlentities($_GET["voiceidx"]);

    // Say where we are.
    print("<center><h3>Edit Voice #$vidx</h3></center>\n");

    // Suppress Postgres error messages
    error_reporting(error_reporting() & 0xFFFD);

    // connect to the database 
    $c1 = pg_connect("host=127.0.0.1 port=8889 user=anyuser");
    if ($c1 == "") { 
        printf("$s%s%s", "Unable to connect to application.<br>",
            "Please verify that the application is running and ",
            "listening on port $port.<br>");
        exit();
    }

    // execute query 
    $command = "SELECT o1type,o1freq,o1symmetry,o1phaseoffset,o1gain,";
    $command = "$command vibtype,vibfreq,vibsymmetry,vibphaseoffset,";
    $command = "$command vibdepth,glidefreq,glidems,o2type,o2freq,";
    $command = "$command o2symmetry,o2phaseoffset,o2gain,mixmode,tremtype,";
    $command = "$command tremfreq,tremdepth,tremsymmetry,tremphaseoffset,vstate,";
    $command = "$command step0time, step1time, step2time, step3time, step4time,";
    $command = "$command step5time, step6time, step7time, step0gain, step1gain,";
    $command = "$command step2gain, step3gain, step4gain, step5gain, step6gain,";
    $command = "$command step7gain, flttype, fltfreq1, fltfreq2, fltrolloff, fltQ ";
    $command = "$command FROM voices WHERE idx=$vidx";
    $r1 = pg_exec($c1, $command);
    if ($r1 == "") { 
        print("<p><font color=\"red\" size=+1>SQL Command failed!</p>");
        print("<p>Command: $command</p>\n");
        pg_close($c1);
        exit();
    }

    if (pg_NumRows($r1) != 1) {
        print("<p><font color=\"red\" size=+1>Internal Error: non-unique index</p>");
        pg_freeresult($r1);
        pg_close($c1);
        exit();
    }

    $o1type    = pg_result($r1, 0, 0);
    $o1freq    = pg_result($r1, 0, 1);
    $o1symmetry = pg_result($r1, 0, 2);
    $o1phaseoffset = pg_result($r1, 0, 3);
    $o1gain = pg_result($r1, 0, 4);
    $vibtype = pg_result($r1, 0, 5);
    $vibfreq = pg_result($r1, 0, 6);
    $vibsymmetry = pg_result($r1, 0, 7);
    $vibphaseoffset = pg_result($r1, 0, 8);
    $vibdepth = pg_result($r1, 0, 9);
    $glidefreq = pg_result($r1, 0, 10);
    $glidems = pg_result($r1, 0, 11);
    $o2type = pg_result($r1, 0, 12);
    $o2freq = pg_result($r1, 0, 13);
    $o2symmetry = pg_result($r1, 0, 14);
    $o2phaseoffset = pg_result($r1, 0, 15);
    $o2gain = pg_result($r1, 0, 16);
    $mixmode = pg_result($r1, 0, 17);
    $tremtype = pg_result($r1, 0, 18);
    $tremfreq = pg_result($r1, 0, 19);
    $tremdepth = pg_result($r1, 0, 20);
    $tremsymmetry = pg_result($r1, 0, 21);
    $tremphaseoffset = pg_result($r1, 0, 22);
    $vstate = pg_result($r1, 0, 23);
    $step0time = pg_result($r1, 0, 24);
    $step1time = pg_result($r1, 0, 25);
    $step2time = pg_result($r1, 0, 26);
    $step3time = pg_result($r1, 0, 27);
    $step4time = pg_result($r1, 0, 28);
    $step5time = pg_result($r1, 0, 29);
    $step6time = pg_result($r1, 0, 30);
    $step7time = pg_result($r1, 0, 31);
    $step0gain = pg_result($r1, 0, 32);
    $step1gain = pg_result($r1, 0, 33);
    $step2gain = pg_result($r1, 0, 34);
    $step3gain = pg_result($r1, 0, 35);
    $step4gain = pg_result($r1, 0, 36);
    $step5gain = pg_result($r1, 0, 37);
    $step6gain = pg_result($r1, 0, 38);
    $step7gain = pg_result($r1, 0, 39);
    $flttype    = pg_result($r1, 0, 40);
    $fltfreq1   = pg_result($r1, 0, 41);
    $fltfreq2   = pg_result($r1, 0, 42);
    $fltrolloff = pg_result($r1, 0, 43);
    $fltQ       = pg_result($r1, 0, 44);
    // Give URL for form processing 
    print("<form method=\"post\" action=voiceupdate.php>\n");
    print("<input type=\"hidden\" name=\"vidx\" value=\"$vidx\">\n");

    // Table for oscillators
    print("<p><table border=1 cellpadding=4>\n");
    print("<tr><th colspan=7>Oscillators</th></tr>\n");
    print("<tr><th>State/Mixing</th><th>Oscillator</th><th>Waveform</th><th>Frequency</th><th>Symmetry</th>");
    print("<th>Phase Offset</th><th>Gain</th></tr>\n");
    print("<tr><td><select name=vstate required>\n");
    if ($vstate == 0)
      print(" <option value=0 selected>Off</option>\n");
    else
      print(" <option value=0>Off</option>\n");
    if ($vstate == 1)
      print(" <option value=1 selected>In Use</option>\n");
    else
      print(" <option value=1>In Use</option>\n");
    if ($vstate == 2)
      print(" <option value=2 selected>On</option>\n");
    else
      print(" <option value=2>On</option>\n");
    if ($vstate == 3)
      print(" <option value=3 selected>Sustain</option></select></td>\n");
    else
      print(" <option value=3>Sustain</option></select></td>\n");
    print("<td>Oscillator #1</td><td><select name=o1type required>\n");
    if ($o1type == 1)
      print("<option value=1 selected>Sine</option>");
    else
      print("<option value=1>Sine</option>\n");
    if ($o1type == 2)
      print("<option value=2 selected>Square</option>");
    else
      print("<option value=2>Square</option>\n");
    if ($o1type == 3)
      print("<option value=3 selected>Triangle</option>");
    else
      print("<option value=3>Triangle</option>\n");
    if ($o1type == 4)
      print("<option value=4 selected>Noise</option></select></td>");
    else
      print("<option value=4>Noise</option></select></td>\n");
    print("<td><input name=\"o1freq\" size=9 value=\"$o1freq\"></td>\n");
    print("<td><input name=\"o1symmetry\" size=9 value=\"$o1symmetry\"></td>\n");
    print("<td><input name=\"o1phaseoffset\" size=11 value=\"$o1phaseoffset\"></td>\n");
    print("<td><input name=\"o1gain\" size=8 value=\"$o1gain\"></td></tr>\n");
    print("<tr><td><select name=mixmode required>\n");
    if ($mixmode == 0)
      print("<option value=0 selected>None</option>");
    else
      print("<option value=0>None</option>");
    if ($mixmode == 1)
      print("<option value=1 selected>Sum</option>");
    else
      print("<option value=1>Sum</option>");
    if ($mixmode == 2)
      print("<option value=2 selected>AM</option>");
    else
      print("<option value=2>AM</option>");
    if ($mixmode == 3)
      print("<option value=3 selected>FM</option>");
    else
      print("<option value=3>FM</option>");
    if ($mixmode == 4)
      print("<option value=4 selected>Ring</option>\n");
    else
      print("<option value=4>Ring</option>\n");
    if ($mixmode == 5)
      print("<option value=5 selected>HardSync</option></select></td>\n");
    else
      print("<option value=5>HardSync</option></select></td>\n");
    print("<td>Oscillator #2</td><td><select name=o2type required>\n");
    if ($o2type == 0)
      print("<option value=0 selected>Off</option>");
    else
      print("<option value=0>Off</option>\n");
    if ($o2type == 1)
      print("<option value=1 selected>Sine</option>");
    else
      print("<option value=1>Sine</option>\n");
    if ($o2type == 2)
      print("<option value=2 selected>Square</option>");
    else
      print("<option value=2>Square</option>\n");
    if ($o2type == 3)
      print("<option value=3 selected>Triangle</option>");
    else
      print("<option value=3>Triangle</option>\n");
    if ($o2type == 4)
      print("<option value=4 selected>Noise</option></select></td>");
    else
      print("<option value=4>Noise</option></select></td>\n");
    print("<td><input name=\"o2freq\" size=9 value=\"$o2freq\"></td>\n");
    print("<td><input name=\"o2symmetry\" size=9 value=\"$o2symmetry\"></td>\n");
    print("<td><input name=\"o2phaseoffset\" size=11 value=\"$o2phaseoffset\"></td>\n");
    print("<td><input name=\"o2gain\" size=8 value=\"$o2gain\"></td></tr>\n");
    print("</table></p>\n");
    print("<p>&nbsp;</p>\n");

    // Table for vibrato / tremolo / glide
    print("<p><table border=1 cellpadding=4><tr><td>\n");
    print("<p><table border=0 cellpadding=4>\n");
    print("<tr><th colspan=6>Vibrato / Tremolo</th></tr>\n");
    print("<tr><th>Oscillator</th><th>Waveform</th><th>Frequency</th><th>Symmetry</th>");
    print("<th>Phase Offset</th><th>Depth</th></tr>\n");
    print("<tr><td>Vibrato</td><td><select name=vibtype required>");
    if ($vibtype == 0)
      print(" <option value=0 selected>Off</option>\n");
    else
      print(" <option value=0>Off</option>\n");
    if ($vibtype == 1)
      print(" <option value=1 selected>Sine</option>\n");
    else
      print(" <option value=1>Sine</option>\n");
    if ($vibtype == 2)
      print(" <option value=2 selected>Square</option>");
    else
      print(" <option value=2>Square</option>");
    if ($vibtype == 3)
      print(" <option value=3 selected>Triangle</option>\n");
    else
      print(" <option value=3>Triangle</option>\n");
    if ($vibtype == 4)
      print(" <option value=4 selected>Noise</option></select></td>\n");
    else
      print("<option value=4>Noise</option></select></td>\n");
    print("<td><input name=\"vibfreq\" size=9 value=\"$vibfreq\"></td>\n");
    print("<td><input name=\"vibsymmetry\" size=9 value=\"$vibsymmetry\"></td>\n");
    print("<td><input name=\"vibphaseoffset\" size=11 value=\"$vibphaseoffset\"></td>\n");
    print("<td><input name=\"vibdepth\" size=8 value=\"$vibdepth\"></td></tr>\n");
    print("<tr><td>Tremolo</td><td><select name=tremtype required>\n");
    if ($tremtype == 0)
      print("<option value=0 selected>Off</option>\n");
    else
      print("<option value=0>Off</option>\n");
    if ($tremtype == 1)
      print("<option value=1 selected>Sine</option>\n");
    else
      print("<option value=1>Sine</option>\n");
    if ($tremtype == 2)
      print("<option value=2 selected>Square</option>\n");
    else
      print("<option value=2>Square</option>\n");
    if ($tremtype == 3)
      print("<option value=3 selected>Triangle</option>\n");
    else
      print("<option value=3>Triangle</option>\n");
    if ($tremtype == 4)
      print("<option value=4 selected>Noise</option></select></td>\n");
    else
      print("<option value=4>Noise</option></select></td>\n");
    print("<td><input name=\"tremfreq\" size=9 value=\"$tremfreq\"></td>\n");
    print("<td><input name=\"tremsymmetry\" size=9 value=\"$tremsymmetry\"></td>\n");
    print("<td><input name=\"tremphaseoffset\" size=11 value=\"$tremphaseoffset\"></td>\n");
    print("<td><input name=\"tremdepth\" size=8 value=\"$tremdepth\"></td></tr>\n");
    print("</table></p>\n");
    print("</td><td><table border=0 cellpadding=4>\n");
    print(" <tr><th colspan=2>Glide</th></tr>\n");
    print(" <tr><th>Frequency</th><th>Milliseconds</th></tr>\n");
    print(" <tr><td><input name=\"glidefreq\" size=9 value=\"$glidefreq\"></td>\n");
    print(" <td><input name=\"glidems\" size=9 value=\"$glidems\"></td></tr>\n");
    print(" </table></td>\n");
    print("</tr></table>\n");


    // Table for ADSR envelope
    print("<p>&nbsp;</p><p><table border=1 cellpadding=0>\n");
    print("<tr><th colspan=9>ADSR Envelope (steptime=60000 for sustain)</th></tr>\n");
    print("<tr><th></th><th>Step 0</th><th>Step 1</th><th>Step 2</th><th>Step 3</th>");
    print("<th>Step 4</th><th>Step 5</th><th>Step 6</th><th>Step 7</th></tr>\n");
    print("<tr><th>Step time (ms)</th>\n");
    print("<td><input name=\"step0time\" size=6 value=\"$step0time\">\n");
    print("<td><input name=\"step1time\" size=6 value=\"$step1time\">\n");
    print("<td><input name=\"step2time\" size=6 value=\"$step2time\">\n");
    print("<td><input name=\"step3time\" size=6 value=\"$step3time\">\n");
    print("<td><input name=\"step4time\" size=6 value=\"$step4time\">\n");
    print("<td><input name=\"step5time\" size=6 value=\"$step5time\">\n");
    print("<td><input name=\"step6time\" size=6 value=\"$step6time\">\n");
    print("<td><input name=\"step7time\" size=6 value=\"$step7time\"></tr>\n");
    print("<tr><th>Step gain</th>\n");
    print("<td><input name=\"step0gain\" size=6 value=\"$step0gain\">\n");
    print("<td><input name=\"step1gain\" size=6 value=\"$step1gain\">\n");
    print("<td><input name=\"step2gain\" size=6 value=\"$step2gain\">\n");
    print("<td><input name=\"step3gain\" size=6 value=\"$step3gain\">\n");
    print("<td><input name=\"step4gain\" size=6 value=\"$step4gain\">\n");
    print("<td><input name=\"step5gain\" size=6 value=\"$step5gain\">\n");
    print("<td><input name=\"step6gain\" size=6 value=\"$step6gain\">\n");
    print("<td><input name=\"step7gain\" size=6 value=\"$step7gain\"></tr>\n");
    print("</table></p>\n");

    // Table for filters
    print("<p>&nbsp;</p>\n");
    print("<p><table border=1 cellpadding=4>\n");
    print("<tr><th colspan=3>Output Filter</th></tr>\n");
    print("<tr><th>Type</th><th>Q</th><th>Rolloff / Frequencies</th></tr>\n");
    if ($flttype == 0)
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"0\" checked> Off</td>\n");
    else
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"0\"> Off</td>\n");
    print("<td colspan=2> </td></tr>\n");
    if ($flttype == 1)
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"1\" checked> Low Pass</td>\n");
    else
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"1\"> Low Pass</td>\n");
    print("<td rowspan=4><input name=\"fltQ\" size=8 value=\"$fltQ\"></td>\n");
    print("<td rowspan=2>Rolloff: <select name=fltrolloff required>\n");
    if ($fltrolloff == 6)
      print("<option value=6 selected>6 dB</option>\n");
    else
      print("<option value=6>6 dB</option>\n");
    if ($fltrolloff == 12)
      print("<option value=12 selected>12 dB</option></select><br>\n");
    else
      print("<option value=12>12 dB</option></select><br>\n");
    print("Freq1: <input name=\"fltfreq1\" size=9 value=\"$fltfreq1\"><br></td></tr>\n");
    if ($flttype == 2)
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"2\" checked> High Pass</td>\n");
    else
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"2\"> High Pass</td>\n");
    if ($flttype == 3)
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"3\" checked> Band Pass</td>\n");
    else
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"3\"> Band Pass</td>\n");
    print("<td rowspan=2>Freq2: <input name=\"fltfreq2\" size=9 value=\"$fltfreq2\"><br></td></tr>\n");
    if ($flttype == 4)
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"4\" checked> Band Stop</td>\n");
    else
      print("<tr><td><input type=\"radio\" name=\"flttype\" value=\"4\"> Band Stop</td>\n");
    print("</table></p>\n");

    print("<p>&nbsp;</p>\n");
    print("<input type=\"submit\" value=\"Update Voices\">\n");
    print("</form>\n");

    // free the result and close the connection 
    pg_freeresult($r1);
    pg_close($c1);
?>
</body>
</html>
