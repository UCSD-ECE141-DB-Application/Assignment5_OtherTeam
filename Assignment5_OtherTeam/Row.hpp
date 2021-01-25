//
//  Row.hpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "Storage.hpp"
#include "Value.hpp"


namespace ECE141 {


  class Row : public Storable {
  public:

    Row();
    Row(const Row &aRow);
    ~Row();
    Row& operator=(const Row &aRow);
    bool operator==(Row &aCopy) const;
    
    StatusResult encode(std::ostream& aWriter);
    StatusResult decode(std::istream& aReader);
    void setTableName(std::string name) { tablename = name; }
    BlockType getType() const {
        return BlockType::data_block;
    }
    uint32_t getId() { return id; }
    void setId(uint32_t aid) { id = aid; }
    void addKeyValue(std::string name, KeyValues* var);
    void addKeyValue(std::string fieldname, ValueType val);

    friend std::ostream& operator<<(std::ostream& os, const Row& aRow);
    friend std::istream& operator >> (std::istream& os, Row& aRow);


  protected:
    KeyValues data;  //you're free to change this if you like...
    std::string tablename;
    uint32_t id; //number in schema?
  };

}

#endif /* Row_hpp */
