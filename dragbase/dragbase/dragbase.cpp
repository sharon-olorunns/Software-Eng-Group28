#include "dragbase.h"
#include "ControlProc.h"
#include "GlobalVars.h"



EXTERNC DLLEXPORT unsigned __stdcall Create(HWND hwnd, SaveFileProc save_file_proc,
						const char * const app_name, void * param)
{
	if(hwnd == NULL || save_file_proc == NULL || app_name == NULL)
		return DBE_INVALIDPARAMS;

	// Register class for our (invisible) dragbase control
	const char class_name[] = "IconControl109283";
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = ControlProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = (HINSTANCE)GetWindowLongPtr (hwnd, GWLP_HINSTANCE);
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = class_name;

	if(!RegisterClass (&wc))
		return DBE_CLASSREGISTRATIONFAILED;

	// Create control
	HWND control_hwnd = CreateWindow (class_name, "", WS_CHILD, 
		0, 0, 0, 0, hwnd, NULL, wc.hInstance, NULL);
	// And show it (although it's not visible)
	ShowWindow(control_hwnd,SW_SHOW);

	if(!control_hwnd)
		return DBE_WINDOWCREATIONFAILED;

	// Install hooks
	mouse_hook = SetWindowsHookEx(WH_MOUSE,MouseProc,dll_hinst,GetCurrentThreadId());

	if(mouse_hook == NULL)
		return DBE_MOUSEHOOKFAILED;

	hook = SetWindowsHookEx(WH_CALLWNDPROC,HookProc,dll_hinst,GetCurrentThreadId());

	if(hook == NULL)
		return DBE_HOOKFAILED;

	// Save important stuff
	::save_file_proc = save_file_proc;
	::param = param;
	strcpy(::app_name,app_name);

	// Specify the default file name which is used when the user doesn't call SetFilename
	SetFilename("dragbase.tmp");

	return DBE_OK;
}


// Setzt den Dateinamen, mit dem die Datei im temporären Ordner gespeichert wird
EXTERNC DLLEXPORT void __stdcall SetFilename(const char * const filename)
{
	// Create the complete file path
	// Get temporary path and add the filename at the end of it
	GetTempPath(1024,save_filename);
	strcat(save_filename,filename);
}