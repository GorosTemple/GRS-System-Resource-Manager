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

//  NEEDED FOR INTERNAL FUNCTIONS...
struct PARAMETROS
{
    unsigned char*datos;
};

template<typename T>
unsigned char*GetDirection(T&value)
{
    return (unsigned char*)&value;
}

template<typename T>
unsigned char*GetDirection(T*value)
{
    return (unsigned char*)value;
}

#pragma once

#ifndef _GRS_INTERFACE_
#define _GRS_INTERFACE_
class GRS_INTERFACE
{
    public: GRS_INTERFACE(void);
    public:~GRS_INTERFACE(void);

    private:

        void SetFormattedOutput
        (
            const char*texto,
            PARAMETROS*param
        );

    // LIKE C
    public:

        template<typename... Args>
        void FormattedOutput(const char*text, Args... rest)
        {
            PARAMETROS param[1 + sizeof...(Args)] = {
                {GetDirection(rest)}...
            };

            SetFormattedOutput(text, param);

            return;
        };

    public:

        void CrateInstance(void);

    //  BRIDGE FOR WORD
    private:

        bool BufferReserve
        (
            unsigned int identifier,
            unsigned int determineType,
            unsigned int typeSize,
            unsigned int elements
        );

        bool BufferPackage
        (
            unsigned int identifier,
            unsigned int determineType,
            unsigned int typeSize,
            unsigned int elements,
            void*value
        );

    //  BRIDGE FOR WORD
    private:

        bool WordReserve
        (
            unsigned int identifier,
            const char*key,
            unsigned int length,
            unsigned int determineType,
            unsigned int typeSize,
            unsigned int elements
        );

        bool WordPackage
        (
            unsigned int identifier,
            const char*key,
            unsigned int length,
            unsigned int determineType,
            unsigned int typeSize,
            unsigned int elements,
            void*value
        );

    //  SOME BASICS TYPES...
    private:

        template <typename T>
        unsigned int DetermineType(T*)           {return 0;}
        unsigned int DetermineType(void**)       {return 1;}
        unsigned int DetermineType(bool*)        {return 2;}
        unsigned int DetermineType(char*)        {return 3;}
        unsigned int DetermineType(const char**) {return 4;}
        unsigned int DetermineType(char32_t*)    {return 5;}
        unsigned int DetermineType(int*)         {return 6;}
        unsigned int DetermineType(unsigned int*){return 7;}
        unsigned int DetermineType(float*)       {return 8;}
        unsigned int DetermineType(double*)      {return 9;}

    //  DEPENDENCIES INJECTION A
    //  AND PLATFORM INDEPENDENCE
    public:

        void DependencyInjection
        (
            void*AllocatorType,
            void*OutputType
        );

    //  MAIN METHODS
    public:

        void InitialConfiguration
        (
            unsigned int tableElements,
            const float    memorySpace,
            const char*    memoryUnits,
            const char*   instanceName
        );

    //  BUFFER BLOCK
    public:

        template<typename T, unsigned int M>
        bool Reserve
        (
            unsigned int identifier
        )
        {
            return BufferReserve(identifier, DetermineType((T*)0), sizeof(T), M);
        };

        template<typename T, unsigned int M>
        bool Package
        (
            unsigned int identifier,
            const T(&value)[M]
        )
        {
            return BufferPackage(identifier, DetermineType((T*)0), sizeof(T), M, (T*)value);
        };

    //  WORD BLOCK
    public:

        template<unsigned int N, typename T, unsigned int M>
        bool Reserve
        (
            unsigned int identifier,
            const char(&key)[N]
        )
        {
            return WordReserve(identifier, key, N, DetermineType((T*)0), sizeof(T), M);
        };

        template<unsigned int N, typename T, unsigned int M>
        bool Package
        (
            unsigned int identifier,
            const char(&key)[N],
            const T(&value)[M]
        )
        {
            return WordPackage(identifier, key, N, DetermineType((T*)0), sizeof(T), M, (T*)value);
        };

    //  CLEAN METHODS
    public:

        void DeleteBlockByIndex(unsigned int index);
        void DeleteBlockByIdentifier(unsigned int identifier);
        void DeleteMemory(void);

    //  AUXILIARY METHODS
    public:

        const char*GetInstanceName(void);
        void PrintTable(void);
        unsigned int  GetTableVersion(void);
        unsigned int  GetTableRecordCount(void);
        unsigned char*GetDataFromTable(const unsigned int index);
        unsigned int  GetMemoryLevel(const unsigned int index);

    //  IDLE
    public:

        void Defragment(void);
        void Migrate(void);

    //  LOGS
    public:

        void EnableLogs
        (
            bool useConsole,
            bool useFile,
            const char*fileName
        );

        void DisableLogs(void);

    //  ONLY FOR WORD-TYPE BLOCKS
    public:

        unsigned int GetKeyLength(unsigned int identifier);
        char*GetKey(unsigned int identifier);

        bool SetKey
        (
            unsigned int identifier,
            const char*newKey
        );

        void PrintKey(unsigned int identifier);

    //  FOR BOTH: BUFFER & WORD
    public:

        bool SetIdentifier
        (
            unsigned int currentIdentifier,
            unsigned int newIdentifier
        );

        unsigned int GetRecordValueCount(unsigned int identifier);
        void*GetValue(unsigned int identifier);
        void PrintValue(unsigned int identifier);
        void PrintMemory(void);
};
#endif  //  _GRS_INTERFACE_
