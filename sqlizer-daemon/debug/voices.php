<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2//EN">
<html>
<!-- ----------------------------------------------------------------- -->
<!--  SQLizer synth                                                    -->
<!--  Copyright (C) 2032 Robert W Smith (bsmith@linuxtoys.org)         -->
<!--                                                                   -->
<!--   This program is distributed under the terms of the MIT          -->
<!--   License.  See the file COPYING file.                            -->
<!-- ----------------------------------------------------------------- -->
<head>
<title>SQLizer Voices</title>
</head>
<body>
<center><h3>SQLizer Voices</h3></center>
<?php
    // Suppress Postgres error messages
    error_reporting(error_reporting() & 0xFFFD);

    // The user has selected a port for RTA access.  This is usually
    // unique to the RTA application.
    $port = htmlentities($_GET["port"]);
    if (! $port)
        $port = 8889;    // In case it is not specified in URL.

    // connect to the synth daemon as a Postgres client
    $conn = pg_connect("host=127.0.0.1 port=$port user=anyuser");
    if ($conn == "") { 
        printf("$s%s%s", "Unable to connect to application.<br>",
            "Please verify that the application is running and ",
            "listening on port $port.<br>");
        exit();
    }

    // Headings
    print("<table border=3 cellpadding=4 align=center width=65%>\n");
    print("<tr><th> </th><th>Index</th><th>Note ID</th>\n");
    print("<th>Chord ID</th><th>V state</th>\n");
    print("<th>O1 Type</th><th>O1 Freq</th>\n");
    print("<th>O2 Type</th><th>O2 Freq</th>\n");
    print("<th>Mix Mode</th><th>Output Gain</th><th>Output Channel</th></tr>\n");

    // execute query 
    $command = "SELECT idx,noteid,chordid,vstate,o1type,o1freq,o1type,o2freq,mixmode,outputgain,outputchannel FROM voices";
    $result = pg_exec($conn, $command);
    if ($result == "") { 
        print("<p><font color=\"red\" size=+1>SQL Command failed!</p>");
        print("<p>Command: $command</p>\n");
        exit();
    }
    for($row = 0; $row < pg_NumRows($result); $row++)
    {
        $vidx       = pg_result($result, $row, 0);
        $noteid     = pg_result($result, $row, 1);
        $chordid    = pg_result($result, $row, 2);
        $vstate     = pg_result($result, $row, 3);
        $o1type     = pg_result($result, $row, 4);
        $o1freq     = pg_result($result, $row, 5);
        $o2type     = pg_result($result, $row, 6);
        $o2freq     = pg_result($result, $row, 7);
        $mixmode    = pg_result($result, $row, 8);
        $outgain    = pg_result($result, $row, 9);
        $outchan    = pg_result($result, $row, 10);
        print("<tr><td><a href=voiceedit.php?voiceidx=$vidx>Edit</a></td>\n");
        print("<td>$vidx</td>\n");
        print("<td>$noteid</td>\n");
        print("<td>$chordid</td>\n");
        print("<td>$vstate</td>\n");
        print("<td>$o1type</td>\n");
        printf("<td>%9.4f</td>\n", $o1freq);
        print("<td>$o2type</td>\n");
        printf("<td>%9.4f</td>\n", $o2freq);
        print("<td>$mixmode</td>\n");
        printf("<td>%5.4f</td>\n", $outgain);
        printf("<td>%d</td>\n", $outchan);
        print("</tr>\n");
    }

    print("</table>\n");

    // free the result and close the connection 
    pg_freeresult($result);
    pg_close($conn);
?>
</body>
</html>
