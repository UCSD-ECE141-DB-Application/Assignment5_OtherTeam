//
//  AppProcessor.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef AppProcessor_hpp
#define AppProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"

namespace ECE141 {


  class AppCmdProcessor : public CommandProcessor {
  public:
    
    AppCmdProcessor(CommandProcessor *aNext=nullptr);
    virtual ~AppCmdProcessor();
    
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(Statement &aStatement);
  
  private: 
     void checkCmdIdentifiers(Tokenizer& aTokenizer, ECE141::Keywords currentKey);
     
     StatusResult doHelpCommand();
     StatusResult doQuitCommand();
     StatusResult doVersionCommand();

     std::string getVersionNumber()
     {
         return "ECE141b-1";
     }
     
    Keywords help_identifier = Keywords::unknown_kw;
  };

}

#endif /* AppProcessor_hpp */
