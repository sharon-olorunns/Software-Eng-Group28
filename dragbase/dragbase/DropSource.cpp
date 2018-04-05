#include "DropSource.h"
#include "Shlobj.h"

DropSource::DropSource() : m_lRefCount(1)
{
}

DropSource::~DropSource()
{
}

ULONG __stdcall DropSource::AddRef(void)
{
    // Increment object reference count
    return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall DropSource::Release(void)
{
    // Decrement object reference count
	LONG count = InterlockedDecrement(&m_lRefCount);
		
	if(count == 0)
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}

HRESULT __stdcall DropSource::QueryInterface(REFIID iid, void **ppvObject)
{
	if(iid == IID_IDropSource || iid == IID_IUnknown)
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
}


HRESULT __stdcall DropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	// Cancel drop when escape is pressed
    if(fEscapePressed == TRUE)
        return DRAGDROP_S_CANCEL;	

	// Do drop when left mouse button is released
	if((grfKeyState & MK_LBUTTON) == 0)
	return DRAGDROP_S_DROP;


	// Continue with the Drag-and-Drop
	return S_OK;
}


HRESULT __stdcall DropSource::GiveFeedback(DWORD dwEffect)
{
    return DRAGDROP_S_USEDEFAULTCURSORS;
}