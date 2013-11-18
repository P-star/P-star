#
# Regular cron jobs for the p-star package
#
0 4	* * *	root	[ -x /usr/bin/p-star_maintenance ] && /usr/bin/p-star_maintenance
