//
// Test & performance of ScsTable
//
// Copyright (c) Microsoft Corporation. Licensed under the MIT license. 
//

#include "precomp.h"

VOID
testScsTableSingle(UINT32 elementSize, UINT32 nElements)
{
    SYMCRYPT_SCSTABLE   scsTable;
    UINT32  cbBuffer;
    PBYTE pbBuffer;
    PBYTE pbValues;
    PBYTE pbTest;

    cbBuffer = SymCryptScsTableInit( &scsTable, nElements, elementSize );
    pbBuffer = (PBYTE) AllocWithChecksSc( cbBuffer );
    SymCryptScsTableSetBuffer( &scsTable, pbBuffer, cbBuffer );

    pbValues = (PBYTE) AllocWithChecksSc( elementSize * nElements );
    pbTest = (PBYTE) AllocWithChecksSc( elementSize );

    CHECK( pbBuffer != NULL && pbValues != NULL && pbTest != NULL, "Out of memory" );

    for( UINT32 i=0; i<nElements; i++ )
    {
        SymCryptScsTableStore( &scsTable, i, pbValues + elementSize * i, elementSize );
    }

    for( UINT32 i = 0; i < 6 * nElements; i++ )
    {
        UINT32 entry = (UINT32) g_rng.sizet( nElements );
        BYTE b = g_rng.byte();
        if (b & 1)
        {
            // read
            SymCryptScsTableLoad( &scsTable, entry, pbTest, elementSize );
            CHECK( memcmp( pbTest, pbValues + elementSize * entry, elementSize ) == 0, "Scs table data mismatch" );
            //iprint( "." );
        } else {
            for( UINT32 j=0; j<elementSize; j++ )
            {
                pbTest[j] = g_rng.byte();
            }
            SymCryptScsTableStore( &scsTable, entry, pbTest, elementSize );
            memcpy( pbValues + elementSize * entry, pbTest, elementSize );
            //iprint( "+" );
        }
    }

    // Test the wipe
    SymCryptScsTableWipe( &scsTable );

    SymCryptWipe( pbTest, elementSize );
    SymCryptWipe( pbValues, elementSize * nElements );

    FreeWithChecksSc( pbTest );
    FreeWithChecksSc( pbValues );
    FreeWithChecksSc( pbBuffer );
}

UINT32 testScsTableElementSizes[] = {
#if !( SYMCRYPT_CPU_AMD64 | SYMCRYPT_CPU_ARM64 )
    16, 48,
#endif
    32, 64, 128, 192, 256, 1024,
};

UINT32 testScsTableNElements[] = {
    8, 16, 24, 32, 64,
};

VOID
testScsTable()
{
    if( !isAlgorithmPresent( "ScsTable", FALSE ) )
    {
        return;
    }

    print( "    ScsTable" );
    for( UINT32 i = 0; i < ARRAY_SIZE(testScsTableElementSizes); i++ )
    {
        for( UINT32 j = 0; j < ARRAY_SIZE(testScsTableNElements); j++ )
        {
            testScsTableSingle( testScsTableElementSizes[i], testScsTableNElements[j] );
        }
    }
    print ( "\n" );
}