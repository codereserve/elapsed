# elapsed
Elapsed timer Unix/Linux-command

elapsed is a command-line utility for displaying or logging the elapsed run-time called from within scripts running on Unix, Linux, or MacOS. Will also run on Windows within a Linux sub-system or with minimal modification. Provides a simple means of outputing intervening and final elapsed times from scripts while supporting long running scripts that can run for days.

elapsed requires nothing special to be built or run, uses only standard C libraries.

May be built using simple call to gcc, as in:
<pre>     gcc elapsed.c -o elapsed</pre>

Help page:
<pre><code>
elapsed v1.0 (c) 2021 CodeReserve.com. All rights reserved.
  Usage: elapsed ACTION [NAME]
  ACTIONS:
    start [timer]   Set start time (with optional named timer).
    show [timer]    Output intermediate/final elapsed time (using named timer
                    if defined at start).
    stop [timer]    Optionally, stop (remove) previously started, optionally
                    named, timer.
  OPTIONS:
    -h, --help      Output this help message.
    -n              Do not output the trailing newline character with show
                    action.
    -r              When used with the show action and following the elapsed
                    output, this will cause the timer to restart for the
                    next show action.
  OUTPUT FORMAT:
    [0 day[s] ][0 hour[s] ][0 minute[s] ]0.000 seconds
    Examples:       1 minute 48.043 seconds
                    1 day 0 hours 17 minutes 54.941 seconds
  SEE ALSO:
    Documentation & code: https://github.com/codereserve/elapsed
  AUTHOR:
    Creator: Jesse R Luckett, The Code Reserve.
</code></pre>
 
