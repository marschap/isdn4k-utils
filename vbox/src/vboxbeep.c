
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <paths.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#include "libvbox.h"
#include "runtime.h"

#define MAX_MESSAGE_BOXES	10


struct messagebox
{
	char		*name;
	time_t	 time;
};

static struct messagebox messageboxes[MAX_MESSAGE_BOXES];

static char sound_available_hours[24];

static char	  *vbasename	= NULL;
static char	  *pidname		= NULL;
static int	 	killmode		= 0;
static time_t	starttime	= 0;

static struct option arguments[] =
{
	{ "version"		, no_argument			, NULL, 'v' },
	{ "help"			, no_argument			, NULL, 'h' },
	{ "kill"			, no_argument			, NULL, 'k' },
	{ "sound" 		, required_argument	, NULL, 's' },
	{ NULL			, 0						, NULL, 0   }
};




static void lets_hear_the_sound(void);
static int	add_dir_to_messagebox(int, char *);
static void	free_all_messageboxes(void);
static time_t get_newest_message_time(char *);
static void beep(int);
static int get_pid_from_file(void);
static void remove_pid_file(void);
static void create_pid_file(void);
static void leave_program(int);
static void set_new_starttime(int);
static void free_resources(void);
static void init_signals();


/*************************************************************************
 ** 
 *************************************************************************/

int main(int argc, char **argv)
{
	int i;
	int b;
	int opts;

	if (!(vbasename = rindex(argv[0], '/')))
	{
		vbasename = argv[0];
	}
	else vbasename++;

	for (i = 0; i < MAX_MESSAGE_BOXES; i++)
	{
		messageboxes[i].name = NULL;
		messageboxes[i].time = 0;
	}

	if (!(pidname = malloc(strlen(PIDFDIR) + strlen("/vboxbeep.pid") + 1)))
	{
		fprintf(stderr, "%s: not enough memory to create pid name.\n", vbasename);
		
		exit(5);
	}

	printstring(pidname, "%s/vboxbeep.pid", PIDFDIR);

	killmode		= 0;
	starttime	= 0;

	b=0;
	if (add_dir_to_messagebox(b, "/var/spool/vbox/michael/incoming")) b++;
	if (add_dir_to_messagebox(b, "/var/spool/vbox/nicole/incoming" )) b++;

	b = 0;

	while ((opts = getopt_long(argc, argv, "vhk", arguments, (int *)0)) != EOF)
	{
		switch (opts)
		{
			case 'k':
				killmode = 1;
				break;
		}
	}

	init_signals();

	i = get_pid_from_file();

	if (i)
	{
		if (killmode == 0)
		{
			fprintf(stderr, "%s: sending signal to stop beep's to process %d...\n", vbasename, i);

			if (kill(i, SIGUSR1) != 0)
			{
				fprintf(stderr, "%s: error sending signal (%s).\n", vbasename, strerror(errno));
			}
		}
		else
		{
			if (getuid() == 0)
			{
				fprintf(stderr, "%s: sending terminate signal to process %d...\n", vbasename, i);

				if (kill(i, SIGTERM) != 0)
				{
					fprintf(stderr, "%s: error sending signal (%s).\n", vbasename, strerror(errno));
				}
			}
			else
			{
				fprintf(stderr, "%s: you must be *root* to kill a running %s.\n", vbasename, vbasename);
			}
		}

		free_resources();
		
		exit(1);
	}

	if (killmode == 1)
	{
		fprintf(stderr, "%s: can't find running %s process.\n", vbasename, vbasename);

		free_resources();
		
		exit(5);
	}

	if (getuid() != 0)
	{
		fprintf(stderr, "%s: you must be *root* to start %s.\n", vbasename, vbasename);

		free_resources();
		
		exit(5);
	}
	
	create_pid_file();

	while (TRUE)
	{
		for (i = 0; i < MAX_MESSAGE_BOXES; i++)
		{
			if (messageboxes[i].name)
			{
				messageboxes[i].time = get_newest_message_time(messageboxes[i].name);
				
				if (messageboxes[i].time > starttime)
				{
					lets_hear_the_sound();
					
					break;
				}
			}
		}

		xpause(5000);
	}

	leave_program(0);
}

/*************************************************************************
 ** 
 *************************************************************************/

static void init_signals(void)
{
	signal(SIGTERM	, leave_program);
	signal(SIGHUP	, leave_program);
	signal(SIGUSR1	, set_new_starttime);
	signal(SIGQUIT	, SIG_IGN);
	signal(SIGINT	, SIG_IGN);
}

/*************************************************************************
 ** 
 *************************************************************************/

static void free_resources(void)
{
	if (pidname) free(pidname);

	pidname = NULL;
	
	free_all_messageboxes();
}

/*************************************************************************
 ** 
 *************************************************************************/

static void leave_program(int sig)
{
	remove_pid_file();
	free_resources();

	exit(0);
}

/*************************************************************************
 ** 
 *************************************************************************/

static void set_new_starttime(int sig)
{
	init_signals();

	starttime = time(NULL);
}

/*************************************************************************
 ** 
 *************************************************************************/

static void lets_hear_the_sound(void)
{
	struct tm *timel;

	time_t	timec;
	int		vt;
	int		i;
	int		k;

	timec = time(NULL);

	if (!(timel = localtime(&timec)))
	{
		fprintf(stderr, "%s: can't get current local time.\n", vbasename);
		
		return;
	}

	if ((vt = open("/dev/console", O_RDWR)) == -1)
	{
		fprintf(stderr, "%s: can't open \"/dev/console\" (%s).\n", vbasename, strerror(errno));
		
		return;
	}

	beep(vt);
	beep(vt);
	beep(vt);

	close(vt);
}

/*************************************************************************
 ** 
 *************************************************************************/

static void beep(int vt)
{
	ioctl(vt, KIOCSOUND, 3200);

	xpause(100);

	ioctl(vt, KIOCSOUND, 0);

	xpause(25);
}

/*************************************************************************
 ** 
 *************************************************************************/

static int add_dir_to_messagebox(int nr, char *box)
{
	char *name;

	if (nr < MAX_MESSAGE_BOXES)
	{
		if ((name = strdup(box)))
		{
			messageboxes[nr].name = name;
			messageboxes[nr].time = time(NULL);
			
			returnok();
		}
	}

	fprintf(stderr, "%s: can't add \"%s\" to checklist.\n", vbasename, box);

	returnerror();
}

/*************************************************************************
 ** 
 *************************************************************************/

static void free_all_messageboxes(void)
{
	int i;
	
	for (i = 0; i < MAX_MESSAGE_BOXES; i++)
	{
		if (messageboxes[i].name) free(messageboxes[i].name);
			
		messageboxes[i].name = NULL;
	}
}

/*************************************************************************
 ** 
 *************************************************************************/

static time_t get_newest_message_time(char *box)
{
	struct dirent *entry;
	struct stat		status;

	DIR	  *dir		= NULL;
	time_t	newest	= 0;

	if (chdir(box) == 0)
	{
		if ((dir = opendir(".")))
		{
			while ((entry = readdir(dir)))
			{
				if (strcmp(entry->d_name, "." ) == 0) continue;
				if (strcmp(entry->d_name, "..") == 0) continue;

				if (stat(entry->d_name, &status) == 0)
				{
					if (status.st_mtime > newest) newest = status.st_mtime;
				}
			}

			closedir(dir);
		}
		else fprintf(stderr, "%s: can't open \"%s\" (%s).\n", vbasename, box, strerror(errno));
	}
	else fprintf(stderr, "%s: can't change to \"%s\" (%s).\n", vbasename, box, strerror(errno));

	return(newest);
}

/*************************************************************************
 ** 
 *************************************************************************/

static void create_pid_file(void)
{
	FILE *pid;

	if (!(pid = fopen(pidname, "w")))
	{
		fprintf(stderr, "%s: could not create pid file \"%s\" (%s).\n", vbasename, pidname, strerror(errno));

		leave_program(0);
	}

	fprintf(pid, "%d\n", getpid());

	fclose(pid);
}

/*************************************************************************
 ** 
 *************************************************************************/

static void remove_pid_file(void)
{
	if (unlink(pidname) != 0)
	{
		fprintf(stderr, "%s: can't remove pid file \"%s\" (%s).\n", vbasename, pidname, strerror(errno));
	}
}

/*************************************************************************
 ** 
 *************************************************************************/

static int get_pid_from_file(void)
{
	char	temp[32];
	FILE *pid;

	if ((pid = fopen(pidname, "r")))
	{
		fgets(temp, 32, pid);
		fclose(pid);

		return(atoi(temp));
	}

	return(0);
}

/*************************************************************************
 ** 
 *************************************************************************/

static void parse_numbers(char *string, int min, int max, char field[])
{
	char *number;
	char *beg;
	char *end;
	int	begnr;
	int	endnr;
	int	i;
	
	number = strtok(string, ",;:");
	
	while (number)
	{
		beg = number;

		if ((end = index(number, '-')))
		{
			*end++ = '\0';
		}
		else end = beg;
		
		begnr = atoi(beg);
		endnr = atoi(end);

		if ((begnr >= min) && (begnr <= max) && (endnr >= min) && (endnr <= max))
		{
			if (endnr >= begnr)
			{
				for (i = begnr; i <= endnr; i++)
				{
fprintf(stderr, "Set: %d\n", i);

					field[i] = 1;
				}
			}
		
		}		

		number = strtok(NULL, ",;:");
	}

	free(string);
}
