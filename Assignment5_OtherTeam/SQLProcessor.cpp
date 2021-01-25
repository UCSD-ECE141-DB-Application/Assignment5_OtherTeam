//
//  SQLProcessor.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "SQLProcessor.hpp"
#include <functional>
#include "ParserHelper.hpp"

#include <map>
#include <memory>
#include <algorithm>

namespace ECE141 {
    bool isFloat(Token& aToken) {
        if ((aToken.data).find(".") == std::string::npos) {
            return false;
        }
        return true;
    }
    //-----------------------------
    // Action
    //-----------------------------
    StatusResult SQLProcessor::doCreate(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        CreateTableStatement* statement = dynamic_cast<CreateTableStatement*>(aStatement);
        //myResult = statement->run(std::cout);
        myResult = statement->getParseResult();
        Database* currDB = getActiveDatabase();
        Schema* newSchema = new Schema(statement->idName);
        if (myResult.code == Errors::noError) {
            if (currDB == nullptr) {
                return myResult = StatusResult(Errors::noDatabaseSpecified);
            }

            //first checks if the table exists
            if (currDB->getSchemaFromName(statement->idName) != nullptr) {
                return myResult = StatusResult(Errors::tableExists);
            }

            for (auto token_itr = statement->theTokens.begin(); token_itr != statement->theTokens.end(); token_itr++) {
                Attribute anAttribute;
                anAttribute.setName(token_itr->data); //first token is always name ??? 
                while (token_itr->data != ",") {
                    token_itr++;    //go to next token

                    //this is gross. quick fix
                    if (token_itr->keyword == Keywords::varchar_kw || token_itr->keyword == Keywords::primary_kw || token_itr->keyword == Keywords::not_kw || token_itr->keyword == Keywords::default_kw) {
                        myResult = handleMultiField(anAttribute, token_itr);
                    }
                    else {
                        setSingleField(anAttribute, *token_itr);
                    }
                    if (myResult.code != Errors::noError) {
                        return myResult;
                    }

                }
                newSchema->addAttribute(anAttribute);
            }
            currDB->createTable(newSchema);     //add the schema when done adding all attributes
        }

        //output view
        std::string status_string = (myResult) ? "(OK)" : "(Fail)";
        std::cout << "Creating Table: " << statement->idName << " " << status_string << std::endl;
        return myResult;
    }
    StatusResult SQLProcessor::doDrop(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        DropTableStatement* statement = dynamic_cast<DropTableStatement*>(aStatement);
        myResult = statement->getParseResult();
        Database* currDB = getActiveDatabase();
        if (currDB == nullptr) {
            return myResult = StatusResult(Errors::noDatabaseSpecified);
        }
        if (myResult.code == Errors::noError) {
            if (currDB->getSchemaFromName(statement->idName) == nullptr) {
                myResult = StatusResult(Errors::unknownTable);
            }
            else {
                myResult = currDB->dropTable(statement->idName);
            }
        }

        std::string status_string = (myResult) ? "(OK)" : "(Fail)";
        std::cout << "Dropping Table: " << statement->idName << " " << status_string << std::endl;
        return myResult;
    }
    StatusResult SQLProcessor::doUse(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        UseTableStatement* statement = dynamic_cast<UseTableStatement*>(aStatement);
        //myResult = statement->run(std::cout);
        myResult = statement->getParseResult();


        if (myResult.code == Errors::noError) {
            Database* currDB = getActiveDatabase();
            if (currDB == nullptr) {
                return myResult = StatusResult(Errors::noDatabaseSpecified);
            }

            if (currDB->getSchemaFromName(statement->idName) == nullptr) {
                myResult = StatusResult(Errors::unknownTable);
            }
            else {
                thisTable = currDB->getSchemaFromName(statement->idName);
                myResult = StatusResult(Errors::noError);
            }
        }
        std::string status_string = (myResult) ? "(OK)" : "(Fail)";
        std::cout << "Using Table: " << statement->idName << " " << status_string << std::endl;
        return myResult;
    }
    StatusResult SQLProcessor::doDescribe(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        DescribeTableStatement* statement = dynamic_cast<DescribeTableStatement*>(aStatement);
        myResult = statement->getParseResult();

        if (myResult.code == Errors::noError) {
            Database* currDB = getActiveDatabase();
            if (currDB == nullptr) {
                return myResult = StatusResult(Errors::noDatabaseSpecified);
            }
            else {
                myResult = currDB->describeTable(statement->idName);
            }
        }

        return myResult;
    }
    StatusResult SQLProcessor::doShow(Statement* aStatement)
    {
        StatusResult myResult = StatusResult();
        ShowTableStatement* statement = dynamic_cast<ShowTableStatement*>(aStatement);
        myResult = statement->getParseResult();

        if (myResult.code == Errors::noError) {
            Database* currDB = getActiveDatabase();
            if (currDB == nullptr) {
                return myResult = StatusResult(Errors::noDatabaseSpecified);
            }

            myResult = currDB->showTables();
        }
        return myResult;
    }

    StatusResult SQLProcessor::doInsert(Statement* aStatement) {
        StatusResult myResult = StatusResult();
        InsertRowStatement* statement = dynamic_cast<InsertRowStatement*>(aStatement);
        myResult = statement->getParseResult();
        KeyValues* toInsert = new KeyValues;
        Database* currDB = getActiveDatabase();
        if (currDB == nullptr) {
            return StatusResult(Errors::noDatabaseSpecified);
        }

        if (myResult.code == Errors::noError) {
            if (thisTable = doesTableExistinDB(currDB, statement->idName)) {
                myResult = makeKeyVals(statement->theFields, statement->theValueTokens, toInsert);
            }
            else {
                myResult.code = Errors::unknownTable;
            }
        }
        return myResult;
    }

    StatusResult SQLProcessor::doDelete(Statement* aStatement) {
        StatusResult myResult = StatusResult();
        DeleteRowStatement* statement = dynamic_cast<DeleteRowStatement*>(aStatement);
        myResult = statement->getParseResult();

        Database* currDB = getActiveDatabase();
        if (currDB == nullptr)
            return StatusResult(Errors::noDatabaseSpecified);

        if (myResult.code == Errors::noError) {
            Schema* targetSchema = currDB->getSchemaFromName(statement->idName);
            if (targetSchema == nullptr)
                return StatusResult(Errors::unknownTable);
            myResult = currDB->deleteRows(targetSchema);
        }

        return myResult;
    }

    StatusResult SQLProcessor::makeKeyVals(std::vector<std::string> fields, std::vector<Token> values, KeyValues* toInsert) {
        StatusResult theResult = StatusResult(Errors::unknownAttribute);    //probably need a better errorcode? will find one later
        Database* currDB = getActiveDatabase();
        auto valPtr = values.begin();
        bool errorFlag = false;
        while (valPtr != values.end()) {
            for (auto aField : fields) {
                if (thisTable->doesAttributeExist(aField)) {
                    ValueType* theRealVal = new ValueType;
                    theResult = makeCorrectDataType(*valPtr, aField, theRealVal);
                    if (theResult.code != Errors::noError)
                        return theResult;
                    (*toInsert)[aField] = *theRealVal;
                    valPtr++;
                }
                else {
                    errorFlag = true;
                    break;
                }
            }
            theResult = thisTable->validate(*toInsert);
            if (theResult.code != Errors::noError)
                break;
            Row* theRow = new Row;
            theRow->addKeyValue(thisTable->getName(), toInsert);
            theResult = currDB->insertRow(theRow, thisTable);
            if (theResult.code != Errors::noError)
                break;
        }
        //not sure if i still need this will check later
        if (toInsert->size() && !errorFlag)
            theResult.code = Errors::noError;

        return theResult;
    }



    // DataTypes no_type = 'N', bool_type = 'B', datetime_type = 'D', float_type = 'F', int_type = 'I', varchar_type = 'V',
    StatusResult SQLProcessor::makeCorrectDataType(Token& aToken, const std::string& aName, ValueType* val) {
        StatusResult theResult = StatusResult(Errors::noError);
        std::optional<DataType> d = thisTable->getAttributeDataType(aName);
        switch (*d) {
        case DataType::bool_type: {
            //std::string dataUpper;
            //std::transform((aToken.data).begin(), (aToken.data).end(), dataUpper.begin(), ::toupper);
            if ((aToken.data) == "FALSE") {
                *val = false;
            }
            else {
                *val = true;
            }
            break;
        }
        case DataType::float_type: {
            *val = std::stof(aToken.data);
            break;
        }
        case DataType::int_type: {
            try {
                *val = static_cast<uint32_t>(std::stoi(aToken.data)); //why?...
            }
            catch (const std::invalid_argument & ia) {
                theResult = StatusResult(Errors::invalidArguments);
            }
            break;
        }
        case DataType::varchar_type: {
            *val = aToken.data;
            break;
        }
        }

        return theResult;
    }

    Schema* SQLProcessor::doesTableExistinDB(Database* theDB, const std::string& aTable) {
        Schema* targetSchema = theDB->getSchemaFromName(aTable);
        if (targetSchema != nullptr) {
            return targetSchema;
        }
        return nullptr;
    }
    StatusResult SQLProcessor::handleMultiField(Attribute& anAttribute, allTokens::iterator aTokenitr) {

        StatusResult theResult(Errors::invalidAttribute);//assume error
        if (aTokenitr->keyword == Keywords::primary_kw) {
            if ((aTokenitr + 1)->keyword == Keywords::key_kw) {
                anAttribute.setPrimaryKey(true);
                aTokenitr = aTokenitr + 1;  //advance pointer
                theResult = StatusResult(Errors::noError);
            }
        }
        else if (aTokenitr->keyword == Keywords::varchar_kw) {
            if (((aTokenitr + 1)->data == "(") || ((aTokenitr + 2)->type == TokenType::number) || ((aTokenitr + 3)->data == ")")) {
                int len = stoi((aTokenitr + 2)->data);
                anAttribute.setVarCharLength(len);
                anAttribute.setType(DataType::varchar_type);
                aTokenitr = aTokenitr + 3;  //advance pointer
                theResult = StatusResult(Errors::noError);
            }
        }
        else if (aTokenitr->keyword == Keywords::not_kw) {
            if ((aTokenitr + 1)->keyword == Keywords::null_kw) {
                anAttribute.setNullable(false);
                aTokenitr = aTokenitr + 1; //advance pointer
                theResult = StatusResult(Errors::noError);
            }
        }
        else if (aTokenitr->keyword == Keywords::default_kw) {
            if ((aTokenitr + 1)->type == TokenType::number) {
                anAttribute.setDefaultable(true);
                if (isFloat(*(aTokenitr + 1))) {
                    anAttribute.setValue(std::stof((aTokenitr + 1)->data));
                }
                else {
                    anAttribute.setValue(std::stoi((aTokenitr + 1)->data));
                }
                aTokenitr = aTokenitr + 1;  //advance pointer
                theResult = StatusResult(Errors::noError);
            }
            else {
                //std::string dataUpper; 
                //std::transform(((aTokenitr + 1)->data).begin(), ((aTokenitr + 1)->data).end(), dataUpper.begin(),::toupper);
                if ((aTokenitr + 1)->data == "FALSE") {
                    anAttribute.setDefaultable(true);
                    anAttribute.setType(DataType::bool_type);
                    anAttribute.setValue(false);
                    theResult = StatusResult(Errors::noError);
                }
                else if ((aTokenitr + 1)->data == "TRUE") {
                    anAttribute.setDefaultable(true);
                    anAttribute.setType(DataType::bool_type);
                    anAttribute.setValue(true);
                    theResult = StatusResult(Errors::noError);
                }
                aTokenitr = aTokenitr + 1;  //advance pointer
            }
        }
        return theResult;
    }

    void SQLProcessor::setSingleField(Attribute& anAttribute, Token& aToken) {
        switch (aToken.keyword) {
        case Keywords::null_kw: {
            anAttribute.setNullable(true);
            break;
        }
        case Keywords::float_kw: {
            anAttribute.setType(DataType::float_type);
            break;
        }
        case Keywords::integer_kw: {
            anAttribute.setType(DataType::int_type);
            break;
        }
        case Keywords::datetime_kw: {
            anAttribute.setType(DataType::datetime_type);
            break;
        }
        case Keywords::auto_increment_kw: {
            anAttribute.setAutoIncrement(true);
            break;
        }
        case Keywords::boolean_kw: {
            anAttribute.setType(DataType::bool_type);
            break;
        }
        }
    }

    //STUDENT: Implement the SQLProcessor class here...
    SQLProcessor::SQLProcessor(CommandProcessor* aNext) : CommandProcessor(aNext)
    {

    }

    SQLProcessor::SQLProcessor() {}
    SQLProcessor::~SQLProcessor()
    {

    }
    StatusResult SQLProcessor::createTable(const Schema& aSchema)
    {


        return StatusResult();
    }
    StatusResult SQLProcessor::dropTable(const std::string& aName)
    {
        return StatusResult();
    }
    StatusResult SQLProcessor::describeTable(const std::string& aName) const
    {
        return StatusResult();
    }
    StatusResult SQLProcessor::showTables() const
    {
        return StatusResult();
    }

    StatusResult SQLProcessor::interpret(Statement& aStatement) {

        StatusResult myStatus = StatusResult();

        std::map<Keywords, std::function<StatusResult(Statement * aStatement)> > myKeys =
        {
            {Keywords::create_kw, std::bind(&SQLProcessor::doCreate, this, &aStatement)},
            {Keywords::drop_kw, std::bind(&SQLProcessor::doDrop, this,&aStatement)},
            {Keywords::use_kw, std::bind(&SQLProcessor::doUse, this,&aStatement)},
            {Keywords::describe_kw, std::bind(&SQLProcessor::doDescribe, this,&aStatement)},
            {Keywords::show_kw, std::bind(&SQLProcessor::doShow, this,&aStatement)},
            {Keywords::insert_kw, std::bind(&SQLProcessor::doInsert, this,&aStatement)},
            {Keywords::delete_kw, std::bind(&SQLProcessor::doDelete, this,&aStatement)}
        };

        //call appropriate handler
        myStatus = myKeys[aStatement.getType()](&aStatement);

        return myStatus;
    }

    // USE: factory to create statement based on given tokens...
    Statement* SQLProcessor::getStatement(Tokenizer& aTokenizer) {

        aTokenizer.restart(); //make sure at index zero

        

        if (aTokenizer.peek().keyword == ECE141::Keywords::databases_kw || aTokenizer.peek().keyword == ECE141::Keywords::database_kw)
            return nullptr;

        Token& firstToken = getNextToken(aTokenizer);

        //look up table for my keys to handle
        std::vector<Keywords> myKeywords =
        {
            Keywords::create_kw,
            Keywords::drop_kw,
            Keywords::use_kw,
            Keywords::describe_kw,
            Keywords::show_kw,
            Keywords::insert_kw,
            Keywords::delete_kw
        };

        std::map<Keywords, Statement*> statementTypes =
        {
            {Keywords::create_kw,   new CreateTableStatement() },
            {Keywords::drop_kw,     new DropTableStatement },
            {Keywords::use_kw,      new UseTableStatement },
            {Keywords::describe_kw, new DescribeTableStatement },
            {Keywords::show_kw,     new ShowTableStatement },
            {Keywords::insert_kw,   new InsertRowStatement },
            {Keywords::delete_kw,   new DeleteRowStatement}
        };



        if (statementTypes.count(firstToken.keyword)) {
            Statement* st = statementTypes[firstToken.keyword];
            StatusResult parseStatus = st->parse(aTokenizer);
            return st;
        }

        return nullptr;
    }

}