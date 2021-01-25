//
//  Schema.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Schema_hpp
#define Schema_hpp

#include <stdio.h>
#include <vector>
#include "Attribute.hpp"
#include "Errors.hpp"
#include "Storage.hpp"
//#include "Row.hpp"
#include <optional>
#include "Value.hpp"

namespace ECE141 {

    struct Block;
    struct Expression;
    class  Database;
    class  Tokenizer;

    // class Storable;
     //using StringList = std::vector<std::string>;
     //using attribute_callback = std::function<bool(const Attribute &anAttribute)>;

    using AttributeList = std::vector<Attribute*>;


    struct Schema : Storable {
    public:
        Schema() : name(""), changed(0), incrementVal(0) {}
        Schema(const std::string aName);
        Schema(const Schema& aCopy);

        ~Schema();

        const std::string& getName() const { return name; }
        const AttributeList& getAttributes() const { return attributes; }
        uint32_t                                   getBlockNum() const { return blockNum; }
        bool                                       isChanged() { return changed; }
        void                                       setBlockNum(int num) { blockNum = num; }
        Schema& addAttribute(const Attribute& anAttribute);
        Attribute& getAttribute(std::string aName);
        bool                                       doesAttributeExist(const std::string& aName) const;
        void                                       setChanged(bool state) { changed = state; }
        std::optional<DataType>                    getAttributeDataType(const std::string& aName);
        bool                                       isDefaultableField(std::string field_name);

        //Value                 getDefaultValue(const Attribute &anAttribute) const;
        //StatusResult          validate(KeyValues &aList);
        StatusResult                              validate(KeyValues aKV);

        std::string           getPrimaryKeyName() const;
        uint32_t              getNextAutoIncrementValue();
        void resetIncrementVal() { incrementVal = 0; }


        StatusResult encode(std::ostream& aWriter); //save
        StatusResult decode(std::istream& aReader); //load
        BlockType getType() const {
            //make a block of the first dataa blocks
            return BlockType::entity_block;
        }

        StorageBlock metaBlock{ BlockType::entity_block };
        //friend class Database; //is this helpful?




        /*std::string   name;
        DataType      type;
        int           varCharLength;
        bool          bAutoIncrement;
        bool          bPrimaryKey;
        //time_t        timestamp;
        bool          nullable;
        std::variant<int, float> value;
        */
        void makeTestAttributeList() {
            AttributeInfo test1;
            test1.name = "testname";
            test1.varCharLength = 10;
            test1.bAutoIncrement = false;
            test1.bPrimaryKey = false;
            test1.nullable = true;
            test1.value = 3;
            test1.type = DataType::int_type;
            Attribute* testAttribute = new Attribute(test1);
            attributes.push_back(testAttribute);

            AttributeInfo test2;
            test2.name = "testname2";
            test2.varCharLength = 4;
            test2.bAutoIncrement = true;
            test2.bPrimaryKey = true;
            test2.nullable = false;
            test2.value = (float)3.03;
            test2.type = DataType::float_type;
            //Attribute testAttribute2(test2);
            Attribute* testAttribute2 = new Attribute(test2);
            attributes.push_back(testAttribute2);

            AttributeInfo test3;
            test3.name = "testname3";
            test3.varCharLength = 5;
            test3.bAutoIncrement = true;
            test3.bPrimaryKey = true;
            test3.nullable = false;
            test3.value = "data";
            test3.type = DataType::varchar_type;
            //Attribute testAttribute2(test2);
            Attribute* testAttribute3 = new Attribute(test3);
            attributes.push_back(testAttribute3);
        }
        bool isFieldName(std::string aField)
        {
            for (auto name : fieldNames)
            {
                if (name == aField)
                    return true;
            }

            return false;
        }
       

    protected:

        AttributeList   attributes;
        std::string     name;
        //uint32_t        blockNum;  //storage location.
        bool            changed;
        uint32_t             incrementVal;
        std::vector<std::string> fieldNames;
    };

}
#endif /* Schema_hpp */