//
//  AppProcessor.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef ParserHelper_hpp
#define ParserHelper_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include "View.hpp"
#include "Database.hpp"
#include "SQLProcessor.hpp"
#include "Attribute.hpp"
#include "Helpers.hpp"

namespace ECE141 {

    using valid_types = std::variant<std::string, Keywords, std::pair<Keywords, int>, std::pair<Keywords, float>>;
    using collectionOfAttributes = std::vector<std::variant<std::string, Keywords, std::pair<Keywords, int>, std::pair<Keywords, float>>>;

    Keywords safeGetCurrentKeyword(Tokenizer& aTokenizer);
    std::string safePeekNextData(Tokenizer& aTokenizer);
    Keywords safePeekNextKeyword(Tokenizer& aTokenizer);
    std::string combineTokens(Tokenizer& aTokenizer);
    bool validateName(std::string name);
    bool isBeginning(Tokenizer& aTokenizer);
    bool isEnd(Tokenizer& aTokenizer);


    Token& getNextToken(Tokenizer& aTokenizer);
    void flushToken(Tokenizer& aTokenizer);

    class CreateDBStatement : public Statement {
    public:
        CreateDBStatement(Keywords build, std::string name) : Statement(Keywords::create_kw), toBuildType(build), idName(name) {}
        CreateDBStatement() : Statement(Keywords::create_kw) {}

        CreateDBStatement(Statement& aStatement)
        {
            stmtType = aStatement.getType();
        }

        StatusResult parse(Tokenizer& aTokenizer)
        {
            toBuildType = safeGetCurrentKeyword(aTokenizer);

            if (aTokenizer.current().keyword == Keywords::database_kw) {
                aTokenizer.next();
                idName = combineTokens(aTokenizer);
                while (aTokenizer.more())
                    aTokenizer.next();
            }
            else
                return parseResult = StatusResult(Errors::invalidArguments);

            if (toBuildType == Keywords::unknown_kw)
                return parseResult = StatusResult(Errors::unknownCommand);

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);

            return parseResult = StatusResult();
        }

        StatusResult run(std::ostream& aStream);

        StatusResult getParseResult() { return parseResult; }
        std::string getidName() { return idName; }

        Keywords toBuildType;
        std::string idName;
        StatusResult parseResult;

    };

    class DropDBStatement : public Statement {
    public:
        DropDBStatement(Keywords aDrop, std::string name) : Statement(Keywords::drop_kw), toDropType(aDrop), idName(name) {}
        DropDBStatement() : Statement(Keywords::drop_kw) {}

        StatusResult run(std::ostream& aStream);

        StatusResult parse(Tokenizer& aTokenizer)
        {
            toDropType = safeGetCurrentKeyword(aTokenizer);

            if (aTokenizer.current().keyword == Keywords::database_kw) {
                aTokenizer.next();
                idName = combineTokens(aTokenizer);
                while (aTokenizer.more()) //make sure empty
                    aTokenizer.next();
            }
            else
                return parseResult = StatusResult(Errors::invalidArguments);

            if (toDropType == Keywords::unknown_kw)
                return parseResult = StatusResult(Errors::unknownCommand);

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);

            return parseResult = StatusResult();
        }

        StatusResult getParseResult() { return parseResult; }

        StatusResult parseResult;
        Keywords toDropType;
        std::string idName;
    };

    class UseDBStatement : public Statement {
    public:
        UseDBStatement(Keywords aUseType, std::string name) : Statement(Keywords::use_kw), toUseType(aUseType), idName(name) {};
        UseDBStatement() : Statement(Keywords::use_kw) {}

        StatusResult run(std::ostream& aStream);

        StatusResult parse(Tokenizer& aTokenizer)
        {
            toUseType = safeGetCurrentKeyword(aTokenizer);

            if (aTokenizer.current().keyword == Keywords::database_kw) {
                aTokenizer.next();
                idName = combineTokens(aTokenizer);
                while (aTokenizer.more())
                    aTokenizer.next();
            }
            else
                return parseResult = StatusResult(Errors::invalidArguments);

            if (toUseType == Keywords::unknown_kw)
                return parseResult = StatusResult(Errors::unknownCommand);

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);

            return parseResult = StatusResult();
        }

        StatusResult getParseResult() { return parseResult; }
        std::string getidName() { return idName; }

        StatusResult parseResult;
        Keywords toUseType;
        std::string idName;
    };

    class DescribeDBStatement : public Statement {
    public:
        DescribeDBStatement(Keywords aUseType, std::string name) : Statement(Keywords::describe_kw), toUseType(aUseType), idName(name) {};
        DescribeDBStatement() : Statement(Keywords::describe_kw) {}

        StatusResult run(std::ostream& aStream);

        StatusResult parse(Tokenizer& aTokenizer)
        {
            toUseType = safeGetCurrentKeyword(aTokenizer);

            if (aTokenizer.current().keyword == Keywords::database_kw) {
                aTokenizer.next();
                idName = combineTokens(aTokenizer);
                while (aTokenizer.more())
                    aTokenizer.next();
            }
            else
                return parseResult = StatusResult(Errors::invalidArguments);

            if (toUseType == Keywords::unknown_kw)
                return parseResult = StatusResult(Errors::unknownCommand);

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);



            return parseResult = StatusResult();
        }

        StatusResult getParseResult() { return parseResult; }
        std::string getidName() { return idName; }

        StatusResult parseResult;
        Keywords toUseType;
        std::string idName = "";
    };

    class ShowDBStatement : public Statement {
    public:
        ShowDBStatement(Keywords aShowType) : Statement(Keywords::show_kw), toShowType(aShowType) {};
        ShowDBStatement() : Statement(Keywords::show_kw) {}

        StatusResult run(std::ostream& aStream);

        StatusResult parse(Tokenizer& aTokenizer)
        {
            if (aTokenizer.current().keyword == Keywords::databases_kw) {//is second word correct
                aTokenizer.next();
                return parseResult = StatusResult();
            }
            if (aTokenizer.current().keyword == Keywords::unknown_kw) {
                aTokenizer.next();
                return parseResult = StatusResult(Errors::unknownCommand);
            }


            return parseResult = StatusResult(Errors::invalidArguments);
        }
        StatusResult getParseResult() { return parseResult; }
        Keywords toShowType;
        StatusResult parseResult;
    };

    //------------------------------------------------------------------Table statements!!!--------------------------------------------------------//
    class CreateTableStatement : public Statement {
    public:
        CreateTableStatement(Keywords build, std::string name) : Statement(Keywords::create_kw), toBuildType(build), idName(name) {}
        CreateTableStatement() : Statement(Keywords::create_kw) {}

        CreateTableStatement(Statement& aStatement)
        {
            stmtType = aStatement.getType();
        }

        StatusResult parse(Tokenizer& aTokenizer)
        {
            toBuildType = safeGetCurrentKeyword(aTokenizer);

            if (aTokenizer.current().keyword == Keywords::table_kw) {
                aTokenizer.next();

                idName = combineTokens(aTokenizer); //accounts for chars like ! being in name so they aren't counted as different tokens

                if (isBeginning(aTokenizer)) {
                    aTokenizer.next();
                    while (!isEnd(aTokenizer)) {
                        parseAttributes(aTokenizer);
                    }
                    aTokenizer.next();//flush ending )
                    Token theEnd{ TokenType::punctuation, Keywords::unknown_kw, Operators::unknown_op, "," };
                    theTokens.push_back(theEnd);                                    //add to end for easier loop control later
                }
                else {
                    return parseResult = StatusResult(Errors::invalidCommand);      //was expecting opening parentheses
                }
            }
            else
                return parseResult = StatusResult(Errors::invalidArguments);

            if (toBuildType == Keywords::unknown_kw)
                return parseResult = StatusResult(Errors::unknownCommand);

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);

            return parseResult = StatusResult(Errors::noError);
        }

        //will return true if at end, else false
        bool parseAttributes(Tokenizer& aTokenizer) {

            while (aTokenizer.current().data != ")") {
                theTokens.push_back(aTokenizer.current());
                if (safePeekNextData(aTokenizer) == "(") {
                    handleInnerParen(aTokenizer);
                }
                else {
                    aTokenizer.next();
                }
            }

            return true;
        }

        void handleInnerParen(Tokenizer& aTokenizer) {
            aTokenizer.next();
            while (aTokenizer.current().data != ")") {
                theTokens.push_back(aTokenizer.current());
                aTokenizer.next();
            }
            theTokens.push_back(aTokenizer.current());      //add the closing paren
            aTokenizer.next();                              //flush inner closing paren
        }

        StatusResult run(std::ostream& aStream);

        StatusResult getParseResult() { return parseResult; }
        std::string getidName() { return idName; }

        Keywords toBuildType;
        std::string idName;
        StatusResult parseResult;
        std::vector<Token> theTokens;

    };

    class DropTableStatement : public Statement {
    public:
        DropTableStatement(Keywords aDrop, std::string name) : Statement(Keywords::drop_kw), toDropType(aDrop), idName(name) {}
        DropTableStatement() : Statement(Keywords::drop_kw) {}

        //StatusResult run(std::ostream& aStream);

        StatusResult parse(Tokenizer& aTokenizer)
        {
            toDropType = safeGetCurrentKeyword(aTokenizer);

            if (aTokenizer.current().keyword == Keywords::table_kw) {
                aTokenizer.next();
                idName = combineTokens(aTokenizer);
                while (aTokenizer.more()) //make sure empty
                    aTokenizer.next();
            }
            else
                return parseResult = StatusResult(Errors::invalidArguments);

            if (toDropType == Keywords::unknown_kw)
                return parseResult = StatusResult(Errors::unknownCommand);

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);

            return parseResult = StatusResult();
        }

        StatusResult getParseResult() { return parseResult; }

        StatusResult parseResult;
        Keywords toDropType;
        std::string idName;
    };

    class UseTableStatement : public Statement {
    public:
        UseTableStatement(Keywords aUseType, std::string name) : Statement(Keywords::use_kw), toUseType(aUseType), idName(name) {};
        UseTableStatement() : Statement(Keywords::use_kw) {}

        StatusResult run(std::ostream& aStream);

        StatusResult parse(Tokenizer& aTokenizer)
        {
            toUseType = safeGetCurrentKeyword(aTokenizer);

            if (aTokenizer.current().keyword == Keywords::table_kw) {
                aTokenizer.next();
                idName = combineTokens(aTokenizer);
                while (aTokenizer.more())
                    aTokenizer.next();
            }
            else
                return parseResult = StatusResult(Errors::invalidArguments);

            if (toUseType == Keywords::unknown_kw)
                return parseResult = StatusResult(Errors::unknownCommand);

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);

            return parseResult = StatusResult();
        }

        StatusResult getParseResult() { return parseResult; }
        std::string getidName() { return idName; }

        StatusResult parseResult;
        Keywords toUseType;
        std::string idName;
    };

    class ShowTableStatement : public Statement {
    public:
        ShowTableStatement(Keywords aShowType) : Statement(Keywords::show_kw), toShowType(aShowType) {};
        ShowTableStatement() : Statement(Keywords::show_kw) {}

        StatusResult run(std::ostream& aStream);

        StatusResult parse(Tokenizer& aTokenizer)
        {
            if (aTokenizer.current().keyword == Keywords::tables_kw) {
                aTokenizer.next();
                return parseResult = StatusResult();
            }
            if (aTokenizer.current().keyword == Keywords::unknown_kw) {
                aTokenizer.next();
                return parseResult = StatusResult(Errors::unknownCommand);
            }


            return parseResult = StatusResult(Errors::invalidArguments);
        }
        StatusResult getParseResult() { return parseResult; }
        Keywords toShowType;
        StatusResult parseResult;
    };

    class DescribeTableStatement : public Statement {
    public:
        DescribeTableStatement(Keywords aUseType, std::string name) : Statement(Keywords::describe_kw), toUseType(aUseType), idName(name) {};
        DescribeTableStatement() : Statement(Keywords::describe_kw) {}

        StatusResult run(std::ostream& aStream);

        StatusResult parse(Tokenizer& aTokenizer)
        {
            toUseType = safeGetCurrentKeyword(aTokenizer);

            if (aTokenizer.current().keyword != Keywords::table_kw) {
                //aTokenizer.next();
                idName = combineTokens(aTokenizer);
                while (aTokenizer.more())
                    aTokenizer.next();
            }
            else
                return parseResult = StatusResult(Errors::invalidArguments);

            //if (toUseType == Keywords::unknown_kw)
                //return parseResult = StatusResult(Errors::unknownCommand);

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);



            return parseResult = StatusResult();
        }

        StatusResult getParseResult() { return parseResult; }
        std::string getidName() { return idName; }

        StatusResult parseResult;
        Keywords toUseType;
        std::string idName = "";
    };
    //-----------------------------------------------------------Row Statements!!--------------------------------------------------------//
    class InsertRowStatement : public Statement {
    public:
        InsertRowStatement(Keywords aUseType, std::string name) : Statement(Keywords::insert_kw), toUseType(aUseType), idName(name) {};
        InsertRowStatement() : Statement(Keywords::insert_kw) {}

        StatusResult parse(Tokenizer& aTokenizer) {
            toUseType = safeGetCurrentKeyword(aTokenizer);
            parseResult = StatusResult(Errors::invalidArguments);

            if (aTokenizer.current().keyword == Keywords::into_kw) {
                aTokenizer.next();
                idName = combineTokens(aTokenizer);
                //i am so sorry about this code...
                if (isBeginning(aTokenizer)) {
                    parseFields(aTokenizer);
                    if (aTokenizer.current().keyword == Keywords::values_kw) {
                        aTokenizer.next();
                        if (isBeginning(aTokenizer)) {
                            while (aTokenizer.more()) {
                                parseResult = parseValues(aTokenizer);
                                if (parseResult.code != Errors::noError) {
                                    return parseResult;
                                }
                            }
                        }
                    }

                }
                else {
                    return parseResult = StatusResult(Errors::invalidArguments);    //was expecting a beginning paren
                }
            }
            else {
                return parseResult = StatusResult(Errors::invalidArguments);
            }

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);

            return parseResult = StatusResult();
        }

        //will return true if at end, else false
        void parseFields(Tokenizer& aTokenizer) {
            aTokenizer.next();
            while (!isEnd(aTokenizer) && aTokenizer.more()) {
                aTokenizer.skipIf(TokenType::punctuation);
                theFields.push_back(aTokenizer.current().data);
                aTokenizer.next();
            }
            aTokenizer.next();
        }

        StatusResult parseValues(Tokenizer& aTokenizer) {
            size_t verifyCount = 0;                     //used to ensure the number of values per row matches the number of fields to fill
            StatusResult theResult = StatusResult(Errors::keyValueMismatch);
            while (aTokenizer.current().data != ")") {
                if (aTokenizer.remaining() == 1 && aTokenizer.current().data != ")") {
                    theResult = StatusResult(Errors::punctuationExpected);
                    break;
                }
                if (!aTokenizer.skipIf(TokenType::punctuation)) {
                    theValueTokens.push_back(aTokenizer.current());
                    verifyCount++;
                    aTokenizer.next();
                }
            }
            if (verifyCount == theFields.size())
                theResult.code = Errors::noError;
            aTokenizer.next();  //flush the right paren
            aTokenizer.next();  //flush the comma

            return theResult;
        }

        Token delimiterToken() {
            Token theEnd{ TokenType::punctuation, Keywords::unknown_kw, Operators::unknown_op, "," };
            return theEnd;
        }

        StatusResult getParseResult() { return parseResult; }
        std::string getidName() { return idName; }
        size_t getFieldCount() { return theFields.size(); }

        StatusResult parseResult;
        Keywords toUseType;
        std::string idName = "";
        std::vector<Token> theValueTokens;
        std::vector<std::string> theFields;
    };

    class DeleteRowStatement : public Statement {
    public:
        DeleteRowStatement(Keywords aUseType, std::string name) : Statement(Keywords::delete_kw), toUseType(aUseType), idName(name) {};
        DeleteRowStatement() : Statement(Keywords::delete_kw) {}

        StatusResult parse(Tokenizer& aTokenizer) {
            toUseType = safeGetCurrentKeyword(aTokenizer);
            parseResult = StatusResult(Errors::invalidArguments);

            if (aTokenizer.current().keyword == Keywords::from_kw) {
                aTokenizer.next();
                idName = combineTokens(aTokenizer);
                //will change this when extending delete to get rid of rows
                while (aTokenizer.more())
                    aTokenizer.next();
            }
            else {
                return parseResult = StatusResult(Errors::invalidArguments);
            }

            if (idName == "") //did they put name
                return parseResult = StatusResult(Errors::noDatabaseSpecified);

            if (!validateName(idName))
                return parseResult = StatusResult(Errors::illegalIdentifier);

            return parseResult = StatusResult();
        }

        StatusResult getParseResult() { return parseResult; }
        std::string getidName() { return idName; }

        StatusResult parseResult;
        Keywords toUseType;
        std::string idName = "";
    };

}

#endif /* ParserHelper_hpp */