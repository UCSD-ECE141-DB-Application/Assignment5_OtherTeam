//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "AppProcessor.hpp"
#include "Tokenizer.hpp"
#include <memory>
#include <vector>
#include <algorithm>

namespace ECE141 {
  
  class VersionStatement : public Statement {
  public:
    VersionStatement() :  Statement(Keywords::version_kw) {}
  };

  //.....................................

  AppCmdProcessor::AppCmdProcessor(CommandProcessor *aNext) : CommandProcessor(aNext) {
  }
  
  AppCmdProcessor::~AppCmdProcessor() {}
  
  // USE: -----------------------------------------------------
  StatusResult AppCmdProcessor::interpret(Statement &aStatement) {
    //STUDENT: write code related to given statement
      StatusResult myStatus = StatusResult();

      switch(aStatement.getType())
      {
      case Keywords::version_kw: 
          myStatus=doVersionCommand();
          break;
      case Keywords::quit_kw:
          myStatus=doQuitCommand();
          break;
      case Keywords::help_kw: // how to handle help specific_thing
          myStatus=doHelpCommand();
          break;
      }

    return myStatus;
  }

  StatusResult AppCmdProcessor::doHelpCommand()
  {
      StatusResult myStatus = StatusResult();
      std::map<ECE141::Keywords, std::string> helpText = {
          {ECE141::Keywords::help_kw,    " -- help <command>(optional) - shows details of command or commands "},
          {ECE141::Keywords::version_kw, " -- version - shows the current version of the application"},
          {ECE141::Keywords::quit_kw,    " -- quit - terminates the application"},
          {ECE141::Keywords::create_kw,  " -- create database <name> - creates a new database with name"},
          {ECE141::Keywords::drop_kw,    " -- drop database <name> - drops or deletes the named database"},
          {ECE141::Keywords::use_kw,     " -- use database <name> - selects the database for use"},
          {ECE141::Keywords::describe_kw," -- describe database <name> - describes the database "},
          {ECE141::Keywords::show_kw,    " -- show databases - shows list of available databases"},
          {ECE141::Keywords::unknown_kw, "\n -- help <command>(optional) - shows details of command or commands \n" 
                                         " -- version - shows the current version of the application \n" 
                                         " -- quit - terminates the application \n"  
                                         " -- create database <name> - creates a new database with name\n"
                                         " -- drop database <name> - drops or deletes the named database\n" 
                                         " -- use database <name> - selects the database for use\n" 
                                         " -- describe database <name> - describes the database \n" 
                                         " -- show databases - shows list of available databases\n" }
      };
      
      
      std::cout << "help: " << helpText[help_identifier] << std::endl;
      
 
      return myStatus;
  }

  StatusResult AppCmdProcessor::doQuitCommand()
  {
      StatusResult myStatus = StatusResult();
      std::cout << "quitting: " << std::endl;;
      myStatus.code = userTerminated;
      return myStatus;
  }
  StatusResult AppCmdProcessor::doVersionCommand()
  {
      StatusResult myStatus = StatusResult();
      std::cout << "version: " << getVersionNumber() << std::endl;;
      return myStatus;
  }
  
  
  
  // USE: factory to create statement based on given tokens...
  Statement* AppCmdProcessor::getStatement(Tokenizer &aTokenizer) {
    aTokenizer.restart();
    Token &theToken = aTokenizer.current();
    aTokenizer.next();
    std::vector<ECE141::Keywords> myTerms = { Keywords::version_kw, Keywords::quit_kw, Keywords::help_kw };
    

    auto it = std::find(myTerms.begin(), myTerms.end(), theToken.keyword);
    if (it != myTerms.end())
    {
        checkCmdIdentifiers(aTokenizer, theToken.keyword);
        return new Statement(theToken.keyword);
    }


    return nullptr;
  }
  // Checks if keywords may have identifiers attached. i.e. does help have a specific target
  void AppCmdProcessor::checkCmdIdentifiers(Tokenizer& aTokenizer, ECE141::Keywords currentKey) {

      if (aTokenizer.more() && currentKey == Keywords::help_kw)
      {
          help_identifier = aTokenizer.current().keyword;
          aTokenizer.next();
      }
  }
  
}
