#ifndef GLOBAL_VARS_H_
#define GLOBAL_VARS_H_

#include <windows.h>

#include "dragbase.h"

// HINSTANCE of the dragbase DLL
extern HINSTANCE dll_hinst;
// HOOK for mouse messages
extern HHOOK mouse_hook;
// HOOK for other messages
extern HHOOK hook;
// Callback procedure used for saving
extern SaveFileProc save_file_proc;
// Filename and -path used for saving
extern char save_filename[1024];
// Optional parameter passed to the save procedure
extern void * param;
// Identifier for the application using dragbase
extern char app_name[1024];

/// Size of the icon
const int ICON_WIDTH = 16;
const int ICON_HEIGHT = 16;

#endif
