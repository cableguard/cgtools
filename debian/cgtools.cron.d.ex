#
# Regular cron jobs for the cgtools package
#
0 4	* * *	root	[ -x /usr/bin/cgtools_maintenance ] && /usr/bin/cgtools_maintenance
