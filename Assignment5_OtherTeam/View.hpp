//
//  View.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef View_h
#define View_h

#include <iostream>
//#include "Schema.hpp"
#include <variant>
#include "Schema.hpp"
//#include "Attribute.hpp"
#include "Errors.hpp"
#include <algorithm>
#include <iomanip>
#include "Database.hpp"
#include <cstring>

namespace ECE141 {

    using ColumnValues = std::vector<std::string>;
    using TableValues = std::vector<ColumnValues>;

    struct VisitPackage
    {
        std::string operator()(int val) { return std::to_string(val); }
        std::string operator()(float val) { return std::to_string(val); }
        std::string operator()(std::string val) { return val; }
        std::string operator()(bool val) { return ((val) ? "TRUE" : "FALSE"); }
    };

    //completely generic view, which you will subclass to show information
    class View {
    public:
        virtual         ~View() {}
        virtual bool    show(std::ostream& aStream) = 0;

    };

    class DatabaseProcessorView : public View {
    public:
        DatabaseProcessorView() {}
        
        virtual bool    show(std::ostream& aStream)
        {
            return 0;
        }

        bool showString(std::ostream& aStream, std::string input)
        {
            aStream << input << std::endl;

            return true;
        }
    };

    class TableView : public View {
    public:
        virtual bool    show(std::ostream& astream)
        {
            return 1;
        }
        
        std::string getfieldname(Attribute *aattribute)
        {
           return aattribute->getName();
        }
        std::string gettype(Attribute* aattribute)
        {
            std::string type;
            std::map<DataType, std::string> keys{
                {DataType::int_type,"integer" },
                {DataType::float_type, "float"},
                {DataType::bool_type, "boolean" },
                {DataType::varchar_type, "varchar("},
                {DataType::no_type, "none"},
                {DataType::datetime_type, "date"}
            };
            
            if (keys.count(aattribute->getType()))
                type = keys[aattribute->getType()];

            if (aattribute->getType() == DataType::varchar_type)
                type += std::to_string(aattribute->getVarCharLength()) + ")";
            return type;
        }
        std::string getnull(Attribute* aattribute)
        {
            return (aattribute->getNullable()) ? "YES" : "NO";
        }
        std::string getprimarykey(Attribute* aattribute)
        {
            return (aattribute->getPrimaryKey()) ? "YES" : "NO";
        }
        std::string getautoincrement(Attribute* aattribute)
        {
            return (aattribute->getAutoIncrement()) ? " auto_increment" : "";
        }
        std::string getdefaultable(Attribute* aattribute)
        {
            return (aattribute->getDefaultable())? getdefaultval(aattribute) : "";
        }
        std::string getdefaultval(Attribute* aattribute)
        {    
            return std::visit(VisitPackage(), aattribute->getValue());
        }
        std::string getextra(Attribute* aattribute)
        {
            std::string result;
            if (aattribute->getPrimaryKey())
                result += " primary key ";

            return result + getautoincrement(aattribute);
        }
       
    };

    class DisplayTable : TableView {
    public: 
        //each constructor builds a "Table" which represents the columns and rows to be shown
        //@brief: show databseses
        DisplayTable(fileHandler* aFilehandler) {
            currentRowSize = 0;
            fileHandler aHandler;
            
            std::vector<std::string> dbNames = aFilehandler->getListOfDBFiles();
            addHeader("Databases");

            for (auto v : dbNames)
            {
                std::map<std::string, std::string> ColumnInfo;
                ColumnInfo.insert({ "Databases",v });
                Table.push_back(ColumnInfo);
            }
        }
        //@breif: show tables
        DisplayTable(std::vector<Schema*> schemas, std::string aName) {
            int numrows = 0;
            //output the names of the tables
            std::string name = "Tables in " + aName;
            addHeader(name);

            for (auto v : schemas)
            {
                std::map<std::string, std::string> ColumnInfo;
                ColumnInfo.insert({ name, v->getName() });
                Table.push_back(ColumnInfo);
            }

        }
        //@breif: describe database
        DisplayTable(std::vector<std::pair<uint32_t, BlockType>> metaIndexes) {
            static std::map<char, std::string> dType = {
                {'T', "meta"},
                {'D', "data"},
                {'E', "schema"},
            };
            currentRowSize = 0;
            addHeader("Block#");
            addHeader("Type");
            addHeader("Other");

            for (auto pair : metaIndexes)
            {
                if (static_cast<char>(pair.second) != 'F') {//not free block
                    std::map<std::string, std::string> ColumnInfo;
                    ColumnInfo.insert({ "Block#", std::to_string(pair.first) });
                    ColumnInfo.insert({ "Type", dType[static_cast<char>(pair.second)] });
                    ColumnInfo.insert({ "Other","" });
                    Table.push_back(ColumnInfo);
                }
                 
            }
        }
        ~DisplayTable() {}

        //@brief: describe table
        DisplayTable(Schema& aschema) {
            addHeader("Field");
            addHeader("Type");
            addHeader("Null");
            addHeader("Key");
            addHeader("Default");
            addHeader("Extra");
            currentRowSize = 0;
            for (auto v : aschema.getAttributes())
            {
                std::map<std::string, std::string> ColumnInfo;
                ColumnInfo.insert({ "Field", getfieldname(v) });
                ColumnInfo.insert({ "Type", gettype(v) });
                ColumnInfo.insert({ "Null", getnull(v) });
                ColumnInfo.insert({ "Key", getprimarykey(v) });
                ColumnInfo.insert({ "Default", getdefaultable(v) });
                ColumnInfo.insert({ "Extra", getextra(v) });
                Table.push_back(ColumnInfo);
            }
        }

        void addHeader(std::string header_text) {
            Headers.push_back(header_text);
        }

        std::vector<std::string> Headers;                      //i.e. column names
        std::vector<std::map<std::string, std::string>> Table; //container for rows/columns to for easier formatting


        //Show functions, formats Table struct into rows and columns with equal spacings
        bool    show(std::ostream& aStream)
        {
            int colNum = Headers.size();
            printHorizontalSeperator(aStream);
            printHeader(aStream);
            printHorizontalSeperator(aStream);

            for (int i = 0; i < Table.size(); i++)
            {
                printRow(aStream, i);
                printHorizontalSeperator(aStream);
            }
            aStream << currentRowSize << " rows in set" << std::endl;

            return 1;
        }
        //@brief: print header info
        void printHeader(std::ostream& aStream)
        {
            aStream << "|";
            for (int i = 0; i < Headers.size(); i++)
            {
                std::cout << " " << Headers.at(i) << std::setfill(' ') << std::setw(get_max_col_size(Table, Headers.at(i))- 1 - Headers.at(i).size()) << "|";
            }
            std::cout << std::endl;
        }
        //@brief: prints the seperators between rows
        void printHorizontalSeperator(std::ostream& aStream)
        {
            aStream << "|";
            for (int i = 0; i < Headers.size()-1; i++)
            {
                aStream << std::setfill('-') << std::setw(get_max_col_size(Table, Headers.at(i))) << "+";
            }

            aStream << std::setfill('-') << std::setw(get_max_col_size(Table, Headers.at(Headers.size()-1)));
            std::cout << "|" << std::endl;
        }
        //@brief: printrs "row" data, not as in Row class, but as in a row of the table to display
        void printRow(std::ostream& aStream, int rowNumber)
        {
            currentRowSize++;
            aStream << "|";
            for (int i = 0; i < Headers.size(); i++)
            {
                std::cout << " " << Table.at(rowNumber)[Headers.at(i)] << std::setfill(' ') << std::setw(get_max_col_size(Table, Headers.at(i))-1- Table.at(rowNumber)[Headers.at(i)].size()) << "|";
                
            }
            std::cout << std::endl;
        }
        //@brief: largest word length in column + 3 spaces for buffer
        int get_max_col_size(std::vector<std::map<std::string, std::string>> aTable,std::string fieldName)
        {
            int max = fieldName.size();
            for (auto v : aTable)
            {
                if (v[fieldName].size() > max)
                    max = v[fieldName].size();
            }

            return max+3;
        }
        

        int currentRowSize;
        int currentColumnSize;
    };


    
    

}

#endif /* View_h */
