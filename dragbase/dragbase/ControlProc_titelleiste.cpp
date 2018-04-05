#include "ControlProc.h"
#include "GlobalVars.h"
#include "DropSource.h"
#include "DataObject.h"
#include "resource.h"

#include <Shlobj.h>
#include <Windowsx.h>
#include <cstring>

using namespace std;

FORMATETC fmtetc;
STGMEDIUM stgmed;
IDropSource *pDropSource =  NULL;
IDataObject *pDataObject = NULL;
IDragSourceHelper * dsh = NULL;
// Rechteck, welches das Icon beschreibt
RECT icon_rect;
HWND this_hwnd, parent_hwnd;
HMENU hmenu;
SHDRAGIMAGE shdragimage;


// Erstellt das DataObject
IDataObject * CreateDataObject()
{

	FORMATETC fmtetc_tmp = 
	{ 
		CF_HDROP,  
		0, 
		DVASPECT_CONTENT, 
		-1, 
		TYMED_HGLOBAL       // stored as a HGLOBAL
	};

	fmtetc = fmtetc_tmp;


	STGMEDIUM stgmed_tmp =
	{
	TYMED_HGLOBAL, // the data goes here
	{ 0 },
	0
	};
	stgmed = stgmed_tmp;
		
	//strcpy(buffer,"C:\\sadf\\lalaasdf.txt");
	stgmed.hGlobal = GlobalAlloc(GMEM_FIXED,sizeof(DROPFILES) + strlen(save_filename) + 2); 
	memcpy((char*)stgmed.hGlobal + sizeof(DROPFILES), save_filename, strlen(save_filename) + 1);
	((char*)stgmed.hGlobal)[sizeof(DROPFILES) + strlen(save_filename) + 1] = 0;
	DROPFILES * dropfiles = (DROPFILES*)stgmed.hGlobal;
	dropfiles->pFiles = sizeof(DROPFILES);
	dropfiles->fWide = 0;

	return new DataObject(&fmtetc,&stgmed,1);
}


LRESULT CALLBACK MouseProc(
  __in  int ncode,
  __in  WPARAM wparam,
  __in  LPARAM lparam
)
{
	static bool left_down = false;

	if(ncode < 0)
		return CallNextHookEx(NULL,ncode,wparam,lparam);

	switch(wparam)
	{
	case WM_NCRBUTTONDOWN:
		{
			GetMenuItemRect(parent_hwnd,GetMenu(parent_hwnd),GetMenuItemCount(GetMenu(parent_hwnd))-2,&icon_rect);
			MOUSEHOOKSTRUCT * mhs = (MOUSEHOOKSTRUCT*)lparam;
		
			if(PtInRect(&icon_rect,mhs->pt) == TRUE)
			{
				TrackPopupMenu(hmenu,TPM_LEFTBUTTON,mhs->pt.x,mhs->pt.y,0,this_hwnd,NULL);

			} return 1;
		} break;
		
	case WM_NCLBUTTONDOWN:
		{
			GetMenuItemRect(parent_hwnd,GetMenu(parent_hwnd),GetMenuItemCount(GetMenu(parent_hwnd))-2,&icon_rect);
			MOUSEHOOKSTRUCT * mhs = (MOUSEHOOKSTRUCT*)lparam;
		
			if(PtInRect(&icon_rect,mhs->pt) == TRUE)
			{
				left_down = true;
				// Datei anlegen
				delete pDataObject;
				pDataObject = CreateDataObject();
				return 1;
			}
		} break;
	case WM_NCLBUTTONUP:
	case WM_DROPFILES:
		{
			left_down = false;
		} break;
	case WM_NCMOUSEMOVE:
		{
			GetMenuItemRect(parent_hwnd,GetMenu(parent_hwnd),GetMenuItemCount(GetMenu(parent_hwnd))-2,&icon_rect);
			MOUSEHOOKSTRUCT * mhs = (MOUSEHOOKSTRUCT*)lparam;
		
			if(PtInRect(&icon_rect,mhs->pt) == TRUE && pDataObject && left_down)
			{
				DWORD dwResult;
				HRESULT hr = dsh->InitializeFromBitmap(&shdragimage,pDataObject);
				DoDragDrop(pDataObject,pDropSource,DROPEFFECT_COPY,&dwResult);
				left_down = false;
				return 1;
			}
			else
				left_down = false;

		} break;

	}
	return CallNextHookEx(NULL,ncode,wparam,lparam);
}

HBITMAP CloneBitmap(HBITMAP hBmp, SIZE Sizes)
{
	return NULL;
	HDC hdc=CreateCompatibleDC(NULL);
	HDC hdcSrc=CreateCompatibleDC(hdc);
	HBITMAP hbm=NULL;
	if(hdc!=NULL && hdcSrc!=NULL)
	{
		if((hbm=CreateBitmap(Sizes.cx, Sizes.cy, 1, 32, NULL))!=NULL)
		// if((hbm=CreateCompatibleBitmap(hdc, Sizes.cx, Sizes.cy))!=NULL)
		{
			HBITMAP hbmPrev=(HBITMAP)SelectObject(hdc, hbm),
			hBmpPrev=(HBITMAP)SelectObject(hdcSrc, hBmp);
			BitBlt(hdc,0,0,Sizes.cx,Sizes.cy,hdcSrc,0,0,SRCCOPY);
			SelectObject(hdc, hbmPrev);
			SelectObject(hdcSrc, hBmpPrev);
		}
		DeleteDC(hdc);
		DeleteDC(hdcSrc);
	}
	return hbm;
}


INT_PTR CALLBACK AboutProc(
  __in  HWND hwndDlg,
  __in  UINT uMsg,
  __in  WPARAM wParam,
  __in  LPARAM lParam
)
{
	switch(uMsg)
	{

	case WM_INITDIALOG:
		{
		} return TRUE;
	case WM_COMMAND:
		{
			if(LOWORD(wParam) == IDOK)
			{
				EndDialog(hwndDlg,TRUE);
				return TRUE;
			}
		} break;
	default: return FALSE;
	}

	return FALSE;
}



LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HBITMAP hbitmap; 
	static BITMAP bitmap ; 
	static bool clicked = true;

	switch(msg)
	{
	case WM_CREATE:
		{
			this_hwnd = hwnd;
			parent_hwnd = GetParent(hwnd);
			pDropSource = new DropSource;
			hbitmap = (HBITMAP)LoadImage(dll_hinst,MAKEINTRESOURCE(IDB_LOGO),IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR);
			GetObject (hbitmap, sizeof (BITMAP), &bitmap) ;       
			hmenu = LoadMenu(dll_hinst,MAKEINTRESOURCE(IDR_CONTEXTMENU));
			hmenu = GetSubMenu(hmenu,0);
			HRESULT hr = CoCreateInstance(CLSID_DragDropHelper,NULL,CLSCTX_INPROC_SERVER,IID_IDragSourceHelper,(void**)&dsh);
			shdragimage.sizeDragImage.cx = 16;
			shdragimage.sizeDragImage.cy = 16;
			shdragimage.ptOffset.x = 8;
			shdragimage.ptOffset.y = 8;
			shdragimage.hbmpDragImage = CloneBitmap(hbitmap,shdragimage.sizeDragImage);
			shdragimage.crColorKey = RGB(255,255,255);

			AppendMenu(GetMenu(parent_hwnd),MF_BITMAP,9999,(LPCSTR)hbitmap);

		} return 0;
	case WM_NCLBUTTONDOWN:
		{
			clicked = !clicked;
			InvalidateRect(hwnd,NULL,TRUE);
		} return 0;
	case WM_COMMAND:
		{
			switch(wparam)
			{
			case ID_COPY:
				{
					delete pDataObject;
					pDataObject = CreateDataObject();
					OleSetClipboard(pDataObject);
					OleFlushClipboard();
				} break;
			case ID_ABOUT:
				{
					DialogBox(dll_hinst,MAKEINTRESOURCE(IDD_ABOUT),parent_hwnd,AboutProc);
				} break;
			case ID_HOMEPAGE:
				{
					ShellExecute(parent_hwnd, "open", "www.beel.org",
						NULL,NULL,SW_SHOWDEFAULT); 
				} break;
			case ID_GETTHISICONFORYOUROWNSOFTWARE:
				{
					ShellExecute(parent_hwnd, "open", "www.getthisiconforyourownsoftware.org",
						NULL,NULL,SW_SHOWDEFAULT); 
				} break;
			case ID_HOWDOESTHISICONWORK:
				{
					ShellExecute(parent_hwnd, "open", "www.howdoesthisiconwork.org",
						NULL,NULL,SW_SHOWDEFAULT); 
				} break;
			}
		} return 0;
	case WM_DESTROY:
		{
			UnhookWindowsHookEx(hook);
			dsh->Release();
			OleUninitialize();
		} return 0;
	default: return DefWindowProc(hwnd,msg,wparam,lparam);
	}
}