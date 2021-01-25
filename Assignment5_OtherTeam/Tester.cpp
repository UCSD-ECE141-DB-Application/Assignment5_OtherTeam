//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//
#include "Tester.hpp"
#include <sstream>

namespace ECE141 {
    //build a tokenizer, tokenize input, ask processors to handle...
    ECE141::StatusResult handleInput(std::istream& aStream, ECE141::CommandProcessor& aProcessor) {
        ECE141::Tokenizer theTokenizer(aStream);

        //tokenize the input from aStream...
        ECE141::StatusResult theResult = theTokenizer.tokenize();
        while (theResult && theTokenizer.more()) {
            if (";" == theTokenizer.current().data) {
                theTokenizer.next();  //skip the ";"...
            }
            else theResult = aProcessor.processInput(theTokenizer);
        }
        return theResult;
    }

    bool Tester::runTests() {
        bool status = true;

        fileTests();


        if (doCreateTests() == false)
            status = false;

        if (doDropTests() == false)
            status = false;
        
        if (doUseTests() == false)
            status = false;

        if (doDescribeTests() == false)
            status = false;

        if (doShowTests() == false)
            status = false;

       return status;
    }

    bool Tester::doCreateTests() {
        //const char* path = ECE141::StorageInfo::getDefaultStoragePath(); 
        ECE141::DbCmdProcessor    dbProccessor;
        ECE141::AppCmdProcessor   theProcessor(&dbProccessor);  //add your db processor here too!
        ECE141::StatusResult      theResult{};

        
        std::string test = "create database temp";
        std::stringstream theStream(test);
        theResult = handleInput(theStream, theProcessor);
        if (!assertTrue("create database temp", theResult.code == Errors::noError))
            return false;

        std::string test2 = "create database ";
        std::stringstream theStream2(test2);
        theResult = handleInput(theStream2, theProcessor);
        if (!assertTrue("create database ", theResult.code == Errors::noDatabaseSpecified))
            return false;

        std::string test3 = "create datase temp";
        std::stringstream theStream3(test3);
        theResult = handleInput(theStream3, theProcessor);
        if (!assertTrue("create datase temp ", theResult.code == Errors::unknownCommand))
            return false;

        std::string test4 = "create database temp";
        std::stringstream theStream4(test4);
        theResult = handleInput(theStream4, theProcessor);
        if (!assertTrue("create database temp", theResult.code == Errors::databaseExists))
            return false;

    }

    bool Tester::doDropTests() {
        //const char* path = ECE141::StorageInfo::getDefaultStoragePath(); 
        ECE141::DbCmdProcessor    dbProccessor;
        ECE141::AppCmdProcessor   theProcessor(&dbProccessor);  //add your db processor here too!
        ECE141::StatusResult      theResult{};

        std::string test5 = "create database temp";
        std::stringstream theStream5(test5);
        theResult = handleInput(theStream5, theProcessor);

        std::string test = "drop database temp";
        std::stringstream theStream(test);
        theResult = handleInput(theStream, theProcessor);
        if (!assertTrue("drop database temp", theResult.code == Errors::noError))
            return false;

        std::string test2 = "drop database ";
        std::stringstream theStream2(test2);
        theResult = handleInput(theStream2, theProcessor);
        if (!assertTrue("drop database ", theResult.code == Errors::noDatabaseSpecified))
            return false;

        std::string test3 = "drop datase temp";
        std::stringstream theStream3(test3);
        theResult = handleInput(theStream3, theProcessor);
        if (!assertTrue("drop datase temp ", theResult.code == Errors::unknownCommand))
            return false;

        std::string test4 = "drop database temp";
        std::stringstream theStream4(test4);
        theResult = handleInput(theStream4, theProcessor);
        if (!assertTrue("drop database temp", theResult.code == Errors::unknownDatabase))
            return false;
    }

    bool Tester::doUseTests() {
        //const char* path = ECE141::StorageInfo::getDefaultStoragePath(); 
        ECE141::DbCmdProcessor    dbProccessor;
        ECE141::AppCmdProcessor   theProcessor(&dbProccessor);  //add your db processor here too!
        ECE141::StatusResult      theResult{};

        std::string test5 = "create database temp";
        std::stringstream theStream5(test5);
        theResult = handleInput(theStream5, theProcessor);

        std::string test = "use database temp";
        std::stringstream theStream(test);
        theResult = handleInput(theStream, theProcessor);
        if (!assertTrue("use database temp", theResult.code == Errors::noError))
            return false;

        std::string test2 = "use database ";
        std::stringstream theStream2(test2);
        theResult = handleInput(theStream2, theProcessor);
        if (!assertTrue("use database ", theResult.code == Errors::noDatabaseSpecified))
            return false;

        std::string test3 = "use datase temp";
        std::stringstream theStream3(test3);
        theResult = handleInput(theStream3, theProcessor);
        if (!assertTrue("use datase temp ", theResult.code == Errors::unknownCommand))
            return false;

        

        return true;
    }

    bool Tester::doDescribeTests() {
        //const char* path = ECE141::StorageInfo::getDefaultStoragePath(); 
        ECE141::DbCmdProcessor    dbProccessor;
        ECE141::AppCmdProcessor   theProcessor(&dbProccessor);  //add your db processor here too!
        ECE141::StatusResult      theResult{};

        std::string test5 = "create database temp";
        std::stringstream theStream5(test5);
        theResult = handleInput(theStream5, theProcessor);

        std::string test4 = "describe database temp";
        std::stringstream theStream4(test4);
        theResult = handleInput(theStream4, theProcessor);
        if (!assertTrue("describe database temp", theResult.code == Errors::noError))
            return false;

        std::string test6 = "describe database ";
        std::stringstream theStream6(test6);
        theResult = handleInput(theStream6, theProcessor);
        if (!assertTrue("describe database ", theResult.code == Errors::noDatabaseSpecified))
            return false;

        return true;
    }

    bool Tester::doShowTests() {

        //const char* path = ECE141::StorageInfo::getDefaultStoragePath(); 
        ECE141::DbCmdProcessor    dbProccessor;
        ECE141::AppCmdProcessor   theProcessor(&dbProccessor);  //add your db processor here too!
        ECE141::StatusResult      theResult{};

        std::string test4 = "show databases";
        std::stringstream theStream4(test4);
        theResult = handleInput(theStream4, theProcessor);
        if (!assertTrue("show databases", theResult.code == Errors::noError))
            return false;

        
        return true;
    }

    void Tester::fileTests()
    {
        std::string path(ECE141::StorageInfo::getDefaultStoragePath());
        std::string path2 = std::string(path) + "Hello";
        FolderReader reader(ECE141::StorageInfo::getDefaultStoragePath());

        //check if valid path
        if (checkExistence(reader, path)) {
            std::clog << "path exists, correct output\n" << std::endl;
        }
        else std::clog << "path exists, incorrect output\n" << std::endl;

        if (checkExistence(reader, path2)) {
            std::clog << "path doesn't exist, incorrect output\n" << std::endl;
        }
        else std::clog << "path doesn't exist, correct output\n" << std::endl;

        //checking FolderView
        std::vector<std::string> correct_files{ "testing.txt", "abcd.txt" };
        FolderView view;
        std::string extension = "Hello";

        std::clog << "output from view with non-existant folder: " << std::endl;
        reader.each(view, extension); 	//sent a folder that doesn't exist should do nothing (behavior is correct)
        std::clog << "\noutput from view with existing folder: " << std::endl;
        reader.each(view, ""); 			//should output the above files (behavior is correct)

        CheckFile c;
        //populate files vector in filechecker
        reader.each(c, "");

        std::clog << "\n\nchecking for file " << extension + ".txt" << std::endl;
        if (c.doesFileExist(extension + ".txt")) {
            std::clog << "file doesn't exist, incorrect output" << std::endl;
        }
        else std::clog << "file doesn't exist, correct output" << std::endl;

        std::string f1("abcd.txt");
        std::clog << "\nchecking for file " << f1 << std::endl;
        if (c.doesFileExist(f1)) {
            std::clog << "file exists, correct output" << std::endl;
        }
        else std::clog << "file exists, incorrect output " << std::endl;
    }


    //
    bool Tester::assertTrue(const char* aMessage, bool aValue) {
        static const char* theMsgs[] = { "PASS\n","FAILED\n" };
        output << "\t" << aMessage << " " << theMsgs[!aValue];
        return aValue;
    }

    //
    bool Tester::assertFalse(const char* aMessage, bool aValue) {
        static const char* theMsgs[] = { "PASS\n","FAILED\n" };
        output << "\t" << aMessage << " " << theMsgs[aValue];
        return false == aValue;
    }
   
}