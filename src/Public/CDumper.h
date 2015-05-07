#if defined WIN32 || defined WIN64

#ifndef CDUMPER_H
#define CDUMPER_H

#include <imagehlp.h>

namespace tlib {
    // 自动产生Dump文件的类，如果不改Dump名，模块开始的地方调用一次 CDumper::GetInstance(); 就可以了
    class CDumper
    {
    public:
        static CDumper&    GetInstance( void );

        ~CDumper();

        // 默认为 执行档名字.dmp
        void SetDumpFileName( const TCHAR* dumpName );

    protected:
        static void CreateMiniDump( EXCEPTION_POINTERS* pep );

        static LONG __stdcall snUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);

        static    BOOL CALLBACK MiniDumpCallback( 
            PVOID pParam,
            const PMINIDUMP_CALLBACK_INPUT pInput, 
            PMINIDUMP_CALLBACK_OUTPUT       pOutput );

    private:
        CDumper(void);

        CDumper( const CDumper& rhs ) {}
    private:
        LPTOP_LEVEL_EXCEPTION_FILTER    m_previousFilter;
    };
}

#endif    // CDUMPER_H
#endif    //win32 | win64
