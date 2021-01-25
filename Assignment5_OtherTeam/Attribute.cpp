//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Attribute.hpp"
#include <cstring>
#include <string>

namespace ECE141 {

    //STUDENT: Implement the attribute class here... 
    Attribute::Attribute(DataType aType)
    {
        bPrimaryKey = false;
        bAutoIncrement = false;
        nullable = true;
        defaultable = false;
        type = aType;
        name = "";
    }
    Attribute::Attribute(std::string aName, DataType aType /*maybe more?*/)
    {
        name = aName;
        type = aType;
    }
    Attribute::Attribute(AttributeInfo newInfo)
    {
        name = newInfo.name;
        type = newInfo.type;
        varCharLength =  newInfo.varCharLength;
        bAutoIncrement = newInfo.bAutoIncrement;
        bPrimaryKey = newInfo.bPrimaryKey;
        timestamp = getCurrentLocalTime();
        nullable = newInfo.nullable;
        value = newInfo.value;

    }
    Attribute::Attribute(const Attribute& aCopy)
    {
        name = aCopy.name;
        type = aCopy.type;
        varCharLength = aCopy.varCharLength;
        bAutoIncrement = aCopy.bAutoIncrement;
        bPrimaryKey = aCopy.bPrimaryKey;
        timestamp = aCopy.timestamp;
        nullable = aCopy.nullable;
        defaultable = aCopy.defaultable;
        value = aCopy.value;
    }
    Attribute& Attribute::setName(std::string& aName)
    {
        name = aName;
        return *this;
    }
    Attribute& Attribute::setType(DataType aType)
    {
        type = aType;
        return *this;
    }
    bool Attribute::isValid()
    {
        return true;
    }

    //sets
    void Attribute::setVarCharLength(int length)
    {
        varCharLength = length;
    }
    void Attribute::setAutoIncrement(bool state)
    {
        bAutoIncrement = state;
    }
    void Attribute::setPrimaryKey(bool state)
    {
        bPrimaryKey = state;
    }
    //sets timestamp based on local time
    void Attribute::setTimeStamp()
    {
        timestamp = getCurrentLocalTime();
    }
    void Attribute::setNullable(bool aState)
    {
        nullable = aState;
    }
    void Attribute::setValue(std::variant<int, float, std::string,bool> aValue)
    {
        //idk if this is safe
        value = aValue;
    }
    void Attribute::setDefaultable(bool state)
    {
        defaultable = state;
    }
    struct VisitPackage
    {
        std::string operator()(int val) { return std::to_string(val); }
        std::string operator()(float val) { return std::to_string(val); }
        std::string operator()(std::string val) { return val; }
    };
    std::ostream& operator<<(std::ostream& os, const Attribute& dt)
    {
        os << dt.name << " ";
        os << static_cast<char>(dt.type)<<" ";
        os << std::visit(VisitPackage(), dt.value) << " "; 
        os << dt.varCharLength << " ";
        os << dt.bAutoIncrement << " ";
        os << dt.bPrimaryKey << " ";
        os << dt.nullable << " ";
        os << dt.defaultable << " ";
        return os;
    }

    std::istream& operator>>(std::istream& os, Attribute& dt)
    {
        //still needs datatype and value
        char type;
        int intval;
        float flval;
        bool boolval;
        std::string stval;
        
        os >> dt.name;
        os >> type;
        DataType aType{ type };
        dt.type = aType;

        switch (aType)
        {
        case DataType::bool_type:
            os >> boolval;
            //dt.value = boolval;
            break;
        case DataType::datetime_type:
            break;
        case DataType::varchar_type:
            os >> stval;
            dt.value = stval;
            break;
        case DataType::int_type:
            os >> intval;
            dt.value = intval;
            break;
        case DataType::no_type:
            break;
        case DataType::float_type:
            os >> flval;
            dt.value = flval;
            break;
        default:
            break;
        }

        os >> dt.varCharLength;
        os >> dt.bAutoIncrement;
        os >> dt.bPrimaryKey;
        os >> dt.nullable;
        os >> dt.defaultable;
        return os;
    }
    std::string   Attribute::getUTCDateTime() const {
        time_t timestamp = time(0);
        char* dt = ctime(&timestamp);
        tm* gmtm = gmtime(&timestamp);
        dt = asctime(gmtm);
        return dt;
    }
    //validation
    struct VisitValidata
    {
        Attribute AttToVerify;

        //verify int
        bool operator()( uint32_t val) {
            if (AttToVerify.getType() != DataType::int_type)
                return false;

                return true;
        }
        //verify float
        bool operator()(float val) {
            if (AttToVerify.getType() != DataType::float_type)
                return false;

            return true;
        }
        //verify string
        bool operator()(std::string val) {
            if (AttToVerify.getType() != DataType::varchar_type)
                return false;

            if (val.size() > AttToVerify.getVarCharLength())
                return false;
            if ((val == "NULL") && (AttToVerify.getNullable() == false))
                return 0;

            return true;
        }
        //verify bool
        bool operator()(bool val) {
            if (AttToVerify.getType() != DataType::bool_type)
                return false;

            return true;
        }

    };
    StatusResult Attribute::validate(KeyValues aKV) {
        {
            VisitValidata validStruct;
            validStruct.AttToVerify = *this;

            //type verify
            if (aKV.count(getName())) {
                if (!std::visit(validStruct, aKV[getName()]))
                return StatusResult{ ECE141::Errors::rowNotValid };
            }

            return StatusResult{ ECE141::Errors::nameNotInMyAttribute }; //0 means it failed, or I do not have the name

        }



    }
}
