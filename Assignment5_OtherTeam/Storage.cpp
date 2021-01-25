//
//  Storage.cpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Storage.hpp"
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>

namespace ECE141 {
    // USE: Our main class for managing storage...
    const char* StorageInfo::getDefaultStoragePath() {
        //STUDENT -- MAKE SURE TO SET AN ENVIRONMENT VAR for DB_PATH! 
        //           This lets us change the storage location during autograder testing

        //WINDOWS USERS:  Use forward slash (/) not backslash (\) to separate paths.
        //                (Windows convert forward slashes for you)

        const char* thePath = std::getenv("DB_PATH");
        //const char* thePath = "./temp/";
        return thePath;
    }

    //----------------------------------------------------------
    bool isWindows() {
        std::string dpath = StorageInfo::getDefaultStoragePath();
        size_t loc = dpath.find("\\");

        if (loc == std::string::npos) {
            return false;
        }
        return true;
    }

    bool isWindows(const char slash) {
        if (slash == '\\') {
            return true;
        }
        return false;
    }

    //path to the folder where you want to store your DB's...
    std::string getDatabasePath(const std::string& aDBName) {
        std::string thePath;

        //build a full path (in default storage location) to a given db file..
        const char* defaultPath = StorageInfo::getDefaultStoragePath();

        if (isWindows()) {
            thePath = std::string(defaultPath) + "\\" + aDBName;
        }
        else {
            thePath = std::string(defaultPath) + "/" + aDBName;
        }

        return thePath;
    }



    // USE: ctor ---------------------------------------
    Storage::Storage(const std::string aName, CreateNewStorage) : name(aName) {
        std::string thePath = getDatabasePath(name);

        setOSslash(); //sets slash depending on OS

        fileHandler theHandler;
        if (theHandler.doesFileExist(name)) {
            status.code = Errors::databaseExists;
        }
        else {
            status = theHandler.createDBFile(name, stream, *this);
        }

        //try to create a new db file in known storage location.
        //throw error if it fails...

    }

    // USE: ctor ---------------------------------------
    Storage::Storage(const std::string aName, OpenExistingStorage) : name(aName) {
        std::string thePath = getDatabasePath(aName);

        setOSslash();   //sets slash depending on OS

        //try to OPEN a db file a given storage location
        fileHandler theHandler;
        if (theHandler.doesFileExist(name)) {
            if (isReady()) {
                //will have to check if file's dirty bit is set or not and update before closing. safer to close and then reopen for now until something like that is implemented
                stream.close();
            }
            stream.open(name + ext, std::ios::binary);
        }
        else {
            status = Errors::unknownDatabase;
        }
        //if it fails, throw an error
    }

    // USE: dtor ---------------------------------------
    Storage::~Storage() {
        stream.close();
    }

    // USE: validate we're open and ready ---------------------------------------
    bool Storage::isReady() const {
        return stream.is_open();
    }

    void Storage::setOSslash() {
        //sets the correct slash depending on OS
        if (isWindows()) {
            OSslash = '\\';
        }
        else {
            OSslash = '/';
        }
        return;
    }


    
    // USE: count blocks in file ---------------------------------------
    uint32_t Storage::getTotalBlockCount() {
        //how can we compute the total number of blocks in storage?
        uint32_t theCount = 0;
        std::streampos pos;

        stream.close();
        std::string fullPath = getDatabasePath(name) + ext;
        stream.open(fullPath, std::fstream::in | std::fstream::binary | std::fstream::out);

        //need to see how big the packet size is
        if (isReady())
        {
            stream.seekp(0, std::ios::end);
            pos = stream.tellg();
            stream.seekp(0, std::ios::beg);
            theCount = (pos / getBlockSize())+1;
        }

        stream.close();
        return theCount;
    }

    //breif: find free block
    StatusResult Storage::findFreeBlockNum() {
        StatusResult freeBlock = StatusResult();

        for (int i = 1; i < getTotalBlockCount(); i++) 
        {
            if (isEmptyBlock(i))
            {
                freeBlock.value = i;
                return freeBlock;
            }
        }
        freeBlock.value = (getTotalBlockCount());
        return freeBlock;
    }
    //breif: append block to end of file
    StatusResult Storage::appendBlock(int blocknumber)
    {
        StatusResult myResult = StatusResult();
        myResult.value = blocknumber;
        stream.close();
        std::string fullPath = getDatabasePath(name) + ext;
        stream.open(fullPath, std::fstream::out | std::fstream::binary | std::fstream::in);
        
        if (isReady())
        {
            std::streampos infile;
            StorageBlock freeBlock(ECE141::BlockType::free_block);
            for (int i = 0; i < ECE141::kPayloadSize; i++)
            {
                freeBlock.data[i] = '0';
            }
            writeBlock(freeBlock, blocknumber);
            stream.seekp(0, std::ios::beg);
        }
        else
            myResult.code = ECE141::Errors::readError;

        stream.close();
        return myResult;
    }
    //breif: check if block has valid header info, or is free
    bool Storage::isEmptyBlock(int blockNumber)
    {
        StorageBlock tempBlock;
        StatusResult readResult;

        readResult = readBlock(tempBlock, blockNumber);

        if (readResult)
            if (tempBlock.header.blockNum != blockNumber || tempBlock.header.type == 'F')
                return true;
        
        return false;
    }

    /*
     * @Breif: saves a storable into a block/blocks
    */
    StatusResult Storage::save(Storable& aStorable, int startBlock)
    {
        uint32_t blockNum = startBlock;
        std::stringstream inputStream;
        std::string bufferString;
        const char* cBuf;

        if (startBlock == -1) //check if set
        {
            StatusResult startVal = findFreeBlockNum();
            blockNum = startVal.value;
        }

        if (aStorable.encode(inputStream))
        {
            bufferString = inputStream.str();
            cBuf = bufferString.c_str();
            int bufferLength = inputStream.tellp();
            int blocksToFill = ceil((bufferLength / ECE141::kPayloadSize) + 0.5);
            int blockOffset = 0;
            StatusResult encodeingStatus;

            StorageBlock aBlock;
            aBlock.header.type = static_cast<char>(aStorable.getType());

            while (encodeingStatus && blockOffset < blocksToFill)
            {
                //why does this sometimes throw an excpetion??
                std::memcpy(reinterpret_cast<void*>(&aBlock.data), &cBuf[blockOffset * kPayloadSize], ECE141::kPayloadSize);
                blockOffset++;
                aBlock.header.count = blocksToFill;     //total amount of data or something
                aBlock.header.blockNum = blockNum;      //which offset it is
                aBlock.header.next = 0;                 //blck number of continuing chunk

                if (blockOffset < blocksToFill)
                {
                    //gete next free block if we're saving next block
                    encodeingStatus = findFreeBlockNum();
                    aBlock.header.next = encodeingStatus.value;
                }
                aStorable.blockNum = blockNum;
                encodeingStatus = writeBlock(aBlock, blockNum);
            }
        }
        StatusResult endResult;
        endResult.value = blockNum;
        return endResult;
    }

    /*
     * @Breif: reads a block and converts to a storable
    */
    StatusResult Storage::BlockToStorable(Storable& aStorable, int startBlock)
    {
        StorageBlock tempBlock;
        StatusResult readResult;
        int blockToRead = startBlock;
        int offset = 0;
        int currBlock = tempBlock.header.blockNum;
        int totalBlocks = tempBlock.header.count;
        int cufsize = (2 + totalBlocks) * ECE141::kPayloadSize;
        
        char* cBuf = new char[cufsize];
        std::string dataString;
                
        //check if I am still going keep reading block
        while (readResult && currBlock < totalBlocks)
        {
            readResult = readBlock(tempBlock, blockToRead);
            std::memcpy(reinterpret_cast<void*>(&cBuf[offset]), &tempBlock.data, sizeof(tempBlock.data));
            offset = offset + ECE141::kPayloadSize;
            currBlock++;
            blockToRead = tempBlock.header.next;
        }

        if (readResult)
        {
            std::stringstream outputStream(cBuf);
            readResult = aStorable.decode(outputStream);
        }

        delete cBuf;
        return readResult;
    }

    //@breif: open or close stream
    void Storage::changeStream(streamStatus stat)
    {
        stream.close();
        if (stat == open) {
            std::string fullPath = getDatabasePath(name) + ext;
            stream.open(fullPath, std::fstream::out | std::fstream::binary | std::fstream::in);
        }
    }
    StatusResult Storage::markBlockFree(int blocknumber)
    {
        StorageBlock empty{ BlockType::free_block };
        return writeBlock(empty, blocknumber);
    }

    // USE: write data a given block (after seek)
    StatusResult Storage::writeBlock(StorageBlock& aBlock, uint32_t aBlockNumber) {
        changeStream(open);

        if (isReady())
        {
            stream.seekp(0, std::fstream::beg);
            stream.seekp((aBlockNumber * getBlockSize()), std::ios::beg); //seek 1012*blocknumber bytes from beginning
            stream.write((char*)&aBlock.header.type, sizeof(char));
            stream.write((char*)&aBlock.header.id, sizeof(char)); //write 1012 bytes at location
            stream.write((char*)&aBlock.header.next, sizeof(char));
            stream.write((char*)&aBlock.header.blockNum, sizeof(char));
            stream.write((char*)&aBlock.header.count, sizeof(char));
            stream.write(aBlock.data, ECE141::kPayloadSize);
            stream.seekp(0, std::fstream::beg); //reset stream pointer to beginning
        }
        changeStream(close);
        return StatusResult{};
    }

    // USE: read data from a given block (after seek)
    StatusResult Storage::readBlock(StorageBlock& aBlock, uint32_t aBlockNumber) {
        
        changeStream(open);

        if (isReady())
        {
            stream.seekg((aBlockNumber*getBlockSize()), stream.beg); 
            stream.read((char*)&aBlock.header.type, sizeof(char));
            stream.read((char*)&aBlock.header.id, sizeof(char));
            stream.read((char*)&aBlock.header.next, sizeof(char));
            stream.read((char*)&aBlock.header.blockNum, sizeof(char));
            stream.read((char*)&aBlock.header.count, sizeof(char));
            stream.read(aBlock.data, ECE141::kPayloadSize);                   
            stream.seekg(0, stream.beg); 
        }
        changeStream(close);
        return StatusResult{};
    }

    //USE: iterate through each block and get metaData
    StatusResult Storage::each(BlockListener* aListener)
    {
        StorageBlock tmpBlock;
        StatusResult result;
         for (int i = 0; i < getTotalBlockCount(); i++)
         {
             result = readBlock(tmpBlock, i);
             if (result)
                 (*aListener)(tmpBlock);
         }

         return result;
    }

    
    

}

