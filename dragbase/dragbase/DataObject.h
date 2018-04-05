#include <Windows.h>
#include <vector>
using namespace std;

class DataObject : public IDataObject
{
public:

	// IUnknown members
	HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject);
	ULONG __stdcall AddRef (void);
	ULONG __stdcall Release (void);

	// IDataObject members
	HRESULT __stdcall GetData (FORMATETC *pFormatEtc, STGMEDIUM *pmedium);
	HRESULT __stdcall GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pmedium);
	HRESULT __stdcall QueryGetData (FORMATETC *pFormatEtc);
	HRESULT __stdcall GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut);
	HRESULT __stdcall SetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease);
	HRESULT __stdcall EnumFormatEtc (DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc);
	HRESULT __stdcall DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *, DWORD *);
	HRESULT __stdcall DUnadvise (DWORD dwConnection);
	HRESULT __stdcall EnumDAdvise (IEnumSTATDATA **ppEnumAdvise);

	// Constructor / Destructor
	DataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count);
	~DataObject();

private:

	// Number of instances
	LONG m_lRefCount;
	// List of formats and mediums
	vector<FORMATETC> m_pFormatEtc;
	vector<STGMEDIUM> m_pStgMedium;

	// Determines if the drop stuff (saving the file, calling SpecifyFilenameProc)
	// was already done for this data object in order to prevent to do it twice
	bool dropped;

	int LookupFormatEtc(FORMATETC *pFormatEtc);
};