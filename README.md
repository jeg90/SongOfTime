SongOfTime
==========

A command line Time Management/Tracker Utility.
Version: 1.2 Release
Created by: Jason Gerlowski
University of Pittsburgh
jeg90@pitt.edu

STATUS
------
Version 1.2 in branch 'master' on github.
-s option implemented
-f option implemented
-l option implemented
-r option implemented
-c option implemented
-d option implemented
-R option implemented
-T option implemented

For information on these options and their usage, use 'man songoftime'

Additionally, reports can be viewed by opening the '.timeSong.html' file
(located in the user's home directory) with a web browser.

Several things I'm considering adding in the future (after another usage iteration):
1) -o opens .timeSong.html in web browser automatically
2) -c clears all history
3) "-m Task1 Task2" merges the times for two already existing tasks
	(potentially useful for spelling errors).

GOALS
-----
There's plenty of time management software out there.  Most of
the software is pretty good, and most of it is pretty easy to use.
But for some reason, it's not very frequently used.  This is normally
because it is inconvenient.  Most users don't want to keep open
a new tab while they're working.  Even fewer want to do this for 
programs they'll user briefly, and then ignore for the majority
of their work, only to use briefly again on finishing.

This makes many time management utilities unwieldy outside of a
commercial or work setting.

A better time tracker can be built by keeping it integrated with a
window or program most programmers already use (i.e. the terminal).
This makes it not easier to use, but more convenient to use frequently.

This is what I'm hoping to capture with SongOfTime, a straightforward
command-line based Time Management utility.

USAGE
-----
SongOfTime usage (when completed) will be simple.

To start tracking time spent on a task:
songoftime -s "TaskName"

To stop tracking time spend on a task (or 'finish' it):
songoftime -f

These commands will keep track of timing information in a file stored 
in a hidden file(s) in the users home directory.

Statistics can then be viewed in the console using:
songoftime -r

Additional options are discussed and documented in the songoftime manpage
