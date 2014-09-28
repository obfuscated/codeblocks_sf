// issue reported here:
// Re: Code completion does not solve STDMETHOD macro for COM interfaces
// http://forums.codeblocks.org/index.php/topic,19661.msg134282.html#msg134282

#define _COM_interface struct
#define interface _COM_interface
#define DECLARE_INTERFACE(i) _COM_interface i
#define DECLARE_INTERFACE_(i,b) _COM_interface i : public b

#define PURE =0
#define STDMETHODCALLTYPE __stdcall
#define STDMETHOD(m) virtual HRESULT STDMETHODCALLTYPE m
#define STDMETHOD_(t,m) virtual t STDMETHODCALLTYPE m

#define REFIID const IID&
#define THIS_
#define THIS void

typedef struct GUID IID;
typedef unsigned long ULONG;
typedef long HRESULT;
typedef bool BOOL;
typedef void *PVOID;

DECLARE_INTERFACE(IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
};

typedef IUnknown *LPUNKNOWN;

DECLARE_INTERFACE_(IClassFactory,IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(CreateInstance)(THIS_ LPUNKNOWN,REFIID,PVOID*) PURE;
	STDMETHOD(LockServer)(THIS_ BOOL) PURE;
};


IUnknown* unknwown;
IClassFactory* factory;

//unknwown-> //QueryInterface,AddRef,Release
//factory->  //QueryInterface,AddRef,Release,CreateInstance,LockServer