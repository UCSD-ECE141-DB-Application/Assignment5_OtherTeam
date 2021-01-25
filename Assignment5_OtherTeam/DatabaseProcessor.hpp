//
//  AppProcessor.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef DbProcessor_hpp
#define DbProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include "View.hpp"
#include "Database.hpp"


namespace ECE141 {


    class DbCmdProcessor : public CommandProcessor {
    public:

        DbCmdProcessor(CommandProcessor* aNext = nullptr);
        virtual ~DbCmdProcessor();

        virtual Statement* getStatement(Tokenizer& aTokenizer);
        virtual StatusResult  interpret(Statement& aStatement);


    private:
        DatabaseProcessorView DBPView;
        Database* currDB;

        std::string getVersionNumber() { return "ECE141b-4"; }

        StatusResult doCreate(Statement* aStatement);
        StatusResult doDrop(Statement* aStatement);
        StatusResult doUse(Statement* aStatement);
        StatusResult doDescribe(Statement* aStatement);
        StatusResult doShow(Statement* aStatement);
        Database* getActiveDatabase() { return currDB; }
    };



}

#endif /* DbProcessor_hpp */
