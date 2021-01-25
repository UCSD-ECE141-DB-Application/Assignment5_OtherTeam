//
//  Storage.hpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <fstream>
#include <variant>
#include "Errors.hpp"
#include "StorageBlock.hpp"
#include "FolderReader.hpp"
#include "FileChecker.hpp"
#include <iostream>
#include <fstream>
#include "Value.hpp"

namespace ECE141 {
    std::string getDatabasePath(const std::string& aDBName);
    //first, some utility classes...

    bool isWindows();

    class StorageInfo {
    public:
        static const char* getDefaultStoragePath();
    };

    struct CreateNewStorage {};
    struct OpenExistingStorage {};



    //STUDENT: If you're using the Storable interface, add that to Schema class?
    struct Storable {
    public:
        virtual StatusResult  encode(std::ostream& aWriter) = 0; //save
        virtual StatusResult  decode(std::istream& aReader) = 0; //load
        virtual BlockType     getType() const = 0; //what kind of block is this?
        int blockNum;
    };

    struct MetaDataStorable : Storable {
    public:
        MetaDataStorable() {}

        StatusResult encode(std::ostream& aWriter) //save
        {
            //write the schema info
            std::map<std::string, int>::iterator it;
            it = schemaTOC.begin();

            aWriter << schemaTOC.size() << " ";

            while (it != schemaTOC.end())
            {
                aWriter << it->first << " " << it->second << " ";
                it++;
            }

            //write the row info
            std::map<uint32_t, int>::iterator it2;
            it2 = rowTOC.begin();

            aWriter << rowTOC.size() << " ";

            while (it2 != rowTOC.end())
            {
                aWriter << it2->first << " " << it2->second << " ";
                it2++;
            }

            return StatusResult();
        }

        StatusResult decode(std::istream& aReader) //load
        {
            //should I be making multiple blocks here
            int schemaTOCsize;
            std::string name;
            uint32_t idnumber;
            int blockNumber;

            //read schemas
            aReader >> schemaTOCsize;
            for (int i = 0; i < schemaTOCsize; i++)
            {
                aReader >> name;
                aReader >> blockNumber;
                schemaTOC.insert({ name, blockNumber });
            }

            //read rows
            aReader >> schemaTOCsize;
            for (int i = 0; i < schemaTOCsize; i++)
            {
                aReader >> idnumber;
                aReader >> blockNumber;
                rowTOC.insert({ idnumber, blockNumber });
            }



            return StatusResult();
        }

        void makeTesetSchemaNames()
        {

            schemaTOC.insert({ "yup", 1 });

        }

        BlockType getType() const {
            return BlockType::meta_block;
        }
        std::string test;
        StorageBlock metaBlock{ BlockType::meta_block };
        std::map<std::string, int> schemaTOC; //name and block number
        std::map<uint32_t, int> rowTOC;    //bad idea for scalability
    };

    /*
    */
    class BlockListener {
    public:
        BlockListener(BlockType type = BlockType::data_block) :type(type) {}

        ~BlockListener() {}
        void operator()(StorageBlock& aBlock) {
            //if (static_cast<char>(aBlock.header.type) == static_cast<char>(type))
            indexes.push_back({ aBlock.header.blockNum, BlockType{ aBlock.header.type } });
        }
        BlockType type;
        std::vector<std::pair<uint32_t, BlockType>> indexes;
    };

    // USE: Our storage manager class...
    class Storage {
    public:

        Storage(const std::string aName, CreateNewStorage);
        Storage(const std::string aName, OpenExistingStorage);
        ~Storage();
        uint32_t        getTotalBlockCount();

        typedef enum {
            open = 0,
            close = 1
        } streamStatus;

        void changeStream(streamStatus stat);


        //high-level IO (you're not required to use this, but it may help)...    
        StatusResult    save(Storable& aStorable, int startBlock = -1); //using a stream api
        //StatusResult    load(Storable& aStorable); //using a stream api
        StatusResult    BlockToStorable(Storable& aStorable, int startBlock);
        //low-level IO...    
        StatusResult    readBlock(StorageBlock& aBlock, uint32_t aBlockNumber);
        StatusResult    writeBlock(StorageBlock& aBlock, uint32_t aBlockNumber);
        StatusResult    markBlockFree(int blocknumber);

        //so the slash doesn't have to be hard-coded depending on OS
        const char      getOSslash() { return OSslash; };
        void            setOSslash();
        StatusResult    findFreeBlockNum();

        //Table of Contents building
        StatusResult   each(BlockListener* aListener);

        char            OSslash;                     //operating system
        StatusResult    status;

    protected:
        bool            isReady() const;
        bool            isEmptyBlock(int i);
        StatusResult    appendBlock(int blockNumber);
        int             getBlockSize() { return (kPayloadSize + BlockHeader::getSize()); }

        std::string     name;

        std::fstream    stream;
        std::string     ext = ".db";


    };

    class fileHandler {
    private:
        std::fstream fileStream;
        std::string defaultPath = std::string(StorageInfo::getDefaultStoragePath());
        std::string txtextension = ".db";
        std::string fwdslash;
        CheckFile fileChecker;
        FolderReader folderReader;


    public:
        fileHandler() {
            folderReader.path = std::string(StorageInfo::getDefaultStoragePath());
            if (!folderReader.exists(folderReader.path))
                folderReader.createFolder(folderReader.path);
            folderReader.each(fileChecker, "");

            if (isWindows()) {
                fwdslash = "\\";
            }
            else {
                fwdslash = "/";
            }
        }

        //create db file
        StatusResult createDBFile(std::string dbname, std::fstream& aStream, Storage& theDB)
        {
            StatusResult theResult;
            //folderReader.each(fileChecker, "");
            std::string fullPath = getDatabasePath(dbname) + txtextension;
            //std::clog << fullPath << std::endl;
            if (!doesFileExist(dbname + txtextension)) //check if file exists already
            {
                std::string fullPath = defaultPath + fwdslash + dbname + txtextension;
                aStream.open(fullPath, std::fstream::out | std::fstream::binary);
                //fileStream.open(fullPath, std::fstream::out);

                bool failed = !std::ifstream(fullPath.c_str());
                if (failed) theResult.code = Errors::writeError;
            }
            else {
                theResult.code = Errors::databaseExists;
            }

            return theResult;
        }

        bool doesFileExist(std::string dbname)
        {
            bool found = false;
            std::vector <std::string> files = folderReader.getDirectoryFiles(defaultPath);
            for (auto file : files) {
                if (dbname + txtextension == file) {
                    found = true;
                    break;
                }
            }
            return found;
        }

        //close stream
        bool closeCurrentFile()
        {
            if (fileStream.is_open())
                fileStream.close();

            return fileStream.is_open();
        }

        //returns 1 if proper delete
        bool deleteFile(std::string dbname)
        {
            std::string fullPath = defaultPath + fwdslash + dbname + txtextension;
            std::remove(fullPath.c_str());
            bool failed = !std::ifstream(fullPath.c_str());
            return failed;
        }

        std::vector<std::string> getListOfDBFiles()
        {
            return fileChecker.files;
        }
    };
}

#endif /* Storage_hpp */