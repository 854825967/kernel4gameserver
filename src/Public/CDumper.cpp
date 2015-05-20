#if defined WIN32 || defined WIN64
#include "MultiSys.h"
#include "CDumper.h"
#include "Tools.h"
#include <string>
using namespace std;
#pragma comment(lib, "dbghelp.lib")

TCHAR    m_acDumpName[MAX_PATH*2] = { _T("\0") };

namespace tlib {
#define snArraySize( x )    sizeof(x) / sizeof(x[0])
    TCHAR*        snFindLastOfChar( const TCHAR* pSrc, const TCHAR* keyChar ) {
        assert( NULL != pSrc && NULL != keyChar );
        size_t    nKeyLen = _tcslen(keyChar);
        const    TCHAR*    pFindPos = NULL;
        const    TCHAR*    pLabor     = pSrc;
        while ( ( pLabor = _tcsstr(pLabor, keyChar) ) != NULL ) {
            pLabor += nKeyLen;
            pFindPos = pLabor;
        }

        return    const_cast<TCHAR*>( pFindPos );
    }

    static string WideStrToString( const wchar_t* pwText, UINT code  ) {
        //assert(pwText);
        int iNeedSize = ::WideCharToMultiByte( code, 0, pwText, -1, NULL, 0, NULL, NULL );

        if ( iNeedSize >= 4096 ) {
            char* pchBuffer = new char[iNeedSize+1];
            memset( pchBuffer, 0, (iNeedSize+1)*sizeof(char));
            WideCharToMultiByte( code, 0, pwText, -1, pchBuffer, iNeedSize+1, NULL, NULL );

            string strResult(pchBuffer);
            delete []pchBuffer;
            return strResult;
        } else {
            //assert(pwText);
            char pchBuffer[4096] = {'\0'};

            ::WideCharToMultiByte( code, 0, pwText, -1, pchBuffer, sizeof(pchBuffer), NULL, NULL);
            return string(pchBuffer);
        }
    }

    bool IsDataSectionNeeded( const wchar_t* pModuleName ) {
        if( NULL == pModuleName ) {
            return false;
        }

        string strTemp = WideStrToString( pModuleName, CP_ACP);

        char szFileName[MAX_PATH] = { "\0" };

        _tsplitpath_s( strTemp.c_str(), NULL, 0, NULL, 0, szFileName, snArraySize(szFileName), NULL, 0 );

        return ( 0 == _tcsicmp( szFileName, "ntdll" )  ? true : false );
    }

    CDumper&        CDumper::GetInstance( void ) {
        static    CDumper    app;
        return app;
    }

    CDumper::~CDumper(void) {
        if ( NULL != m_previousFilter ) {
            SetUnhandledExceptionFilter( m_previousFilter );
            m_previousFilter = NULL;
        }
    }

    LONG __stdcall CDumper::snUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
        CDumper::CreateMiniDump( pExceptionInfo );

        exit( pExceptionInfo->ExceptionRecord->ExceptionCode  );

        return EXCEPTION_EXECUTE_HANDLER;
    }

    BOOL CALLBACK CDumper::MiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput) {
        if( pInput == 0 || pOutput == 0) return FALSE;
        switch( pInput->CallbackType ) {
        case ModuleCallback:
            if( pOutput->ModuleWriteFlags & ModuleWriteDataSeg )
                if( !IsDataSectionNeeded( pInput->Module.FullPath ) )
                    pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
            // fall through
        case IncludeModuleCallback:
        case IncludeThreadCallback:
        case ThreadCallback:
        case ThreadExCallback:
            return TRUE;
        default:;
        }
        return FALSE;
    }

    void CDumper::CreateMiniDump( EXCEPTION_POINTERS* pep ) {
        HANDLE hFile = CreateFile( m_acDumpName,
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

        if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) {
            MINIDUMP_EXCEPTION_INFORMATION mdei;
            mdei.ThreadId           = GetCurrentThreadId();
            mdei.ExceptionPointers  = pep;
            mdei.ClientPointers     = FALSE;
            MINIDUMP_CALLBACK_INFORMATION mci;
            mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)(CDumper::MiniDumpCallback);
            mci.CallbackParam       = 0;

            MINIDUMP_TYPE mdt = MiniDumpNormal;

            BOOL bOK = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(),
                hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci );

            CloseHandle( hFile );
        }
    }

    CDumper::CDumper(void) {
        ::GetModuleFileName( 0, m_acDumpName, snArraySize(m_acDumpName) );

        bool    bModified = false;
        TCHAR* pszDot = snFindLastOfChar( m_acDumpName, _T(".") );
        if ( pszDot ) {
            if ( _tcslen(pszDot) >= 3 ) {
                bModified = true;
                _tcscpy_s( pszDot, sizeof(_T("dmp"))/sizeof(TCHAR) +1, _T("dmp") );
            }
        }

        if ( !bModified ) {
            _tcscat_s( m_acDumpName, _T(".dmp") );
        }

        m_previousFilter = ::SetUnhandledExceptionFilter( CDumper::snUnhandledExceptionFilter );
    }

    void CDumper::SetDumpFileName( const TCHAR* dumpName ) {
        if ( NULL != dumpName ) {
            _tcscpy_s( m_acDumpName, snArraySize(m_acDumpName), dumpName );
        }
    }
}
#endif //#if defined WIN32 || defined WIN64
