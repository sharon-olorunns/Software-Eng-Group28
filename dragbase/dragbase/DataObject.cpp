#include "DataObject.h"
#include "Shlobj.h"
#include "GlobalVars.h"

DataObject::DataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count) :
	m_lRefCount(1), m_pFormatEtc(count), m_pStgMedium(count), 
	dropped(false)
{
	 // reference count must ALWAYS start at 1
	for(int i = 0; i < count; i++)
	{
		m_pFormatEtc[i] = fmtetc[i];
		m_pStgMedium[i] = stgmed[i];
	}
}

DataObject::~DataObject()
{
}


ULONG __stdcall DataObject::AddRef(void)
{
    // Increment object reference count
    return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall DataObject::Release(void)
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

HRESULT __stdcall DataObject::QueryInterface(REFIID iid, void **ppvObject)
{
    // Valid interface?
    if(iid == IID_IDataObject || iid == IID_IUnknown)
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

HRESULT __stdcall DataObject::QueryGetData(FORMATETC *pFormatEtc)
{
    return (LookupFormatEtc(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
}


int DataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
{
    // Find a format which matches the required format
    for(unsigned i = 0; i < m_pFormatEtc.size(); i++)
    {
        if((m_pFormatEtc[i].tymed    &  pFormatEtc->tymed)   &&
            m_pFormatEtc[i].cfFormat == pFormatEtc->cfFormat &&
            m_pFormatEtc[i].dwAspect == pFormatEtc->dwAspect)

        {
            // Return its index
            return i;
        }

    }

	return -1;
}

// Duplicate a global memory
HGLOBAL DupGlobalMem(HGLOBAL hMem)
{
    SIZE_T   len    = GlobalSize(hMem);
    PVOID   source = GlobalLock(hMem);
    PVOID   dest   = GlobalAlloc(GMEM_FIXED, len);

	memcpy(dest, source, len);

	GlobalUnlock(hMem);
	return dest;
}


HRESULT __stdcall DataObject::GetData (FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium)
{
	int index = LookupFormatEtc(pFormatEtc);
	if(index == -1)
		return DV_E_FORMATETC;

	pStgMedium->tymed           = m_pFormatEtc[index].tymed;
    pStgMedium->pUnkForRelease  = 0;

	switch(m_pFormatEtc[index].tymed)
	{
	case TYMED_HGLOBAL:
		{
			// Mouse key pressed and enough time since last drop?
			if(!(GetAsyncKeyState(VK_LBUTTON) & (1<<15))  && !dropped)
			{
				dropped = true;

				// Save old file name (could be changed by the application in the
				// save_file_proc function
				char old_filename[512];
				strcpy(old_filename,save_filename);

				// Pass temporary path to the save function if the application
				// wants to change the file name just before saving
				char temp_path[1024];
				GetTempPath(1024,temp_path);

				// Save file
				save_file_proc(save_filename,temp_path,param);

				// Has the file name changed?
				if(strcmp(save_filename,old_filename) != 0)
				{
					// Relloc memory and copy the file path
					GlobalFree(m_pStgMedium[0].hGlobal);
					m_pStgMedium[0].hGlobal = GlobalAlloc(GMEM_FIXED,sizeof(DROPFILES) + strlen(save_filename) + 2); 
					memcpy((char*)m_pStgMedium[0].hGlobal + sizeof(DROPFILES), save_filename, strlen(save_filename) + 1);
					// Add an additional 0 at the end (end of the list when multiple files are used)
					((char*)m_pStgMedium[0].hGlobal)[sizeof(DROPFILES) + strlen(save_filename) + 1] = 0;

					DROPFILES * dropfiles = (DROPFILES*)m_pStgMedium[0].hGlobal;
					dropfiles->pFiles = sizeof(DROPFILES);
					dropfiles->fWide = 0;
				}
			}

			// Make a copy of our data before returning it
			pStgMedium->hGlobal = DupGlobalMem(m_pStgMedium[index].hGlobal);
		} break;
	default: return DV_E_FORMATETC;
	}

	return S_OK;
}


HRESULT __stdcall DataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
{

    // Only the get direction is supported for OLE
    if(dwDirection == DATADIR_GET)
    {
		FORMATETC * formats = new FORMATETC[m_pFormatEtc.size()];
		for(unsigned i = 0;i< m_pFormatEtc.size(); ++i)
			formats[i] = m_pFormatEtc[i];
        HRESULT hr =  SHCreateStdEnumFmtEtc((UINT)m_pFormatEtc.size(), formats, ppEnumFormatEtc);
		delete [] formats;
		return hr;
    } 
    else
    {
        // The direction specified is not supported for drag+drop
        return E_NOTIMPL;
    }
}


// We dont need this for our purpose
HRESULT DataObject::DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, 
                                                                 DWORD *pdwConnection)
{
    return OLE_E_ADVISENOTSUPPORTED;
}
HRESULT DataObject::DUnadvise (DWORD dwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}
HRESULT DataObject::EnumDAdvise (IEnumSTATDATA **ppEnumAdvise)
{
	return OLE_E_ADVISENOTSUPPORTED;
}
HRESULT DataObject::GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
    return DATA_E_FORMATETC;
}
HRESULT DataObject::GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
{
    pFormatEtcOut->ptd = NULL;
    return E_NOTIMPL;
}


HRESULT DataObject::SetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease)
{
	 if (pFormatEtc == NULL || pMedium == NULL)
        return E_INVALIDARG;

	 // Get index of the format
	 int index = LookupFormatEtc(pFormatEtc);

	 // Format doesn't exit?
	 if(index == -1)
	 {
		 // Then we insert it
		 m_pFormatEtc.push_back(*pFormatEtc);
		 m_pStgMedium.push_back(STGMEDIUM());
		 index = (int)m_pFormatEtc.size() - 1;
	 }

	 // Copy the data
	if (fRelease)
		m_pStgMedium[index] = *pMedium;
	else
	{
		STGMEDIUM med;
		CopyStgMedium(pMedium, &med);
		m_pStgMedium[index] = med;
	}

	return S_OK;
}