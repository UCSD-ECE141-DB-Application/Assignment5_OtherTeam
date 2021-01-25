//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//
#include "DatabaseProcessor.hpp"
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
#include "ParserHelper.hpp"

namespace ECE141 {
    CreateNewStorage CreateNew;
    OpenExistingStorage OpenStorage;


    //.....................................

    DbCmdProcessor::DbCmdProcessor(CommandProcessor* aNext) : CommandProcessor(aNext), currDB(nullptr) {
    }

    DbCmdProcessor::~DbCmdProcessor() {
        delete currDB;
    }

    /*------------- command functions -------------------------*/

    StatusResult DbCmdProcessor::doCreate(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        CreateDBStatement* statement = dynamic_cast<CreateDBStatement*>(aStatement);
        //myResult = statement->run(std::cout);
        myResult = statement->getParseResult();

        if (myResult.code == Errors::noError) {
            std::string myID = statement->getidName();
            Database* createDB = new Database(myID, CreateNewStorage{});
            myResult = createDB->getStorage().status;
            if (myResult.code == Errors::noError) {
                delete createDB;
            }
        }

        //output view
        std::string status_string = (myResult) ? "(OK)" : "(Fail)";
        DBPView.showString(std::cout, "Creating Database: " + statement->idName + " " + status_string);

        

        return myResult;
    }

    StatusResult DbCmdProcessor::doDrop(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        DropDBStatement* statement = dynamic_cast<DropDBStatement*>(aStatement);
        //myResult = statement->run(std::cout);

        fileHandler deleteHandler;

        if (myResult.code == Errors::noError) {
            if (deleteHandler.doesFileExist(currDB->getName())) {
                if (!deleteHandler.deleteFile(currDB->getName())) {
                    delete currDB;
                    currDB = nullptr;            //once file is deleted, set db pointer to null
                }
                else {
                    myResult.code = Errors::deletionError;
                }
            }
            else {
                myResult.code = Errors::deletionError;
            }
        }

        //output to view
        std::string status_string = (myResult) ? "(OK)" : "(Fail)";
        DBPView.showString(std::cout, "Dropping Database: " + statement->idName + " " + status_string);
        return myResult;
    }
    StatusResult DbCmdProcessor::doUse(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        UseDBStatement* statement = dynamic_cast<UseDBStatement*>(aStatement);
        //myResult = statement->run(std::cout);
        myResult = statement->getParseResult();

        fileHandler aHandler;

        if (myResult.code == Errors::noError) {
            if (aHandler.doesFileExist(statement->getidName())) {
                if (currDB == nullptr) {                                    //first run probably?
                    currDB = new Database(statement->getidName(), OpenExistingStorage{});
                }
                else if (currDB->getName() != statement->getidName()) {       //switch database
                    Database* temp = new Database(statement->getidName(), OpenExistingStorage{});
                    delete currDB;
                    currDB = temp;
                }
            }
            else {
                myResult.code = Errors::unknownDatabase;
            }
        }

        std::string status_string = (myResult) ? "(OK)" : "(Fail)";
        DBPView.showString(std::cout, "Using Database: " + statement->idName + " " + status_string);
        return myResult;
    }

    StatusResult DbCmdProcessor::doDescribe(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        DescribeDBStatement* statement = dynamic_cast<DescribeDBStatement*>(aStatement);
        //myResult = statement->run(std::cout);
        myResult = statement->getParseResult();

        fileHandler aHandler;

        if (myResult.code == Errors::noError) {
            //will probably move this if elseif else in all these do methods into a separate function later
            if (currDB == nullptr) {
                currDB = new Database(statement->getidName(), OpenExistingStorage{});
            }
            else if (currDB->getName() != statement->getidName()) {
                Database* temp = new Database(statement->getidName(), OpenExistingStorage{});
                delete currDB;
                currDB = temp;
            }

            currDB->describeDatabase(currDB->getName());

        }

        std::string status_string = (myResult) ? "(OK)" : "(Fail)";
        DBPView.showString(std::cout, "Describing Database: " + statement->idName + " " + status_string);
        return myResult;
    }

    StatusResult DbCmdProcessor::doShow(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        ShowDBStatement* statement = dynamic_cast<ShowDBStatement*>(aStatement);
        myResult = statement->run(std::cout);

        std::string status_string = (myResult) ? "(OK)" : "(Fail)";
        DBPView.showString(std::cout, "Showing Databases: " + status_string);
        return myResult;
    }


    // USE: -----------------------------------------------------
    // Dispatches keywords to command specific functions
    StatusResult DbCmdProcessor::interpret(Statement& aStatement) {

        StatusResult myStatus = StatusResult();

        std::map<Keywords, std::function<StatusResult(Statement * aStatement)> > myKeys =
        {
            {Keywords::create_kw, std::bind(&DbCmdProcessor::doCreate, this, &aStatement)},
            {Keywords::drop_kw, std::bind(&DbCmdProcessor::doDrop, this,&aStatement)},
            {Keywords::use_kw, std::bind(&DbCmdProcessor::doUse, this,&aStatement)},
            {Keywords::describe_kw, std::bind(&DbCmdProcessor::doDescribe, this,&aStatement)},
            {Keywords::show_kw, std::bind(&DbCmdProcessor::doShow, this,&aStatement)}
        };

        //call appropriate handler
        myStatus = myKeys[aStatement.getType()](&aStatement);

        return myStatus;
    }

    // USE: factory to create statement based on given tokens...
    Statement* DbCmdProcessor::getStatement(Tokenizer& aTokenizer) {

        //check to make sure correct format of statement

        aTokenizer.restart(); //make sure at index zero

        //if not a database command; go to next in chain
        if (aTokenizer.peek().keyword != ECE141::Keywords::databases_kw && aTokenizer.peek().keyword != ECE141::Keywords::database_kw) {
            return nullptr;
        }

        Token& firstToken = getNextToken(aTokenizer);

        //look up table for my keys to handle
        std::vector<Keywords> myKeywords =
        {
            Keywords::create_kw,
            Keywords::drop_kw,
            Keywords::use_kw,
            Keywords::describe_kw,
            Keywords::show_kw
        };

        std::map<Keywords, Statement*> statementTypes =
        {
            {Keywords::create_kw,   new CreateDBStatement() },
            {Keywords::drop_kw,     new DropDBStatement },
            {Keywords::use_kw,      new UseDBStatement },
            {Keywords::describe_kw, new DescribeDBStatement },
            {Keywords::show_kw,     new ShowDBStatement }
        };

        if (statementTypes.count(firstToken.keyword)) {
            Statement* st = statementTypes[firstToken.keyword];
            StatusResult parseStatus = st->parse(aTokenizer);
            
            return st;
        }

        return nullptr;
    }

    //-----------------------------------------------------------------
    //helper functions

    //Keywords safeGetCurrentKeyword(Tokenizer& aTokenizer) {
    //    return aTokenizer.current().keyword;
    //}

    //std::string safePeekNextData(Tokenizer& aTokenizer) {
    //    if (aTokenizer.remaining() > 1)
    //        return aTokenizer.peek().data;
    //    else return "";
    //}
    //std::string combineTokens(Tokenizer& aTokenizer)
    //{
    //    std::string out;
    //    while (aTokenizer.more()) {
   //         if (aTokenizer.current().data == "\n" || aTokenizer.current().data == "\r")
   //             break;
    //        out += aTokenizer.current().data;
    //        aTokenizer.next();
    //    }

     //   return out;
    //}
}