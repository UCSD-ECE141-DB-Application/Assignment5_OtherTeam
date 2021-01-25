//
//  Row.cpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Row.hpp"
#include <cstring>
#include <string>

namespace ECE141 {
  
  //STUDENT: You need to fully implement these methods...

  Row::Row() {}
  Row::Row(const Row &aRow) {}
  Row& Row::operator=(const Row &aRow) {return *this;}
  bool Row::operator==(Row &aCopy) const {return false;}
  Row::~Row() {}

  struct VisitPackage
  {
      //makes sure data type is written
      std::string operator()(uint32_t val) { return ("I " + std::to_string(val)); }
      std::string operator()(float val) { return ("F " + std::to_string(val)); }
      std::string operator()(std::string val) { return ("V " + (val)); }
      std::string operator()(bool val) { return ("B "+std::to_string(val)); }
  };

  StatusResult Row::encode(std::ostream& aWriter)
  {
      aWriter << tablename << " ";
      aWriter << data.size() << " ";
      KeyValues::iterator it = data.begin();
      
      for (auto v : data)
      {
        aWriter << v.first << " ";
        aWriter << std::visit(VisitPackage(), v.second) << " ";
      }
      
      return StatusResult{};
  }
  StatusResult Row::decode(std::istream& aReader)
  {
      int size;
      aReader >> tablename;
      aReader >> size;
      std::string tmpName;
      
      uint32_t tmpInt;
      float tmpFloat;
      std::string tempString;
      bool tmpBool;
      char dataType;

      for (int i = 0; i < size; i++)
      {
          //KeyValues* aPair = new KeyValues();
          ValueType* value = new ValueType();

          aReader >> tmpName; //field name?
          aReader >> dataType;
          DataType aType{ dataType };

          switch (aType)
          {
          case DataType::bool_type:
              aReader >> tmpBool;
              *value = tmpBool;
              break;
          case DataType::datetime_type:
              break;
          case DataType::varchar_type:
              aReader >> tempString;
              *value = tempString;
              break;
          case DataType::int_type:
              aReader >> tmpInt;
              *value = tmpInt;
              break;
          case DataType::no_type:
              break;
          case DataType::float_type:
              aReader >> tmpFloat;
              *value = tmpFloat;
              break;
          default:
              break;
          }

          //aPair->insert({ tmpName, value });
          data.insert({ tmpName, value });
      }
      return StatusResult{};
  }
  void Row::addKeyValue(std::string name, KeyValues* var)
  {
      data.insert({ name, var });
  }
  void Row::addKeyValue(std::string fieldname, ValueType val)
  {
      
  }
  std::ostream& operator<<(std::ostream& os, const Row& aRow)
  {
      return os;
  }
  std::istream& operator>>(std::istream& os, Row& aRow)
  {
      return os;
  }
}


