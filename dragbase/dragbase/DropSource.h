#include <Windows.h>

class DropSource : public IDropSource
{
public:

    DropSource();
    ~DropSource();

    HRESULT __stdcall QueryInterface    (REFIID iid, void ** ppvObject);
    ULONG   __stdcall AddRef            (void);
    ULONG   __stdcall Release           (void);	

    HRESULT __stdcall QueryContinueDrag (BOOL fEscapePressed, DWORD grfKeyState);
    HRESULT __stdcall GiveFeedback      (DWORD dwEffect);

private:

	LONG m_lRefCount;
};