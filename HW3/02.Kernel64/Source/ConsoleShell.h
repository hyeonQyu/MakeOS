#ifndef __CONSOLESHELL_H__
#define __CONSOLESHELL_H__

#include "Types.h"

#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT  300
#define CONSOLESHELL_PROMPTMESSAGE          "MINT64>"

// 문자열 포인터를 파라미터로 받는 함수 포인터 타입 정의
typedef void(*CommandFunction) (const char* pcParameter);

#pragma pack(push, 1)

// 셸의 커맨드를 저장하는 자료구조
typedef struct kShellCommandEntryStruct{
    // 커맨드 문자열
    char* pcCommand;
    // 커맨드 도움말
    char* pcHelp;
    // 커맨드를 수행하는 함수의 포인터
    CommandFunction pfFunction;
} SHELLCOMMANDENTRY;

// 파라미터를 처리하기 위해 정보를 저장하는 자료구조
typedef struct kParameterListStruct{
    // 파라미터 버퍼의 어드레스
    const char* pcBuffer;
    // 파라미터의 길이
    int iLength;
    // 현재 처리할 파라미터가 시작하는 위치
    int iCurrentPosition;
} PARAMETERLIST;

#pragma pack(pop)

void kStartConsoleShell();
void kExecuteCommand(const char* pcCommandBuffer);
void kInitializeParameter(PARAMETERLIST* pstList, const char* pcParameter);
int kGetNextParameter(PARAMETERLIST* pstList, char* pcParameter);

// 커맨드를 처리하는 함수
void kHelp(const char* pcParameterBuffer);
void kCls(const char* pcParameterBuffer);
void kShowTotalRAMSize(const char* pcParameterBuffer);
void kStringToDecimalHexTest(const char* pcParameterBuffer);
void kShutdown(const char* pcParameterBuffer);
void kRaiseFault(const char* pcParameterBuffer);

void kPrint1(const char* pcParameterBuffer);
void kPrint2(const char* pcParameterBuffer);
void kPrint3(const char* pcParameterBuffer);
void kPrint11(const char* pcParameterBuffer);
void kPrint22(const char* pcParameterBuffer);
void kPrint33(const char* pcParameterBuffer);

#endif /*__CONSOLESHELL_H__*/