//
//  KittyMemory.hpp
//
//  Created by MJ (Ruit) on 1/1/19.
//

#ifndef KittyMemory_h
#define KittyMemory_h

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <vector>


#define _SYS_PAGE_SIZE_ (sysconf(_SC_PAGE_SIZE))

#define _PAGE_START_OF_(x)    ((uintptr_t)x & ~(uintptr_t)(_SYS_PAGE_SIZE_ - 1))
#define _PAGE_END_OF_(x, len) (_PAGE_START_OF_((uintptr_t)x + len - 1))
#define _PAGE_LEN_OF_(x, len) (_PAGE_END_OF_(x, len) - _PAGE_START_OF_(x) + _SYS_PAGE_SIZE_)
#define _PAGE_OFFSET_OF_(x)   ((uintptr_t)x - _PAGE_START_OF_(x))

#define _PROT_RWX_ (PROT_READ | PROT_WRITE | PROT_EXEC)
#define _PROT_RX_  (PROT_READ | PROT_EXEC)


#define EMPTY_VEC_OFFSET std::vector<int>()


namespace KittyMemory {

    typedef enum {
        FAILED = 0,
        SUCCESS = 1,
        INV_ADDR = 2,
        INV_LEN = 3,
        INV_BUF = 4,
        INV_PROT = 5
    } Memory_Status;


    struct ProcMap {
        void *startAddr;
        void *endAddr;
        size_t length;
        std::string perms;
        long offset;
        std::string dev;
        int inode;
        std::string pathname;

        bool isValid() { return (startAddr != NULL && endAddr != NULL && !pathname.empty()); }
    };

    /*
   * Changes protection of an address with given length
   */
    bool ProtectAddr(void *addr, size_t length, int protection);

    /*
    * Writes buffer content to an address
   */
    Memory_Status memWrite(void *addr, const void *buffer, size_t len);

    /*
   * Reads an address content into a buffer
   */
    Memory_Status memRead(void *buffer, const void *addr, size_t len);

    /*
     * Reads an address content and returns hex string
     */
    std::string read2HexStr(const void *addr, size_t len);


    /*
     * Wrapper to dereference & read value of a pointer
     * Make sure to use the correct data type!
     */
    template<typename Type>
    Type readPtr(void *ptr, std::vector<int> offsets) {
        Type defaultVal = {};
        if (ptr == NULL)
            return defaultVal;
        void *finalPtr = ptr;
        for (int i = 0; finalPtr != NULL && i < offsets.size(); i++) {
            finalPtr = reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(finalPtr) + offsets[i]);
        }
        if (finalPtr == NULL)
            return defaultVal;
        return *reinterpret_cast<Type *>(finalPtr);
    }


    /*
     * Wrapper to dereference & set value of a pointer
     * Make sure to use the correct data type!
     */
    template<typename Type>
    void writePtr(void *ptr, std::vector<int> offsets, Type val) {
        if (ptr == NULL)
            return;
        void *finalPtr = ptr;
        for (int i = 0; finalPtr != NULL && i < offsets.size(); i++) {
            finalPtr = reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(finalPtr) + offsets[i]);
        }
        if (finalPtr) {
            *reinterpret_cast<Type *>(finalPtr) = val;
            finalPtr = NULL;
        }
    }
    /*
     * Gets info of a mapped library in self process
     */
    ProcMap getLibraryMap(const char *libraryName);

    /*
    * Expects a relative address in a library
    * Returns final absolute address
    */
    uintptr_t getAbsoluteAddress(const char *libraryName, uintptr_t relativeAddr);
};

#endif /* KittyMemory_h */
