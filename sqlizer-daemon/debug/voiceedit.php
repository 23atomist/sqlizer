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
    $command = "$command step7gain";
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

    // Give URL for form processing 
    print("<form method=\"post\" action=voiceupdate.php>\n");
    print("<input type=\"hidden\" name=\"vidx\" value=\"$vidx\"></p>\n");

    // Table for oscillator #1
    print("<p><table border=0 cellpadding=4>\n");
    print("<tr><th colspan=5>Oscillator #1</th></tr>\n");
    print("<tr><th>Waveform</th><th>Frequency</th><th>Symmetry</th>");
    print("<th>Phase Offset</th><th>Gain</th></tr>\n");
    print("<tr><td><select name=o1type required>\n");
    print("<option value=1>Sine</option><option value=2>Square</option>");
    print("<option value=3>Triangle</option><option value=4>Noise</option></select></td>\n");
    print("<td><input name=\"o1freq\" size=9 value=\"$o1freq\"></td>\n");
    print("<td><input name=\"o1symmetry\" size=9 value=\"$o1symmetry\"></td>\n");
    print("<td><input name=\"o1phaseoffset\" size=11 value=\"$o1phaseoffset\"></td>\n");
    print("<td><input name=\"o1gain\" size=8 value=\"$o1gain\"></td></tr>\n");
    print("</table></p>\n");
    // Table for oscillator #2
    print("<p><table border=0 cellpadding=4>\n");
    print("<tr><th colspan=5>Oscillator #2</th></tr>\n");
    print("<tr><th>Waveform</th><th>Frequency</th><th>Symmetry</th>");
    print("<th>Phase Offset</th><th>Gain</th><th>Mix Mode</th></tr>\n");
    print("<tr><td><select name=o2type required><option value=0>Off</option>\n");
    print("<option value=1>Sine</option><option value=2>Square</option>");
    print("<option value=3>Triangle</option><option value=4>Noise</option></select></td>\n");
    print("<td><input name=\"o2freq\" size=9 value=\"$o2freq\"></td>\n");
    print("<td><input name=\"o2symmetry\" size=9 value=\"$o2symmetry\"></td>\n");
    print("<td><input name=\"o2phaseoffset\" size=11 value=\"$o2phaseoffset\"></td>\n");
    print("<td><input name=\"o2gain\" size=8 value=\"$o2gain\"></td>\n");
    print("<td><select name=mixmode required>\n");
    print("<option value=0>None</option><option value=1>Sum</option><option value=2>AM</option>");
    print("<option value=3>FM</option><option value=4>Ring</option>\n");
    print("<option value=5>HardSync</option></select></td>\n");
    print("</table></p>\n");
    // Table for vibrato
    print("<p><table border=0 cellpadding=4>\n");
    print("<tr><th colspan=5>Vibrato</th></tr>\n");
    print("<tr><th>Waveform</th><th>Frequency</th><th>Symmetry</th>");
    print("<th>Phase Offset</th><th>Depth</th></tr>\n");
    print("<tr><td><select name=vibtype required><option value=0>Off</option>\n");
    print("<option value=1>Sine</option><option value=2>Square</option>");
    print("<option value=3>Triangle</option><option value=4>Noise</option></select></td>\n");
    print("<td><input name=\"vibfreq\" size=9 value=\"$vibfreq\"></p>\n");
    print("<td><input name=\"vibsymmetry\" size=9 value=\"$vibsymmetry\"></p>\n");
    print("<td><input name=\"vibphaseoffset\" size=11 value=\"$vibphaseoffset\"></p>\n");
    print("<td><input name=\"vibdepth\" size=8 value=\"$vibdepth\"></p>\n");
    print("</table></p>\n");
    // Table for glide and voice state
    print("<p><table border=0 cellpadding=6><tr></tr><tr><td>\n");
     print(" <p><table border=0 cellpadding=4>\n");
     print(" <tr><th colspan=2>Glide</th></tr>\n");
     print(" <tr><th>Frequency</th><th>Milliseconds</th></tr>");
     print(" <td><input name=\"glidefreq\" size=9 value=\"$glidefreq\"></td></p>\n");
     print(" <td><input name=\"glidems\" size=9 value=\"$glidems\"></td></tr></p>\n");
     print(" </table></p>\n");
    print("</td><td valign=\"top\">\n");
     print(" <p><table border=0 cellpadding=4>\n");
     print(" <tr><th> </th></tr><tr><th>Voice State</th></tr>\n");
     print(" <tr><td><select name=vstate required><option value=0>Off</option>\n");
     print(" <option value=1>In Use</option><option value=2>On</option>");
     print(" <option value=3>Sustain</option></td></select>\n");
     print(" </table></p>\n");
    print("</td></tr></table>\n");
    // Table for tremolo
    print("<p><table border=0 cellpadding=4>\n");
    print("<tr><th colspan=5>Tremolo</th></tr>\n");
    print("<tr><th>Waveform</th><th>Frequency</th><th>Symmetry</th>");
    print("<th>Phase Offset</th><th>Depth</th></tr>\n");
    print("<tr><td><select name=tremtype required><option value=0>Off</option>\n");
    print("<option value=1>Sine</option><option value=2>Square</option>");
    print("<option value=3>Triangle</option><option value=4>Noise</option></select></td>\n");
    print("<td><input name=\"tremfreq\" size=9 value=\"$tremfreq\">\n");
    print("<td><input name=\"tremsymmetry\" size=9 value=\"$tremsymmetry\">\n");
    print("<td><input name=\"tremphaseoffset\" size=11 value=\"$tremphaseoffset\">\n");
    print("<td><input name=\"tremdepth\" size=8 value=\"$tremdepth\">\n");
    print("</table></p>\n");
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


    print("<p>&nbsp;</p>\n");
    print("<input type=\"submit\" value=\"Update Voices\"></p>\n");
    print("</form>\n");

    // free the result and close the connection 
    pg_freeresult($r1);
    pg_close($c1);
?>
</body>
</html>
