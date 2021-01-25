//
//  Schema.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Schema.hpp"
#include "Storage.hpp"

namespace ECE141 {

    //STUDENT: Implement the Schema class here...
    Schema::Schema(const std::string aname) {
        name = aname;
        blockNum = 0;
        changed = true;
        incrementVal = 0;
    }
    Schema::Schema(const Schema& aCopy) {
        name = aCopy.name;
        blockNum = aCopy.blockNum;
        changed = aCopy.changed;
        incrementVal = aCopy.incrementVal;
        //make sure they copy
        for (auto v : aCopy.attributes)
            attributes.push_back(v);

    }
    Schema::~Schema() {
        for (auto v : attributes)
            delete v;
        attributes.clear();
    }
    Schema& Schema::addAttribute(const Attribute& anAttribute) {

        Attribute* v = new Attribute(anAttribute);
        attributes.push_back(v);
        fieldNames.push_back(anAttribute.getName());
        return *this;
    }

    Attribute& Schema::getAttribute(std::string aName) {
        for (auto v : attributes)
        {
            if (v->getName() == aName)
                return *v;
        }

        //To Do: add nullOpt case
        //return Attribute();
    }

    bool Schema::doesAttributeExist(const std::string& aName) const {
        for (auto v : attributes)
        {
            if (v->getName() == aName)
                return true;
        }
        return false;
    }

    std::optional<DataType> Schema::getAttributeDataType(const std::string& aName) {
        //hacky way to make sure we don't forget and accidentally try to use an empty attribute. need to change after std::optional is used instead
        std::optional<DataType> d = std::nullopt;
        if (doesAttributeExist(aName)) {
            Attribute att = getAttribute(aName);
            d = att.getType();
        }
        return d;
    }

    std::string Schema::getPrimaryKeyName() const {
        return "";
    }
    uint32_t Schema::getNextAutoIncrementValue() {
        incrementVal++;
        return incrementVal;
    }
    StatusResult Schema::encode(std::ostream& aWriter) //save
    {
        //makeTestAttributeList();

        aWriter << name << " ";
        aWriter << blockNum << " ";  //storage location.
        aWriter << changed << " ";
        aWriter << incrementVal << " ";
        aWriter << attributes.size() << " ";

        for (auto v : attributes)
            aWriter << *v;

        aWriter << fieldNames.size() << " ";

        for (auto fname : fieldNames)
            aWriter << fname;

        return StatusResult();
    }

    StatusResult Schema::decode(std::istream& aReader) //load
    {
        int size;
        aReader >> name;
        aReader >> blockNum;  //storage location.
        aReader >> changed;
        aReader >> incrementVal;
        aReader >> size;

        for (int i = 0; i < size; i++)
        {
            Attribute* test = new Attribute();
            aReader >> *test;
            attributes.push_back(test);
        }

        aReader >> size;

        for (int i = 0; i < size; i++) {
            std::string fieldName;
            aReader >> fieldName;
            fieldNames.push_back(fieldName);
        }

        aReader >> metaBlock.header.id;

        return StatusResult();
    }
    bool Schema::isDefaultableField(std::string field_name) {

        return getAttribute(field_name).getDefaultable();
    }

    StatusResult Schema::validate(KeyValues aKV) {
        StatusResult theResult = StatusResult(Errors::noError);
        std::map<std::string, bool> unused_field;

        for (auto names : fieldNames)
            unused_field.insert({ names, true });
        
        for (auto name_val : aKV) {
            StatusResult result = getAttribute(name_val.first).validate(aKV);
            if (result.code == ECE141::Errors::rowNotValid)
                return result;

            unused_field.erase(name_val.first);
            
        }
        for (auto field : unused_field)
        {
            if (!isDefaultableField(field.first))
                return StatusResult{ ECE141::Errors::rowNotValid };
        }

        return StatusResult{ ECE141::Errors::noError };
    }
}