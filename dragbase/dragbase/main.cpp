#include "GlobalVars.h"
#include <Commctrl.h>

// First entry point of the dragbase DLL
BOOL WINAPI DllMain(
  __in  HINSTANCE hinstDLL,
  __in  DWORD fdwReason,
  __in  LPVOID lpvReserved
)
{
	// Initialize OLE
	OleInitialize(NULL);
	// Remeber hinstance of the DLL
	dll_hinst = hinstDLL;
	return TRUE;
}
