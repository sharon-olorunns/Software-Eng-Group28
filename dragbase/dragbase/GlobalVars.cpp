#include "GlobalVars.h"

// HINSTANCE of the dragbase DLL
HINSTANCE dll_hinst;
// HOOK for mouse messages
HHOOK mouse_hook;
// HOOK for other messages
HHOOK hook;
// Callback procedure used for saving
SaveFileProc save_file_proc;
// Filename and -path used for saving
char save_filename[1024];
// Optional parameter passed to the save procedure
void * param;
// Identifier for the application using dragbase
char app_name[1024];
