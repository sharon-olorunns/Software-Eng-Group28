#include "ControlProc.h"
#include "GlobalVars.h"
#include "DropSource.h"
#include "DataObject.h"
#include "resource.h"

#include <Shlobj.h>
#include <Windowsx.h>
#include <cstring>

using namespace std;

// Interfaces used for Drag-and-Drop
IDropSource *pDropSource =  NULL;
IDataObject *pDataObject = NULL;
// Rectangle which describes position and size of the icon (window-relative coordinates)
RECT icon_rect;
// Control-Window and Window of the application using the icon
HWND this_hwnd, parent_hwnd;
// Menu where the icon is placed
HMENU hmenu;
// Bitmaps of the icon
HBITMAP hbitmap; 
BITMAP bitmap ; 


// Creates the data object used for Drag-and-Drop
IDataObject * CreateDataObject()
{
	// Store the content as HGLOBAL
	FORMATETC fmtetc = 
	{ 
		CF_HDROP,  
		0, 
		DVASPECT_CONTENT, 
		-1, 
		TYMED_HGLOBAL
	};

	STGMEDIUM stgmed =
	{
	TYMED_HGLOBAL,
	{ 0 },
	0
	};
		
	// Alloc memory and copy the file path
	stgmed.hGlobal = GlobalAlloc(GMEM_FIXED,sizeof(DROPFILES) + strlen(save_filename) + 2); 
	memcpy((char*)stgmed.hGlobal + sizeof(DROPFILES), save_filename, strlen(save_filename) + 1);
	// Add an additional 0 at the end (end of the list when multiple files are used)
	((char*)stgmed.hGlobal)[sizeof(DROPFILES) + strlen(save_filename) + 1] = 0;

	DROPFILES * dropfiles = (DROPFILES*)stgmed.hGlobal;
	dropfiles->pFiles = sizeof(DROPFILES);
	dropfiles->fWide = 0;

	return new DataObject(&fmtetc,&stgmed,1);
}



// Hook procedure
LRESULT CALLBACK HookProc(
  __in  int ncode,
  __in  WPARAM wparam,
  __in  LPARAM lparam
)
{
	if(ncode < 0)
		return CallNextHookEx(NULL,ncode,wparam,lparam);

	CWPSTRUCT  * cwps = (CWPSTRUCT *)lparam;
	
	switch(cwps->message)
	{
	default: break;
	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT * dis = ((DRAWITEMSTRUCT*)cwps->lParam);
			if(dis->CtlType == ODT_MENU && dis->itemID == 9929)
			{
				// Window size changed or window was moved
				// --> adjust the icon rectangle
				RECT window_rect; // rectangle describing the window in screen coordinates
				GetWindowRect(parent_hwnd,&window_rect);
				GetMenuItemRect(parent_hwnd,GetMenu(parent_hwnd),GetMenuItemCount(GetMenu(parent_hwnd))-1,&icon_rect);
				// Use the top position of the menu item and center the item vertically in the menu bar
				icon_rect.top = icon_rect.top - window_rect.top +  (GetSystemMetrics(SM_CYMENU)-ICON_HEIGHT) /2;
				// Place the icon on the right side of the menu (distance between right edge of the icon and the frame is 5 pixels)
				icon_rect.left = window_rect.right - window_rect.left - ICON_WIDTH- GetSystemMetrics(SM_CXSIZEFRAME)-5;
				// Complete the right and low edge
				icon_rect.right = icon_rect.left + ICON_WIDTH;
				icon_rect.bottom = icon_rect.top + ICON_HEIGHT;

				// Menu item should be drawn
				// Copy the icon to the appropriate place
				HDC hdc_mem = CreateCompatibleDC(dis->hDC);
				HBITMAP hbmold = (HBITMAP)SelectObject(hdc_mem,hbitmap);
				BitBlt(dis->hDC,icon_rect.left,icon_rect.top,ICON_WIDTH,ICON_HEIGHT,hdc_mem,0,0,SRCCOPY);
				SelectObject( hdc_mem, hbmold );
				DeleteDC( hdc_mem );
			}
			
		} break;

	}
	return CallNextHookEx(NULL,ncode,wparam,lparam);
}

// Mouse hook
LRESULT CALLBACK MouseProc(
  __in  int ncode,
  __in  WPARAM wparam,
  __in  LPARAM lparam
)
{
	// Left mouse button down?
	static bool left_down = false;
	// Time stamp when the last hint message was shown
	static unsigned last_message_shown = 0;

	if(ncode < 0)
		return CallNextHookEx(NULL,ncode,wparam,lparam);


	switch(wparam)
	{
	// Right mouse button pressed
	case WM_NCRBUTTONDOWN:
		{
			
			// Transform screen coordinates into window coordinates
			RECT window_rect;
			GetWindowRect(parent_hwnd,&window_rect);
			MOUSEHOOKSTRUCT * mhs = (MOUSEHOOKSTRUCT*)lparam;
			POINT p = mhs->pt;
			p.x -= window_rect.left;
			p.y -= window_rect.top;

			// Mouse in the rectangle?
			if(PtInRect(&icon_rect,p) == TRUE)
			{
				// Show context menu
				TrackPopupMenu(hmenu,TPM_LEFTBUTTON,mhs->pt.x,mhs->pt.y,0,this_hwnd,NULL);
			} return 1;
		} break;
	// Left mouse button pressed
	case WM_NCLBUTTONDOWN:
		{
			// Transform screen coordinates into window coordinates
			RECT window_rect;
			GetWindowRect(parent_hwnd,&window_rect);
			MOUSEHOOKSTRUCT * mhs = (MOUSEHOOKSTRUCT*)lparam;
			mhs->pt.x -= window_rect.left;
			mhs->pt.y -= window_rect.top;
			
			// Mouse in the rectangle?
			if(PtInRect(&icon_rect,mhs->pt) == TRUE)
			{
				left_down = true;
				// Create our data object (but not the file!)
				delete pDataObject;
				pDataObject = CreateDataObject();
				return 1;
			}

		
		} break;
	// Left mouse button released
	case WM_NCLBUTTONUP:
		{
			// Transform screen coordinates into window coordinates
			left_down = false;
			RECT window_rect;
			GetWindowRect(parent_hwnd,&window_rect);
			MOUSEHOOKSTRUCT * mhs = (MOUSEHOOKSTRUCT*)lparam;
			mhs->pt.x -= window_rect.left;
			mhs->pt.y -= window_rect.top;
		
			// Mouse in the rectangle and enough time (2000 ms) since the last hint message was shown?
			if(PtInRect(&icon_rect,mhs->pt) == TRUE && GetTickCount()-last_message_shown > 2000)
			{
				// Show the hint and remember the currrent time
				last_message_shown = GetTickCount();
				MessageBox(parent_hwnd,"Please drag-and-drop this icon to another application in order to submit the currently opened file to that other application. Do a right-click on the dragbase for more information.","dragbase",MB_ICONINFORMATION);
				
			}
		} break;
	// Mouse was moved
	case WM_NCMOUSEMOVE:
		{
			// Transform screen coordinates into window coordinates
			RECT window_rect;
			GetWindowRect(parent_hwnd,&window_rect);
			MOUSEHOOKSTRUCT * mhs = (MOUSEHOOKSTRUCT*)lparam;
			mhs->pt.x -= window_rect.left;
			mhs->pt.y -= window_rect.top;
		
			// Mouse in the rectangle and mouse button pressed?
			if(PtInRect(&icon_rect,mhs->pt) == TRUE && pDataObject && left_down)
			{
				// Do drag and drop stuff
				DWORD res;
				DoDragDrop(pDataObject,pDropSource,DROPEFFECT_COPY,&res);
				left_down = false;
				return 1;
			}
			else
				// Mouse left icon area -> mark button as release because we don't receive a
				// buttonup message in this case
				left_down = false;

		} break;

	}
	return CallNextHookEx(NULL,ncode,wparam,lparam);
}


// Window procedure for the about dialog
INT_PTR CALLBACK AboutProc(
  __in  HWND hwndDlg,
  __in  UINT uMsg,
  __in  WPARAM wParam,
  __in  LPARAM lParam
)
{
	// Font used for url fonts
	static HFONT hf;

	switch(uMsg)
	{
	// Dialog opened
	case WM_INITDIALOG:
		{
			// Use underlined font for the url fonts
			hf = CreateFont(12,0,0,0,0,FALSE,TRUE,FALSE,0,0,0,0,0,"MS Shell Dlg");
			SendMessage(GetDlgItem(hwndDlg,IDC_BEELINED_LINK),WM_SETFONT,(WPARAM)hf,TRUE);
			SendMessage(GetDlgItem(hwndDlg,IDC_DRAGBASE_LINK),WM_SETFONT,(WPARAM)hf,TRUE);
			
		} return TRUE;
	// Dialog was closed
	case WM_DESTROY:
		{
			// Destroy the custom font
			DeleteObject(hf);
		} break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDOK:
			case IDCANCEL:
				{
					// OK / Cancel button pressed -> close dialog
					EndDialog(hwndDlg,TRUE);
				} return TRUE;
			case IDC_DRAGBASEICON:
			case IDC_DRAGBASE_LINK:
				{
					// Link on the big dragbase logo
					char hp[1024];
					strcpy(hp,"http://www.dragbase.com/");
					strcat(hp,app_name);
					strcat(hp,"/");
					ShellExecute(parent_hwnd, "open", hp,
						NULL,NULL,SW_SHOWDEFAULT); 
				} return TRUE;
			case IDC_BEELINED_LINK:
				{
					// Bee-lined link
					ShellExecute(parent_hwnd, "open", "http://www.bee-lined.com/",
						NULL,NULL,SW_SHOWDEFAULT); 
				} return TRUE;
			}
		} break;
	case WM_ERASEBKGND:
		{
			// Force white background in the dialog
			RECT rc;            
			HBRUSH hbr =  CreateSolidBrush(RGB (  255,  255,  255));        
			GetClientRect(hwndDlg, &rc);
			FillRect((HDC)wParam, &rc, hbr);            
			DeleteObject(hbr);
		}
		return TRUE; 
	case WM_CTLCOLORSTATIC:
		{
			HWND hwnd_static = (HWND)lParam;
			int ctrl_id = GetDlgCtrlID(hwnd_static); 
			HDC hdc_static = (HDC) wParam;

			switch(ctrl_id)
			{
			default: break;
			case IDC_BEELINED_LINK:
			case IDC_DRAGBASE_LINK:
				{
					// Set blue text color for the url links
					SetTextColor(hdc_static,RGB(0,64,160));
				} break;
			}

			// And white background color for all static controls
			SetBkColor(hdc_static, RGB(255,255,255));
			return (INT_PTR)CreateSolidBrush(RGB(255,255,255));
		} 
	default: return FALSE;
	}

	return FALSE;
}


// Window procedure of our dragbase Control
LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			
			// Save handles
			this_hwnd = hwnd;
			parent_hwnd = GetParent(hwnd);
			// Create drop source
			pDropSource = new DropSource;
			// Load icon
			hbitmap = (HBITMAP)LoadImage(dll_hinst,MAKEINTRESOURCE(IDB_LOGO),IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR);
			GetObject (hbitmap, sizeof (BITMAP), &bitmap);  
			// Get menu handle
			hmenu = LoadMenu(dll_hinst,MAKEINTRESOURCE(IDR_CONTEXTMENU));
			hmenu = GetSubMenu(hmenu,0);
			// Create the menu item
			if(GetMenu(parent_hwnd) == NULL)
				MessageBox(parent_hwnd,"No native Windows menu was found in this application! dragbase might not work properly.","dragbase warning",MB_ICONWARNING);
			if(!AppendMenu(GetMenu(parent_hwnd),MF_OWNERDRAW,9929,(LPCSTR)NULL))
				MessageBox(parent_hwnd,"Menu item creation failed!","dragbase error",MB_ICONERROR);
			
		} return 0;
	case WM_COMMAND:
		{
			// Context menu entries
			switch(wparam)
			{
			case ID_COPY:
				{
					// Save file
					if(save_file_proc(save_filename,NULL,param))
					{
						// If successful -> Copy the data object to the clipboard
						delete pDataObject;
						pDataObject = CreateDataObject();
						OleSetClipboard(pDataObject);
						OleFlushClipboard();
					}
				} break;
			case ID_ABOUT:
				{
					// Show About dialog
					DialogBox(dll_hinst,MAKEINTRESOURCE(IDD_ABOUT),parent_hwnd,AboutProc);
				} break;
			// Link entries
			case ID_HOMEPAGE:
				{
					char hp[1024];
					strcpy(hp,"http://www.dragbase.com/");
					strcat(hp,app_name);
					strcat(hp,"/");
					ShellExecute(parent_hwnd, "open", hp,
						NULL,NULL,SW_SHOWDEFAULT); 
				} break;
			case ID_GETDRAGBASEFORYOUROWNSOFTWARE:
				{
					char hp[1024];
					strcpy(hp,"http://www.dragbase.com/");
					strcat(hp,app_name);
					strcat(hp,"/developer/");
					ShellExecute(parent_hwnd, "open", hp,
						NULL,NULL,SW_SHOWDEFAULT); 
				} break;
			case ID_WHATISDRAGBASE:
				{
					char hp[1024];
					strcpy(hp,"http://www.dragbase.com/");
					strcat(hp,app_name);
					strcat(hp,"/tutorial/");
					ShellExecute(parent_hwnd, "open", hp,
						NULL,NULL,SW_SHOWDEFAULT); 
				} break;
			}
		} return 0;
	case WM_DESTROY:
		{
			// Destroy the hooks
			UnhookWindowsHookEx(hook);
			UnhookWindowsHookEx(mouse_hook);
			// Shutdown OLE
			OleUninitialize();
		} return 0;
	default: return DefWindowProc(hwnd,msg,wparam,lparam);
	}
}