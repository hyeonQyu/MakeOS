#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"

// 함수 선언
void kPrintString( int iX, int iY, const char* pcString );
void kDoubleMappingPrintString( int iX, int iY, const char* pcString );

/**
 *  아래 함수는 C 언어 커널의 시작 부분임
 */
void Main( void )
{
    int iCursorX, iCursorY;

    // 콘솔을 먼저 초기화한 후, 다음 작업을 수행
    kInitializeConsole( 0, 10 );    
    kPrintf( "Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );
    
    // 부팅 상황을 화면에 출력
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf( "GDT Initialize And Switch For IA-32e Mode...[    ]" );
    kInitializeGDTTableAndTSS();
    kLoadGDTR( GDTR_STARTADDRESS );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );
    
    kPrintf( "TSS Segment Load............................[    ]" );
    kLoadTR( GDT_TSSSEGMENT );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );
    
    kPrintf( "IDT Initialize..............................[    ]" );
    kInitializeIDTTables();    
    kLoadIDTR( IDTR_STARTADDRESS );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );
    
    kPrintf( "Total RAM Size Check........................[    ]" );
    kCheckTotalRAMSize();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass], Size = %d MB\n", kGetTotalRAMSize() );
    
    kPrintf( "Keyboard Activate And Queue Initialize......[    ]" );
    // 키보드를 활성화
    if( kInitializeKeyboard() == TRUE )
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Pass\n" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Fail\n" );
        while( 1 ) ;
    }
    
    kPrintf( "PIC Controller And Interrupt Initialize.....[    ]" );
    // PIC 컨트롤러 초기화 및 모든 인터럽트 활성화
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    // 셸을 시작
    kStartConsoleShell();

    // while(1){
    //     // 키 큐에 데이터가 있으면 키를 처리
    //     if(kGetKeyFromKeyQueue(&stData) == TRUE){
    //         // 키가 눌러졌으면 키의 아스키 코드 값을 화면에 출력
    //         if(stData.bFlags & KEY_FLAGS_DOWN){
    //             // 키 데이터의 아스키 코드 값을 저장
    //             vcTemp[0] = stData.bASCIICode;
    //             kPrintString(i++, 18, vcTemp);

    //             // 0이 입력되면 변수를 0으로 나누어 Divide Error 예외(벡터 0번)를 발생시킴
    //                 if(vcTemp[0] == '0'){
    //                     // 아래 코드를 수행하면 Divide Error 예외가 발생하여 커널의 임시 핸들러가 수행됨
    //                     bTemp = bTemp / 0;
    //                 }
    //                 else if(vcTemp[0] == 'w'){
    //                     // 쓰기
    //                     DWORD* address = 0x1FF000;
    //                     *address = 0xFF0FF;
    //                 }
    //                 else if(vcTemp[0] == 'a'){
    //                     // 접근
    //                     DWORD* address = 0x1FF000;
    //                     DWORD test = *address;
    //                 }
    //         }
    //     }
    // }

    //kDoubleMappingPrintString( 0, 13, "This message is printed through the video memory relocated to 0xAB8000" );

    // kPrintString(0, 14, "Read from 0x1fe000 [  ]");
    // Readable(0x1FE000, 14);
    // kPrintString(0, 15, "Write to 0x1fe000 [  ]");
    // Writable(0x1FE000, 15);
    // kPrintString(0, 16, "Read from 0x1ff000 [  ]");
    // Readable(0x1FF000, 16);
    // kPrintString(0, 17, "Write to 0x1ff000 [  ]");
 //   Writable(0x1FF000, 17);
}

// 해당 페이지 읽기 확인
void Readable(DWORD* address, int y){
    DWORD test = 0xFF00FF;
    test = *address;
    
    if(test != 0xFF00FF){
        kPrintString(20, y, "OK");
    }
    else {
        kPrintString(0, y, "                                             ");
    }
}

// 해당 페이지에 쓰기 확인
void Writable(DWORD* address, int y){
    DWORD test = 0xFF00FF;
    *address = test;
    
    if(*address == 0xFF00FF){
        kPrintString(19, y, "OK");
    }
    else {
        kPrintString(0, y, "                                             ");
    }
}

/**
 *  문자열을 X, Y 위치에 출력
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    pstScreen += ( iY * 80 ) + iX;

    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

// 0xAB8000 비디오 메모리를 통해서 문자열 출력
void kDoubleMappingPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    pstScreen += ( iY * 80 ) + iX;

    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}