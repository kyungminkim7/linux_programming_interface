#pragma once

#include <signal.h>
#include <stdio.h>

void printSigSet(FILE *file, const char *prefix, const sigset_t *sigSet);
int printSigMask(FILE *file, const char *msg);
int printPendingSigs(FILE *file, const char *msg);
