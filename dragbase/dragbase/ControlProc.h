#include <windows.h>

// Window procedure of our dragbase Control
extern LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
// Normal hook
extern LRESULT CALLBACK HookProc(__in  int ncode, __in  WPARAM wparam, __in  LPARAM lparam);
// Mouse hook
extern LRESULT CALLBACK MouseProc(__in  int ncode, __in  WPARAM wparam, __in  LPARAM lparam);