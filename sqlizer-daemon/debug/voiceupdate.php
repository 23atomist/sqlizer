<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2//EN">
<html>
<!-- ----------------------------------------------------------------- -->
<!--  SQLizer synthesizer                                              -->
<!--  Copyright (C) 2023 Robert W Smith (bsmith@linuxtoys.org)         -->
<!--                                                                   -->
<!--   This program is distributed under the terms of the MIT          -->
<!--   License.  See the file COPYING file.                            -->
<!-- ----------------------------------------------------------------- -->
<head>
<title>Edit Row</title>
</head>
<body>
<?php

    // The user has submitted an update to a particular
    // row in a particular table.  Build and execute 
    // the UPDATE command.
    $vidx    = htmlentities(current($_POST));

    // Suppress Postgres error messages
    error_reporting(error_reporting() & 0xFFFD);

    // connect to the database 
    $c1 = pg_connect("host=127.0.0.1 port=8889 user=anyuser");
    if ($c1 == "") { 
        printf("$s%s", "Unable to connect to application.<br>",
            "Please verify that the application is running and ",
            "listening on port $port.<br>");
        exit();
    }

    // Build SQL UPDATE command.
    $command = "UPDATE voices SET ";
    $count = count($_POST) -1;     // -1 since index takes first slot
    for ($index=0; $index < $count; $index++) {
        $value=htmlentities(next($_POST));
        $key = htmlentities(key($_POST));
        if ($index > 0)
            $command = "$command, \"$key\" = \"$value\" ";
        else
            $command = "$command \"$key\" = \"$value\" ";
    }
    $command = "$command,o1phaseacc=0,o2phaseacc=0";
    $command = "$command WHERE idx=$vidx";

    // execute query 
    $r1 = pg_exec($c1, $command);
    if ($r1 == "") { 
        print("<p><font color=\"red\" size=+1>Update failed!</p>");
        print("<p>Please verify input values.</font></p>\n");
        print("<p>Command: $command</p>\n");
        exit();
    }

    // Update succeeded.  Say so.
    print("<p><font color=\"green\" size=+1>Update succeeded.");
    print("</font></p>\n<p>Command: $command</p>\n");

    // free the result and close the connection 
    pg_freeresult($r1);
    pg_close($c1);
?>
</body>
</html>
