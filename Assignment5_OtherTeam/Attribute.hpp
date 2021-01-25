//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include <ctime>
#include <variant>
#include <iostream>
#include "Value.hpp"
#include <variant>
#include "Errors.hpp"

namespace ECE141 {
      
  
  //input data to be passed into constructor
  typedef struct {
      std::string   name;
      DataType      type;
      int           varCharLength;
      bool          bAutoIncrement;
      bool          bPrimaryKey;
      //time_t        timestamp;
      bool          nullable;
      std::variant<int, float, std::string,bool> value;
      bool defaultable;
    
  } AttributeInfo;

  class Attribute {
  protected:
    std::string   name;
    DataType      type;

    int           varCharLength;
    //bool          bVarChar;
    bool          bAutoIncrement;
    bool          bPrimaryKey;
    time_t        timestamp;
    bool          nullable;
    bool          defaultable;
    std::variant<int, float, std::string, bool> value;
    

    //STUDENT: What other data should you save in each attribute?
    
  public:
          
    Attribute(DataType aType=DataType::no_type);
    Attribute(std::string aName, DataType aType /*maybe more?*/);
    Attribute(const Attribute &aCopy);
    Attribute(const AttributeInfo newInfo);

    ~Attribute() {}
    
    Attribute&    setName(std::string &aName);
    Attribute&    setType(DataType aType);

    bool          isValid(); //is this schema valid? Are all the attributes value?
    
    friend std::ostream& operator<<(std::ostream& os, const Attribute& dt);
    friend std::istream& operator >> (std::istream& os, Attribute& dt);

    //sets
    void                 setVarCharLength(int length);
    void                setAutoIncrement(bool state);
    void                setPrimaryKey(bool state);
    void                setTimeStamp();
    void                setNullable(bool state);
    void                setValue(std::variant<int, float, std::string,bool> aValue);
    void                setDefaultable(bool state);
    //gets
    const std::string&  getName() const {return name;}
    DataType            getType() const {return type;}
    int                 getVarCharLength() const { return varCharLength; }
    bool                getAutoIncrement() const { return bAutoIncrement; }
    bool                getPrimaryKey() const { return bPrimaryKey; }
    std::string         getUTCDateTime() const;
    bool                getNullable() const { return nullable; }

    

    StatusResult validate(KeyValues aPair);



    std::variant<int, float, std::string, bool> getValue() const { return value;  }
    bool getDefaultable() const { return defaultable; }
    
    time_t getCurrentLocalTime() { return time(0); }
   

    //STUDENT: are there other getter/setters to manage other attribute properties?
    
  };

 


}


#endif /* Attribute_hpp */
