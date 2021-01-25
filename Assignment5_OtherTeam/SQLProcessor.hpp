//
//  SQLProcessor.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include "Schema.hpp"
#include <map>
#include "Statement.hpp"
#include "Row.hpp"


class Statement;
class Database; //define this later...

namespace ECE141 {
    using allTokens = std::vector<ECE141::Token>;

    bool isFloat(Token& aToken);
    class SQLProcessor : public CommandProcessor {
    public:

        SQLProcessor(CommandProcessor* aNext);
        SQLProcessor();
        virtual ~SQLProcessor();

        //virtual Statement*    getStatement(Tokenizer &aTokenizer);
        //virtual StatusResult  interpret(const Statement &aStatement);

        //actions
        StatusResult doCreate(Statement* aStatement);
        StatusResult doDrop(Statement* aStatement);
        StatusResult doUse(Statement* aStatement);
        StatusResult doDescribe(Statement* aStatement);
        StatusResult doShow(Statement* aStatement);
        StatusResult doInsert(Statement* aStatement);
        StatusResult doDelete(Statement* aStatement);


        StatusResult createTable(const Schema& aSchema);
        StatusResult dropTable(const std::string& aName);
        StatusResult describeTable(const std::string& aName) const;
        StatusResult showTables() const;
        StatusResult interpret(Statement& aStatement);
        Statement* getStatement(Tokenizer& aTokenizer);

        StatusResult handleMultiField(Attribute& anAttribute, allTokens::iterator aTokenitr);
        void setSingleField(Attribute& anAttribute, Token& aToken);
        Schema* doesTableExistinDB(Database* theDB, const std::string& aTable);
        StatusResult makeKeyVals(std::vector<std::string> fields, std::vector<Token> values, KeyValues* toInsert);
        StatusResult makeCorrectDataType(Token& aToken, const std::string& aName, ValueType* val);

        /*  do these in next assignment
            StatusResult insert();
            StatusResult update();
            StatusResult delete();
        */

    protected:
        Schema* thisTable;
        //do you need other data members?
    };

}
#endif/* SQLProcessor_hpp */