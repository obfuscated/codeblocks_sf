/*
  LICENSE:

  This code is provided as-is. No warranties are given. Use at your own risk.
  You may do with this code whatever you want except presenting it as yours or
  removing the following copyright notice:

  Copyright 2005, Yiannis Mandravellos <mandrav _AT_ codeblocks _DOT_ org>
*/

#ifndef SCRIPTINGCALL_H
#define SCRIPTINGCALL_H

#include <as/include/angelscript.h>
#include <settings.h>
#include <manager.h>
#include <scriptingmanager.h>

/*
 A dummy struct. Used as default parameter in the following templated calls, to
 allow for up to 10 parameters in the call.
*/
struct DummyOperand {};

/*
    The following functions set the context's arguments before execution.
    They 're templated to allow calling the correct SetArg* function based on
    the param's type.
*/
template <typename OP> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, OP op)
{
    // default template treats it as 'void*'
    ctx->SetArgObject(arg, reinterpret_cast<void*>(op));
}
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, DummyOperand op){ /* do nothing */ }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, asQWORD op){ ctx->SetArgQWord(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, asDWORD op){ ctx->SetArgDWord(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, bool op){ ctx->SetArgDWord(arg, op ? 1 : 0); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, char op){ ctx->SetArgDWord(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, int op){ ctx->SetArgDWord(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, short int op){ ctx->SetArgDWord(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, unsigned int op){ ctx->SetArgDWord(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, double op){ ctx->SetArgDouble(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, float op){ ctx->SetArgFloat(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, void* op){ ctx->SetArgObject(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, wxString* op){ ctx->SetArgObject(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, const wxString* op){ ctx->SetArgObject(arg, (void*)op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, wxString& op){ ctx->SetArgObject(arg, (void*)&op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, const wxString& op){ ctx->SetArgObject(arg, (void*)&op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, wxArrayString* op){ ctx->SetArgObject(arg, op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, const wxArrayString* op){ ctx->SetArgObject(arg, (void*)op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, wxArrayString& op){ ctx->SetArgObject(arg, (void*)&op); }
template <> inline void ExecuteSetArg(asIScriptContext* ctx, int arg, const wxArrayString& op){ ctx->SetArgObject(arg, (void*)&op); }

/*
    The following functions return the context's return value after execution.
    They 're templated to allow calling the correct GetReturn* function based on
    the return value's type.
*/
template <typename RET> inline RET ExecuteGetRet(asIScriptContext* ctx)
{
    // default template treats it as 'void*'
    return static_cast<RET>(ctx->GetReturnObject());
}
template <> inline asQWORD ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnQWord(); }
template <> inline asDWORD ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnDWord(); }
template <> inline bool ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnDWord() != 0; }
template <> inline char ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnDWord(); }
template <> inline int ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnDWord(); }
template <> inline short int ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnDWord(); }
template <> inline unsigned int ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnDWord(); }
template <> inline double ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnDouble(); }
template <> inline float ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnFloat(); }
template <> inline void* ExecuteGetRet(asIScriptContext* ctx){ return ctx->GetReturnObject(); }

class ScriptingCall
{
    public:
        ScriptingCall(int functionID)
            : m_pCtx(0),
            m_Success(true),
            m_Line(0)
        {
            // create the context
            m_pCtx = Manager::Get()->GetScriptingManager()->GetEngine()->CreateContext();
            if (!m_pCtx)
                return;
            // prepare the script function
            int r = m_pCtx->Prepare(functionID);
            if (r < 0)
            {
                if (r == asEXECUTION_EXCEPTION)
                    RecordContextFailure();
                m_pCtx->Release();
                m_pCtx = 0;
            }
        }
        virtual ~ScriptingCall(){}
        /** Returns the created context (NULL if invalid). */
        inline asIScriptContext* GetContext(){ return m_pCtx; }

        virtual bool Success(){ return m_Success; }
        virtual const wxString& GetFunction(){ return m_Function; }
        virtual const wxString& GetModule(){ return m_Module; }
        virtual const wxString& GetSection(){ return m_Section; }
        virtual const wxString& GetError(){ return m_Error; }
        virtual int GetLineNumber(){ return m_Line; }

        virtual wxString CreateErrorString()
        {
            if (m_Success)
                return wxEmptyString;
            wxString result;
            result << m_Error << _T("\n\n");
            result << _T("Module: ") << m_Module << _T("\n");
            result << _T("Section: ") << m_Section << _T("\n");
            result << _T("Function: ") << m_Function << _T("\n");
            result << _T("Line: ") << wxString::Format(_T("%d"), m_Line);
            return result;
        }
    protected:
        void RecordContextFailure()
        {
            m_Success = false;
            asIScriptEngine* engine = Manager::Get()->GetScriptingManager()->GetEngine();
            int funcID = m_pCtx->GetExceptionFunction();
            m_Function = cbC2U(engine->GetFunctionDeclaration(funcID));
            m_Module = cbC2U(engine->GetModuleNameFromIndex(asMODULEIDX(funcID)));
            m_Section = cbC2U(engine->GetFunctionSection(funcID));
            m_Line = m_pCtx->GetExceptionLineNumber();
            m_Error = cbC2U(m_pCtx->GetExceptionString());
        }
        asIScriptContext* m_pCtx;
        bool m_Success;
        wxString m_Function;
        wxString m_Module;
        wxString m_Section;
        wxString m_Error;
        int m_Line;
};

/** Executes a function taking as many as 10 parameters.
  *
  * This is the version that doesn't return a value (void).
  * Use it like this
  * (for a function taking a double and an integer parameter):
  *
  * VoidExecutor<double, int> exec(functionID);
  * exec.Call(5.331, 1);
  *
  * If you need more control of the process, here are more steps
  * for the same example:
  *
  * VoidExecutor<double, int> exec(functionID);
  * exec.SetArguments(5.331, 1);
  * asIScriptContext* ctx = exec.GetContext();
  * // do whatever with the context
  * ctx->Execute();
  * ctx->Release();
  *
  * Notice that in the long version, you *must* release the context
  * yourself. It is normally released in Call()...
  */
template <typename OP1 = DummyOperand, typename OP2 = DummyOperand,
          typename OP3 = DummyOperand, typename OP4 = DummyOperand,
          typename OP5 = DummyOperand, typename OP6 = DummyOperand,
          typename OP7 = DummyOperand, typename OP8 = DummyOperand,
          typename OP9 = DummyOperand, typename OP10 = DummyOperand>
class VoidExecutor : public ScriptingCall
{
    public:
        /** Constructor.
          *
          * Creates and prepares a context for execution.
          * If you don't use Call(), you should release the context yourself by calling
          * GetContext()->Release()...
          *
          * @param functionID The function's ID to call. You can get this by a call to GetFunctionIDByDecl().
          */
        VoidExecutor(int functionID)
            : ScriptingCall(functionID)
        {
        }

        /** Destructor.
          *
          * The context is release only when you use Call().
          * If you haven't , you should release it yourself...
          */
        ~VoidExecutor(){}

        /** Set the execution arguments.
          * Up to 10 arguments can be set.
          */
        void SetArguments(OP1 op1 = DummyOperand(), OP2 op2 = DummyOperand(),
                          OP3 op3 = DummyOperand(), OP4 op4 = DummyOperand(),
                          OP5 op5 = DummyOperand(), OP6 op6 = DummyOperand(),
                          OP7 op7 = DummyOperand(), OP8 op8 = DummyOperand(),
                          OP9 op9 = DummyOperand(), OP10 op10 = DummyOperand())
        {
            if (!m_pCtx) return;
            ExecuteSetArg<OP1>(m_pCtx, 0, op1); ExecuteSetArg<OP2>(m_pCtx, 1, op2);
            ExecuteSetArg<OP3>(m_pCtx, 2, op3); ExecuteSetArg<OP4>(m_pCtx, 3, op4);
            ExecuteSetArg<OP5>(m_pCtx, 4, op5); ExecuteSetArg<OP6>(m_pCtx, 5, op6);
            ExecuteSetArg<OP7>(m_pCtx, 6, op7); ExecuteSetArg<OP8>(m_pCtx, 7, op8);
            ExecuteSetArg<OP9>(m_pCtx, 8, op9); ExecuteSetArg<OP10>(m_pCtx, 9, op10);
        }

        /** Make the call.
          *
          * This runs all the necessary steps:
          * - Sets the arguments,
          * - Executes, and
          * - Releases the context
          *
          * Up to 10 parameters can be used.
          */
        void Call(OP1 op1 = DummyOperand(), OP2 op2 = DummyOperand(),
                  OP3 op3 = DummyOperand(), OP4 op4 = DummyOperand(),
                  OP5 op5 = DummyOperand(), OP6 op6 = DummyOperand(),
                  OP7 op7 = DummyOperand(), OP8 op8 = DummyOperand(),
                  OP9 op9 = DummyOperand(), OP10 op10 = DummyOperand())
        {
            if (!m_pCtx) return;
            SetArguments(op1, op2, op3, op4, op5, op6, op7, op8, op9, op10);
            // this takes care of coroutines and such.
            // it also releases the context when done.
            if (m_pCtx->Execute() == asEXECUTION_EXCEPTION)
                RecordContextFailure();
            m_pCtx->Release();
            m_pCtx = 0;
        }
};

/** Executes a function taking as many as 10 parameters.
  *
  * This is the version that allows for a return a value.
  * Use it like this
  * (for a function taking a double and an integer parameter,
  * returning a float):
  *
  * Executor<float, double, int> exec(functionID);
  * float f = exec.Call(5.331, 1);
  *
  * If you need more control of the process, here are more steps
  * for the same example:
  *
  * Executor<float, double, int> exec(functionID);
  * exec.SetArguments(5.331, 1);
  * asIScriptContext* ctx = exec.GetContext();
  * // do whatever with the context
  * ctx->Execute();
  * float f = exec.GetReturnValue();
  * ctx->Release();
  *
  * Notice that in the long version, you *must* release the context
  * yourself. It is normally released in Call()...
  */
template <typename RET,
          typename OP1 = DummyOperand, typename OP2 = DummyOperand,
          typename OP3 = DummyOperand, typename OP4 = DummyOperand,
          typename OP5 = DummyOperand, typename OP6 = DummyOperand,
          typename OP7 = DummyOperand, typename OP8 = DummyOperand,
          typename OP9 = DummyOperand, typename OP10 = DummyOperand>
class Executor : public ScriptingCall
{
    public:
        /** Constructor.
          *
          * Creates and prepares a context for execution.
          * If you don't use Call(), you should release the context yourself by calling
          * GetContext()->Release()...
          *
          * @param functionID The function's ID to call. You can get this by a call to GetFunctionIDByDecl().
          */
        Executor(int functionID)
            : ScriptingCall(functionID)
        {
        }

        /** Destructor.
          *
          * The context is release only when you use Call().
          * If you haven't , you should release it yourself...
          */
        ~Executor(){}

        /** Set the execution arguments.
          * Up to 10 arguments can be set.
          */
        void SetArguments(OP1 op1 = DummyOperand(), OP2 op2 = DummyOperand(),
                          OP3 op3 = DummyOperand(), OP4 op4 = DummyOperand(),
                          OP5 op5 = DummyOperand(), OP6 op6 = DummyOperand(),
                          OP7 op7 = DummyOperand(), OP8 op8 = DummyOperand(),
                          OP9 op9 = DummyOperand(), OP10 op10 = DummyOperand())
        {
            if (!m_pCtx) return;
            ExecuteSetArg<OP1>(m_pCtx, 0, op1); ExecuteSetArg<OP2>(m_pCtx, 1, op2);
            ExecuteSetArg<OP3>(m_pCtx, 2, op3); ExecuteSetArg<OP4>(m_pCtx, 3, op4);
            ExecuteSetArg<OP5>(m_pCtx, 4, op5); ExecuteSetArg<OP6>(m_pCtx, 5, op6);
            ExecuteSetArg<OP7>(m_pCtx, 6, op7); ExecuteSetArg<OP8>(m_pCtx, 7, op8);
            ExecuteSetArg<OP9>(m_pCtx, 8, op9); ExecuteSetArg<OP10>(m_pCtx, 9, op10);
        }

        /** Make the call.
          *
          * @return RET
          *
          * This runs all the necessary steps:
          * - Sets the arguments,
          * - Executes, and
          * - Releases the context
          *
          * Up to 10 parameters can be used.
          */
        RET Call(OP1 op1 = DummyOperand(), OP2 op2 = DummyOperand(),
                  OP3 op3 = DummyOperand(), OP4 op4 = DummyOperand(),
                  OP5 op5 = DummyOperand(), OP6 op6 = DummyOperand(),
                  OP7 op7 = DummyOperand(), OP8 op8 = DummyOperand(),
                  OP9 op9 = DummyOperand(), OP10 op10 = DummyOperand())
        {
            if (!m_pCtx) return (RET)0;
            SetArguments(op1, op2, op3, op4, op5, op6, op7, op8, op9, op10);
            if (m_pCtx->Execute() == asEXECUTION_EXCEPTION)
                RecordContextFailure();
            RET r = GetReturnValue();
            m_pCtx->Release();
            m_pCtx = 0;
            return r;
        }

        /** Returns the return value (valid only after execution). */
        inline RET GetReturnValue(){ if (m_pCtx) return ExecuteGetRet<RET>(m_pCtx); else return (RET)0; }
};

#endif // SCRIPTINGCALL_H
