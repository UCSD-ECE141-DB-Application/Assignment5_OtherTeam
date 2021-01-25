//
//  Database.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include "Storage.hpp"
#include <map>
#include "View.hpp"
#include "Row.hpp"
#include "Value.hpp"


namespace ECE141 {
  
  class Database  {
  public:
    
    Database(const std::string aPath, CreateNewStorage);
    Database(const std::string aPath, OpenExistingStorage);
    ~Database();
    
    void describeDatabase(std::string& aName);
    void initDatabase();
    void clearCache();
    
    //cache handling
    void fillCache();

    //schema loading/cache
    Schema* getSchemaFromName(std::string name);
    Schema* getSchemaFromBlock(int blockNumber);
    StatusResult reSaveMetaData();
    bool isSchemaInCache(std::string name);
    Schema* getSchemaFromCache(std::string name);

    //table actions
    StatusResult showTables();
    StatusResult createTable(Schema* aSchema);
    StatusResult describeTable(std::string name);
    StatusResult dropTable(std::string name);

    //row actions
    StatusResult insertRow(Row* aRow, Schema* aSchema);
    StatusResult deleteRows(Schema* aSchema);

    //row cache
    StatusResult addToRowToc(Row* aRow);
    Row* getRowFromBlock(int blockNumber);

    //for testing
    StatusResult insertRowTester(Schema* aSchema);

    //
    std::vector<Schema*> getSchemaCache() { return schemaCache; }
    Storage&          getStorage() {return storage;}
    std::string&      getName() {return name;}
              
  protected:
    void addToSchemToc(Schema*);
    std::vector<StorageBlock> memBlocks;
    
    Schema* getActiveSchema() { return currSchema; }

    MetaDataStorable MetaData;

    std::map<std::string, int> schemaToc; //{name, blockNumber} 
    std::map<uint32_t, int> rowToc;

    std::vector<Schema*> schemaCache;
    std::vector<Row*>    rowCache;

    Schema* currSchema;

    std::string     name;
    Storage         storage;
  };

  
  
}

#endif /* Database_hpp */
