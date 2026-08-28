// ================================================================================================
// Copyright © 2026 Marcos Eugenio Gorostegui
// Non-Commercial Use and Evaluation License - All rights reserved.
//
// The header code and associated binary files are the exclusive property of the author.
// Their use is authorized solely for non-commercial evaluation and testing purposes.
// Unauthorized reproduction, distribution, modification, or commercial use is strictly prohibited.
//
// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// ================================================================================================

enum ALLOCATOR_TYPE
{
    STANDARD_ALLOCATOR,  //  new - delete
    WINDOWS_ALLOCATOR    //  VirtualAlloc - VirtualFree
};

enum CONSOLE_TYPE
{
    NULL_OUTPUT,        //  NOT CONSOLE
    WINDOWS_OUTPUT      //  WriteConsole - WriteFile
};

struct TRANSFORM_COMPONENT
{
    public: TRANSFORM_COMPONENT(void){return;};
    public:~TRANSFORM_COMPONENT(void){return;};

    private:

        float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
        float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
        float escX = 0.0f, escY = 0.0f, escZ = 0.0f;

    public:

        void SetPos(float posX, float posY,float posZ)
        {
            this->posX = posX; this->posY = posY; this->posZ = posZ; return;
        };

        void SetRot(float rotX, float rotY, float rotZ)
        {
            this->rotX = rotX; this->rotY = rotY; this->rotZ = rotZ; return;
        };

        void SetScl(float escX, float escY, float escZ)
        {
            this->escX = escX; this->escY = escY; this->escZ = escZ; return;
        };

        float GetRotY(void){return this->rotY;};
};

#include "../include/GRSInterface.h"
#pragma comment(lib, "../lib/GRSWindows64.lib")
#include "../include/GRSDependencies.h"
#pragma comment(lib, "../lib/GRSDependencies.lib")

int main(void)
{
    /*  ATTENTION: Even if you want to create multiple instances,
    you will only have one manager throughout the entire lifecycle of your program.*/
    GRS_INTERFACE GRSInterface;
    GRSInterface.CrateInstance( );

    GRSInterface.DependencyInjection
    (
        GRS_DEPENDENCIES::AllocateMemory(WINDOWS_ALLOCATOR),
        GRS_DEPENDENCIES::SetOutputType(WINDOWS_OUTPUT)
    );

    /*  4 POSSIBLE COMBINATIONS
    GRSInterface.EnableLogs( true, false, "LOG.txt"); = CONSOLA
    GRSInterface.EnableLogs(false,  true, "LOG.txt"); = ARCHIVO
    GRSInterface.EnableLogs( true,  true, "LOG.txt"); = CONSOLA & ARCHIVO
    GRSInterface.EnableLogs(false, false, "LOG.txt"); SIMILAR TO GRSInterface.DisableLogs( );
    BUT IT'S BEST TO DISABLE IT COMPLETELY TO AVOID UNNECESSARY CALCULATIONS.*/
    GRSInterface.EnableLogs(true, true, "Log_StressTest_With_Limit.txt");

    //  DEFINE YOUR BASIC POOL
    GRSInterface.InitialConfiguration(10, 1.5, "kB", "StressTest");

    //  DID YOU FORGET THE NAME YOU JUST ENTERED?
    GRSInterface.FormattedOutput("\nWHAT WAS HIS NAME? %s\n", GRSInterface.GetInstanceName( ));

    //  WE DEFINE SOME VARIABLES TO USE
    unsigned int simpleData[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    double doubleData[8] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};
    unsigned int bigData[256] = { };
    for(unsigned int i = 0; i < 256; i++)
    {
        bigData[i] = i;
    }

    /* CAUTION: The identifiers entered with each block of data must be unique,
    and it is the programmer's responsibility to guarantee this,
    either by implementing a hash function or simply, as in this example,
    a counter and then returning it to a variable to perform searches effectively.*/
    unsigned int uniqueIdentifier = 0;

    GRSInterface.FormattedOutput
    (
        "\n--------------------------"
        "\nENTER NEW DATA INTO MEMORY"
        "\n--------------------------\n\n"
    );

    for(unsigned int i = 0; i < 10; i++)
    {
        GRSInterface.Package(uniqueIdentifier++, {0, 1, 2, 3, 4, 5, 6, 7});
        GRSInterface.Package(uniqueIdentifier++, bigData);
        GRSInterface.Package(uniqueIdentifier++, doubleData);
    }

    GRSInterface.FormattedOutput("\n------------------------");
    GRSInterface.FormattedOutput("\nBUFFER SHOW INDIVIDUALLY");
    GRSInterface.FormattedOutput("\n------------------------\n");
    //  WE ARBITRARILY CHOSE INDEX 7 FROM THE TABLE TO SHOW...
    GRSInterface.PrintValue(7);
    GRSInterface.FormattedOutput("\n\n");

    //  ANY TYPE AND ANY SIZE...
    TRANSFORM_COMPONENT TransforComponent = { };
    //GRSInterface.Reserve<TRANSFORM_COMPONENT, 1>(333);
    GRSInterface.Package(333, "TransforComponent", {TransforComponent});
    TRANSFORM_COMPONENT*transformDir = reinterpret_cast<TRANSFORM_COMPONENT*>(GRSInterface.GetValue(333));
    /*  ONLY IF THE RESERVE METHOD HAS BEEN USED
    transformDir[0] = TransforComponent;//*/
    reinterpret_cast<TRANSFORM_COMPONENT*>(transformDir)->SetRot(30.0f, 60.0f, 90.0f);
    GRSInterface.FormattedOutput("\n%1f\n", reinterpret_cast<TRANSFORM_COMPONENT*>(transformDir)->GetRotY( ));

    /*  A SIMULATED BUFFER RESERVES SPACE FOR DATA THAT EXCEEDS THE STACK;
    OF COURSE, IN THIS CASE, IT CANNOT GO BEYOND THE LIMIT OF THIS DEMO... 262144 * 4 = 1MB*/
    GRSInterface.Reserve<unsigned int, 262144>(999);

    //  BEWARE OF COLLISIONS...
    if(GRSInterface.SetIdentifier(999, 666) == true)
        {GRSInterface.FormattedOutput("\nTHE IDENTIFIER DID CHANGE\n");}
    else{GRSInterface.FormattedOutput("\nTHE IDENTIFIER DID NOT CHANGE\n");}

    /*  CAUTION: The pointer is solely for the purpose of retrieving,
    entering, or replacing data directly in the space assigned to it;
    do not attempt anything else outside of these limits for which it was created.
    Get pointer to th reserved space respecting its type.*/
    unsigned int*dataAddress = reinterpret_cast<unsigned int*>(GRSInterface.GetValue(666));
    if(dataAddress != nullptr)
    {
        GRSInterface.FormattedOutput("\n-------------");
        GRSInterface.FormattedOutput("\nADDRESS FOUND");
        GRSInterface.FormattedOutput("\n-------------\n");

        //  PARTIAL LOAD EXTERNAL DATA USING THE POINTER
        unsigned int*externalData = &bigData[0];
        for(unsigned int i = 0; i < 256; i++)
        {
            dataAddress[i] = externalData[i];
            GRSInterface.FormattedOutput("[%u] ", dataAddress[i]);
        }   GRSInterface.FormattedOutput("\n");

        //  OR DO IT DIRECTLY... DON´T FORGET WHITH SPACE YOU HAD RESERVED
        unsigned int recordValueCount = GRSInterface.GetRecordValueCount(666);
        for(unsigned int i = 0; i < recordValueCount; i++)
        {
            dataAddress[i] = i;
        }
    }
    else
    {
        GRSInterface.FormattedOutput("\n---------------");
        GRSInterface.FormattedOutput("\nINVALID ADDRESS");
        GRSInterface.FormattedOutput("\n---------------\n");
    }

    /*  REMOVE BLOCK BY ITS IDENTIFIER
    GRSInterface.DeleteBlockByIdentifier(666);*/

    /*  EXAMPLE OF A WORD AS PART OF A POSSIBLE DICTIONARY
    GRSInterface.Package(777, "Second Coming", simple);*/

    /*  ANOTHER EXAMPLE OF A WORD, BUT INTENDED FOR MASSIVE DATA RESERVETION
    char key[ ] = {"Second Coming"};
    GRSInterface.Reserve<sizeof(key), unsigned int, 262144>(777, key);*/

    //  IF YOU NEED A SPECIFIC SIZE...
    char reserveKey[32] = { };
    GRSInterface.Package(777, reserveKey, simpleData);
    GRSInterface.SetKey(777, "Second Coming");

    //  DID YOU ALSO FORGET THE KEY?
    const char*key = GRSInterface.GetKey(777);
    if(key != nullptr)
    {
        GRSInterface.FormattedOutput("\nWHAT IS MY KEY? %s\n", key);
    }
    else
    {
        GRSInterface.FormattedOutput("\nDOES NOT HAVE A KEY O OR INCORRECT IDENTIFIER\n");
    }

    //  OR DO IT DIRECTLY...
    GRSInterface.PrintKey(777);
    GRSInterface.FormattedOutput(" WITH A LENGTH OF %u CHARACTERS.\n", GRSInterface.GetKeyLength(777));
    //  INTERCHANGELY: BUFFER or WORD
    GRSInterface.FormattedOutput("AND DATA ENTERED: ");
    GRSInterface.PrintValue(777); GRSInterface.FormattedOutput("\n");

    //  WE CHANGED THE KEY
    GRSInterface.SetKey(777, "WE CHANGED THE KEY");
    GRSInterface.PrintKey(777); GRSInterface.FormattedOutput("\n");

    //  CAUTION: Do not exceed the established length
    GRSInterface.SetKey(777, "BETTER WITH A LONGER ONE?");
    GRSInterface.PrintKey(777); GRSInterface.FormattedOutput("\n");

    //  WE PRINTED THE COMPLETE MEMORY STATUS
    GRSInterface.PrintMemory( );

    GRSInterface.FormattedOutput
    (
        "\n-----------------------------------------"
        "\nDELETE DATA WITH EVEN NUMBERS FROM MEMORY"
        "\n-----------------------------------------\n\n"
    );

    void*blockAddress = nullptr;

    //  WE ASK THE INDEX TABLE FOR THE DATA RECORDES WITH EVEN NUMBERS INDEXES
    for(unsigned int i = 0; i < GRSInterface.GetTableRecordCount( ); i+=2)
    {
        blockAddress = GRSInterface.GetDataFromTable(i);
        if(blockAddress == 0)
        {
            continue;
        }

        GRSInterface.DeleteBlockByIndex(i);

        GRSInterface.FormattedOutput("\nSTART DEFRAGMENTATION BY EXTERNAL IDLE: \n");
        GRSInterface.Defragment( );
        /*  OR ALSO BY IDLE OF MIGRATION...
        GRSInterface.Migrate( );*/
        GRSInterface.FormattedOutput("END EXTERNAL IDLE.\n");
    }   GRSInterface.PrintMemory( );

    GRSInterface.FormattedOutput
    (
        "\n--------------------------"
        "\nENTER NEW DATA INTO MEMORY"
        "\n--------------------------\n\n"
    );

    for(unsigned int i = 0; i < 10; i++)
    {
        GRSInterface.Package(uniqueIdentifier++, doubleData);
    }   GRSInterface.PrintMemory( );

    GRSInterface.FormattedOutput
    (
        "\n----------------------------------------"
        "\nDELETE DATA WITH ODD NUMBERS FROM MEMORY"
        "\n----------------------------------------\n\n"
    );

    /*  AGAIN, WE REQUEST THE RECORDED DATA FROM THE INDEX TABLE,
    ONLY THIS TIME, USING THE ODD NUMBERS INDEXES*/
    for(unsigned int i = 1; i < GRSInterface.GetTableRecordCount( ); i+=2)
    {
        blockAddress = GRSInterface.GetDataFromTable(i);
        if(blockAddress == 0)
        {
            continue;
        }

        GRSInterface.DeleteBlockByIndex(i);
    }   GRSInterface.PrintMemory( );

    /*  NEVER FORGET TO CLEAN UP AT THE END,
    BUT IF YOU FORGET, THE DESTROYER WILL DO IT FOR YOU.*/
    GRSInterface.DeleteMemory( );

    return 0;
}
