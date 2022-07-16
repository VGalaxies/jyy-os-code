#include <errno.h>
#include <spawn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define errExitEN(en, msg)                                                     \
  do {                                                                         \
    errno = en;                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

char **environ;

int main(int argc, char *argv[]) {
  pid_t child_pid;
  int s, opt, status;
  sigset_t mask;
  posix_spawnattr_t attr;
  posix_spawnattr_t *attrp;
  posix_spawn_file_actions_t file_actions;
  posix_spawn_file_actions_t *file_actionsp;

  /* Parse command-line options, which can be used to specify an
     attributes object and file actions object for the child. */

  attrp = NULL;
  file_actionsp = NULL;

  while ((opt = getopt(argc, argv, "sc")) != -1) {
    switch (opt) {
    case 'c': /* -c: close standard output in child */

      /* Create a file actions object and add a "close"
         action to it. */

      s = posix_spawn_file_actions_init(&file_actions);
      if (s != 0)
        errExitEN(s, "posix_spawn_file_actions_init");

      s = posix_spawn_file_actions_addclose(&file_actions, STDOUT_FILENO);
      if (s != 0)
        errExitEN(s, "posix_spawn_file_actions_addclose");

      file_actionsp = &file_actions;
      break;

    case 's': /* -s: block all signals in child */

      /* Create an attributes object and add a "set signal mask"
         action to it. */

      s = posix_spawnattr_init(&attr);
      if (s != 0)
        errExitEN(s, "posix_spawnattr_init");
      s = posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGMASK);
      if (s != 0)
        errExitEN(s, "posix_spawnattr_setflags");

      sigfillset(&mask);
      s = posix_spawnattr_setsigmask(&attr, &mask);
      if (s != 0)
        errExitEN(s, "posix_spawnattr_setsigmask");

      attrp = &attr;
      break;
    }
  }

  /* Spawn the child. The name of the program to execute and the
     command-line arguments are taken from the command-line arguments
     of this program. The environment of the program execed in the
     child is made the same as the parent's environment. */

  s = posix_spawnp(&child_pid, argv[optind], file_actionsp, attrp,
                   &argv[optind], environ);
  if (s != 0)
    errExitEN(s, "posix_spawn");

  /* Destroy any objects that we created earlier. */

  if (attrp != NULL) {
    s = posix_spawnattr_destroy(attrp);
    if (s != 0)
      errExitEN(s, "posix_spawnattr_destroy");
  }

  if (file_actionsp != NULL) {
    s = posix_spawn_file_actions_destroy(file_actionsp);
    if (s != 0)
      errExitEN(s, "posix_spawn_file_actions_destroy");
  }

  printf("PID of child: %jd\n", (intmax_t)child_pid);

  /* Monitor status of the child until it terminates. */

  do {
    s = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
    if (s == -1)
      errExit("waitpid");

    printf("Child status: ");
    if (WIFEXITED(status)) {
      printf("exited, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("killed by signal %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
      printf("stopped by signal %d\n", WSTOPSIG(status));
    } else if (WIFCONTINUED(status)) {
      printf("continued\n");
    }
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  exit(EXIT_SUCCESS);
}
