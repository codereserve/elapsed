/*
 * ELAPSED command-line utility
 * (c) 2021 Jesse R Luckett, The Code Reserve. All rights reserved.
 * https://www.codereserve.com
 *
 * elapsed is a command-line utility for displaying or logging the elapsed
 * run-time of actions called from within scripts running on Unix, Linux,
 * or MacOS. Will also run on Windows within a Linux sub-system or with
 * minimal modification. Provides a simple means of outputing intervening
 * and final elapsed times from scripts while supporting long running
 * scripts that can run for days.
 *
 * elapsed requires nothing special to be built or run, uses only standard
 * C libraries.
 *
 * May be built using simple call to gcc, as in:
 *      gcc elapsed.c -o elapsed
 *
 * elapsed is free software, available under the terms of the [GNU] General
 * Public License as published by the Free Software Foundation,
 * version 3 of the License (or later version). For more information
 * see http://www.gnu.org/licenses/
 *
 *
 * Revision History:
 *   2021-05-22 v1.0/J.Luckett - Initial code.
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#define COPYRIGHT "2021"
#define RELEASE "1.0"
#define DEFAULTTIMER "timer$tart"
#define TIMEREXTENSION "elapsed"
#define DEFAULTPATH "/tmp"
#define STATICRETURNBUFFERS 3
#define MAXTIMERNAME 2048
#define MAXINPUTLENGTH 64

char timer[MAXTIMERNAME + 2] = "";
char newline[3] = "\n";
struct stat buf;

/*
 * Uppercase a string. (Supports multiple static return-buffers.)
 *
 */
char *upcase(char *str)
{
  static char up[STATICRETURNBUFFERS][MAXINPUTLENGTH + 2];
  static int buf = STATICRETURNBUFFERS;
  int i;
  if (++buf >= STATICRETURNBUFFERS)
    buf = 0;
  for (i=0; str && str[i]; i++)
    up[buf][i] = toupper(str[i]);
  up[buf][i] = 0;
  return up[buf];
}

/*
 * Returns current date-time in milliseconds.
 * 
 * (64-bit value.)
 *
 */
long long now()
{
  struct timespec spec;
  long long ms;
  clock_gettime(CLOCK_REALTIME, &spec);
  ms = round(spec.tv_nsec / 1.0e6);      // nanoseconds to milliseconds
  return (spec.tv_sec * 1000) + ms;
}

/*
 * Generates start-timer path/name.
 * 
 * Default path is /tmp unless given name begins with a slash '/', then
 * the given absolute path is used instead.
 *
 */
char* timerName(char *name)
{
  static char filename[MAXTIMERNAME + 64];
  if (name && name[0])
  {
    if (name[0] == '/')
    {
      strcpy(filename, name);
    }
    else
    {
      strcpy(filename, DEFAULTPATH);
      strcat(filename, "/");
      strcat(filename, name);
    }
  }
  else
  {
    strcpy(filename, DEFAULTPATH);
    strcat(filename, "/");
    strcat(filename, DEFAULTTIMER);
  }
  strcat(filename, ".");
  strcat(filename, TIMEREXTENSION);
  return filename;
}

/*
 * Checks for start-timer existence and basic validity.
 *
 */
int timerExists(char *name)
{
  int exists = stat(timerName(name), &buf) ? 0 : 1;
  if (exists && buf.st_size != 8)
  {
    remove(timerName(name));
    printf("%s start-timer found but is %s. Timer removed.\n",
      (name && name[0] ? name : "default"),
      (buf.st_size == 0 ? "empty" : "corrupted"));
    exit(3);
  }
  return exists;
}

/*
 * Starts or restarts timer. (Creates start-time file.)
 *
 */
void startTimer(char *name)
{
  long long t = now();
  FILE *fp = fopen(timerName(name), "w");
  if ( fp )
  {
    size_t sz = fwrite( &t, 8, 1, fp );
    fclose(fp);
  }
}

/*
 * Returnes value from the start-timer.
 *
 */
long long getTimer(char *name)
{
  if (timerExists(name))
  {
    FILE *fp = fopen(timerName(name), "r");
    if ( fp )
    {
      long long t;
      size_t sz = fread( &t, 8, 1, fp );
      fclose(fp);
      return sz == 0 ? 0L : t;
    }
  }
  else
  {
    if (name && name[0])
      printf("Timer named '%s' was not started.\n", name);
    else
      printf("Default (unnamed) timer was not started.\n");
    exit(2);
  }
  return 0L;
}

/*
 * Outputs current elapsed time to stdout.
 *
 */
void showTimer(long long startTime)
{
  if (startTime == 0L)
  {
    printf("No start-time set%s%s\n", (timer[0] ? " for " : ""), timer);
    exit(1);
  }
  long long ms = now() - startTime;
  long seconds = ms / 1000L % 60L;
  long minutes = ms / 60000L % 60L;
  long hours = ms / 3600000L % 24L;
  long days = ms / 86400000L;
  if (days > 0L)
    printf("%'ld day%s ", days, (days == 1 ? "" : "s" ));
  if (days > 0L || hours > 0L)
    printf("%ld hour%s ", hours, (hours == 1 ? "" : "s"));
  if (days > 0L || hours > 0L || minutes > 0L)
    printf("%ld minute%s ", minutes, (minutes == 1 ? "" : "s"));
  printf("%ld.%03lld seconds%s", seconds, (ms %1000L), newline);
}

/*
 * Stops elapsed timer (removes the start-time file).
 *
 */
void stopTimer(char *name)
{
  if (timerExists(name))
    remove(timerName(name));
  else
  {
    if (name && name[0])
      printf("Timer named '%s' was not found.\n", name);
    else
      printf("Default (unnamed) timer was not found.\n");
    exit(2);
  }
}

/*
 * Outputs help message.
 *
 */
void showHelp()
{
  printf("\nelapsed v%s (c) %s The Code Reserve. All rights reserved.\n\
  Usage: elapsed ACTION [NAME]\n\
  ACTIONS:\n\
    start [timer]   Set start time (with optional named timer).\n\
    show [timer]    Output intermediate/final elapsed time (using named timer\n\
                    if defined at start).\n\
    stop [timer]    Optionally, stop (remove) previously started, optionally\n\
                    named, timer.\n\
  OPTIONS:\n\
    -h, --help      Output this help message.\n\
    -n              Do not output the trailing newline character with show\n\
                    action.\n\
    -r              When used with the show action and following the elapsed\n\
                    output, this will cause the timer to restart for the\n\
                    next show action.\n\
  OUTPUT FORMAT:\n\
    [0 days ][0 hours ][0 minutes ]0.000 seconds\n\
    Examples:       1 minute 48.043 seconds\n\
                    1 day 0 hours 17 minutes 54.941 seconds\n\
  SEE ALSO:\n\
    Documentation & code: https://github.com/codereserve/elapsed\n\
  AUTHOR:\n\
    Creator: Jesse R Luckett, The Code Reserve, codereserve.com.\n\
", RELEASE, COPYRIGHT);
  exit(1);
}

/*
 * Main. Command-line entry.
 *
 */
int main (int ac, char *av[])
{
  int arg, i, restart = 0;
  char cmd[MAXINPUTLENGTH] = "";

  // Acquire and parse command-line arguments ..
  for(arg = 1; arg < ac;)
  {
    // Process -options ..
    if (av[arg][0] == '-')
    {
      if (av[arg][1] == '-')
      {
        if (strcmp(av[arg], "--help") == 0)
          showHelp();
        else
        {
          printf("Unexpected option: %s (Terminated, see -h for help)\n",
            av[arg]);
          exit(1);
        }
      }
      else
      {
        for(i=1; av[arg][i]; i++)
        {
          switch (av[arg][i])
          {
            case 'n':
              newline[0] = 0;
              break;
            case 'r':
              restart = 1;
              break;
            case '?':
            case 'h':
              showHelp();
            default:
              printf("Unexpected option: -%c (Terminated, see -h for help)\n",
                av[arg][i]);
              exit(1);
          }
        }
      }
    }
    else
    {
      if (!cmd[0])
      {
        // Save the given command ..
        strncpy(cmd, upcase(av[arg]), MAXINPUTLENGTH);
      }
      else if (!timer[0])
      {
        // Save the given timer name ..
        strncpy(timer, av[arg], MAXTIMERNAME);
      }
      else
      {
        printf("Unexpected argument: %s (Terminated, see -h for help)\n",
          av[arg]);
        exit(1);
      }
    }
    arg++;
  }

  if (ac < 2)
  {
    printf("Argument(s) expected. See -h for help\n");
    exit(1);
  }
  else
  {
    // Execute given command ..
    if (strcmp(cmd, "START") == 0)
    {
      startTimer(timer);
    }
    else if (strcmp(cmd, "SHOW") == 0)
    {
      showTimer(getTimer(timer));
      if (restart)
        startTimer(timer);
    }
    else if (strcmp(cmd, "STOP") == 0)
    {
      stopTimer(timer);
    }
    else
    {
      printf("Unknown command: %s (Terminated, see -h for help)\n",
        cmd);
    }
    return(0);
  }
}
