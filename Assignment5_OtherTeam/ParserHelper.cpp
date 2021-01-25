//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//
#include "ParserHelper.hpp"
#include "Tokenizer.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <algorithm>
#include <fstream>

#include "FolderReader.hpp"
#include "FileChecker.hpp"


namespace ECE141 {

    //--------------- Run overloads for statement subclasses -------------
    bool validateName(std::string name)
    {
        std::vector<char> validChars = { 'A', 'B','C', 'D','E','F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                        'a', 'b','c', 'd','e','f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                        '#', '$', '@', '_' };


        for (auto c : name)
        {
            auto it = std::find(validChars.begin(), validChars.end(), c);
            if (it == validChars.end())
                return false;
        }

        return true;
    }

    //Statement runs
    StatusResult CreateDBStatement::run(std::ostream& astream)
    {
        StatusResult myStatus;
        /* StatusResult myStatus = getParseResult();

         if (myStatus.code == Errors::noError) {
             Database currDB(idName, CreateNews);
             myStatus = currDB.getStorage().status;
         }*/

        return myStatus;
    }

    StatusResult DropDBStatement::run(std::ostream& astream)
    {
        StatusResult myStatus;
        //StatusResult myStatus = getParseResult();
        /*fileHandler deleteHandler;

        if (myStatus.code == Errors::noError) {
            if (deleteHandler.doesFileExist(idName)) {
                deleteHandler.deleteFile(idName);
            }
            else {
                myStatus.code = Errors::deletionError;
            }
        }*/

        return myStatus;
    }

    StatusResult UseDBStatement::run(std::ostream& astream)
    {
        StatusResult myStatus;
        //StatusResult myStatus = getParseResult();
        //fileHandler aHandler;

       /* if (!aHandler.doesFileExist(idName))
            myStatus.code = Errors::unknownDatabase;

        if (myStatus.code == Errors::noError) {
            Database currDB(idName, OpenStorages);
            myStatus = currDB.getStorage().status;
        }*/

        return myStatus;
    }

    StatusResult DescribeDBStatement::run(std::ostream& astream)
    {
        StatusResult myStatus;
        //StatusResult myStatus = getParseResult();
   /*     fileHandler aHandler;

        if (myStatus.code == Errors::noError) {
            Database currDB(idName, OpenStorages);
            myStatus = currDB.getStorage().status;
            currDB.describeDatabase(idName);
        }*/

        return myStatus;
    }

    StatusResult ShowDBStatement::run(std::ostream& astream)
    {
        fileHandler aHandler;
        DisplayTable tv(&aHandler);
        tv.show(std::cout);
        
        return StatusResult{};
    }

    //.....................................

    //-----------------------------------------------------------------
    //helper functions

    Keywords safeGetCurrentKeyword(Tokenizer& aTokenizer) {
        return aTokenizer.current().keyword;
    }

    std::string safePeekNextData(Tokenizer& aTokenizer) {
        if (aTokenizer.remaining() > 1)
            return aTokenizer.peek().data;
        else return "";
    }

    Keywords safePeekNextKeyword(Tokenizer& aTokenizer) {
        if (aTokenizer.remaining() > 1)
            return aTokenizer.peek().keyword;
        else return Keywords::unknown_kw;
    }

    std::string combineTokens(Tokenizer& aTokenizer)
    {
        std::string out;
        while (aTokenizer.more()) {
            if (aTokenizer.current().data == "\n" || aTokenizer.current().data == "\r" || aTokenizer.current().data == "(")
                break;
            out += aTokenizer.current().data;
            aTokenizer.next();
        }

        return out;
    }

    Token& getNextToken(Tokenizer& aTokenizer) {
        Token& temp = aTokenizer.current();
        aTokenizer.next();
        return temp;
    }

    void flushToken(Tokenizer& aTokenizer) {
        aTokenizer.next();
        return;
    }

    bool isBeginning(Tokenizer& aTokenizer) {
        if (aTokenizer.current().data == "(") {
            return true;
        }

        return false;
    }

    bool isEnd(Tokenizer& aTokenizer) {
        if (aTokenizer.current().data == ")") {
            return true;
        }
        return false;
    }

}