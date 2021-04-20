#pragma once

void errorMsg(const char *format, ...);
void errorExit(const char *format, ...);
void errorNumExit(int errorNum, const char *format, ...);
void errorExitImmediate(const char *format, ...);
void fatal(const char *format, ...);
void usageError(const char *format, ...);
void cmdlineError(const char *format, ...);
