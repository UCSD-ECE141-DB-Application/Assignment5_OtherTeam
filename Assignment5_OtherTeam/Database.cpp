//
//  Database.cpp
//  Database1
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//


#include <sstream>
#include "Database.hpp"
#include "View.hpp"
#include "Storage.hpp"
#include "Schema.hpp"
#include "Tokenizer.hpp"


//this class represents the database object.
//This class should do actual database related work,
//we called upon by your db processor or commands

namespace ECE141 {
  
  Database::Database(const std::string aName, CreateNewStorage)
    : name(aName), storage(aName, CreateNewStorage{}) {
      //new database, create metadata
      //initDatabase();
   
     reSaveMetaData();
     
  }

  Database::Database(const std::string aName, OpenExistingStorage)
      : name(aName), storage(aName, OpenExistingStorage{}) {

      initDatabase(); 
  }

  Database::~Database() {
      for (auto v : schemaCache)
          delete v;
      for (auto v : rowCache)
          delete v;

      schemaCache.clear();
      rowCache.clear();
  }


  //brief: on "use database" loads metadata with schemaToC
  void Database::initDatabase() {
      clearCache();

      MetaDataStorable newRead;
      StorageBlock testBlock;
      
      storage.BlockToStorable(newRead, 0);
      schemaToc = newRead.schemaTOC;
      rowToc = newRead.rowTOC;

      fillCache();
  }
  /*
   * @breif: resaves the meta Block with updated schemaToC 
  */
  StatusResult Database::reSaveMetaData()
  {
      MetaDataStorable testReadStorable;
      testReadStorable.schemaTOC = schemaToc;
      testReadStorable.rowTOC = rowToc;
      StatusResult result = storage.save(testReadStorable, 0);

      return result;
  }
  

  //**************************************************************
  //        Table Main Actions
  //**************************************************************
  StatusResult Database::showTables()
  {
      DisplayTable tv(getSchemaCache(), getName());
      tv.show(std::cout);

      return StatusResult{};
  }
  StatusResult Database::createTable(Schema* aSchema)
  {
      //add a table to vector, mark that it needs to be saved
      if (!isSchemaInCache(aSchema->getName())) {
          //Schema* save = new Schema(aSchema);
          StatusResult result = storage.save(*aSchema);
          schemaCache.push_back(aSchema);
          addToSchemToc(aSchema);
          reSaveMetaData();
          return result;
      }

      return StatusResult{ECE141::Errors::tableExists};
  }
  StatusResult Database::describeTable(std::string name)
  {
      DisplayTable testView( *getSchemaFromName(name));
      testView.show(std::cout);
      return StatusResult{};//testView.describe(std::cout, *getSchemaFromName(name));
  }
  StatusResult Database::dropTable(std::string name)
  {
      deleteRows(getSchemaFromName(name));

      StatusResult result;
      //mark as free/remove from toc
      auto pos = schemaToc.find(name);
      if (pos != schemaToc.end())
      {
          storage.markBlockFree(schemaToc[name]);
          schemaToc.erase(name);
      }
      else
          result.code = ECE141::Errors::unknownTable;

      //remove from cache
      if (schemaCache.size()) {
          std::vector<Schema*>::iterator it;
          it = schemaCache.begin();
          int i = 0;
          while (it != schemaCache.end())
          {
              if ((*it)->getName() == name) {
                  schemaCache.erase(schemaCache.begin() + i);
                  break;
              }
              it++;
              i++;
          }
      }
      reSaveMetaData();
      return result;
  }
  void Database::describeDatabase(std::string& aName)
  {
      

      //tv.describeDatabase(std::cout);
      BlockListener* aListner = new BlockListener(BlockType::meta_block);
      storage.each(aListner);
      DisplayTable tv(aListner->indexes);
      tv.show(std::cout);

      delete aListner;
      
  }
 
  //*********************************
  //    Row Actions
  //********************************
  //@breif: adds a row to active schema
  StatusResult Database::insertRow(Row* aRow, Schema* aSchema)
  {
      //add a row to vector, mark that it needs to be saved
      if (isSchemaInCache(aSchema->getName())) {

          //Row added -> set tablename, id number
          aRow->setTableName(aSchema->getName());
          aRow->setId(aSchema->getNextAutoIncrementValue());
          aSchema->setChanged(true);
          //save 
          StatusResult result = storage.save(*aRow);
          
          //add to db cache
          rowCache.push_back(aRow);
          addToRowToc(aRow);

          //reset the schema increment (row num) to zero
          Schema* memSchem = getSchemaFromName(aSchema->getName());
          //memSchem->resetIncrementVal();
          storage.save(*memSchem, memSchem->blockNum);
          
          //resave new cache info
          reSaveMetaData();
          return result;
      }
  }
  //@brief: easy tester for inserting rows
  StatusResult Database::insertRowTester(Schema* aSchema)
  {
      ValueType variant_val;
      KeyValues string_var_map;
     

    //string, string
    //{"id", 9}
    //{"name, "foo"}
      variant_val = (uint32_t)12;

    //string bool
    //{"id", 12} 
    //{"status", false}
      ValueType variant_val2;
      KeyValues string_var_map2;
      

      variant_val2 = (bool)true;
     
      //vec_of_KeyVals.push_back(&string_var_map2);
    
      Row* testRow = new Row();
      testRow->setTableName(aSchema->getName());
      testRow->addKeyValue("test1", variant_val);
      testRow->addKeyValue("fieldname", variant_val2);

      insertRow(testRow, aSchema);
    
      return StatusResult{};
  }
  //@breif: deletes all records from active schema
  StatusResult Database::deleteRows(Schema* aSchema)
  {   
      StatusResult result;
      
      //rewrite the blocks to free
      for (auto v : rowToc)
      {
          storage.markBlockFree(v.second);
      }

      //clear cache, toc
      rowToc.clear();
      rowCache.clear();

      //reset the schema increment (row num) to zero
      Schema* memSchem = getSchemaFromName(aSchema->getName());
      memSchem->resetIncrementVal();

      //rewrite the schema
      storage.save(*memSchem, memSchem->blockNum);

      //rewrite the metaData
      reSaveMetaData();

      return result;
  }

  //*********************************
  // New Row Handling
  //*********************************
  StatusResult Database::addToRowToc(Row* aRow)
  {
      rowToc.insert({aRow->getId(), aRow->blockNum});

      return StatusResult{};
  }


  //*********************************
  //    Cache/ToC handling
  //*********************************
  //breif: clears cache containers, resets metadata
  void Database::clearCache()
  {
      for (auto v : schemaCache)
          delete v;
      for (auto v : rowCache)
          delete v;

      schemaCache.clear();
      //memBlocks.clear();
      rowCache.clear();
      schemaToc.clear();
      MetaDataStorable freshData;
      MetaData = freshData;
  }
  //breif: adds schema details to ToC
  void Database::addToSchemToc(Schema* aSchema)
  {
      schemaToc.insert({ aSchema->getName(),aSchema->getBlockNum() });
  }
  //brief: on "use db" after schemaToC is read, cache is generated
  //loads all blocks currently
  void Database::fillCache()
  {
      std::map<std::string, int>::iterator it;
      it = schemaToc.begin();
      while (it != schemaToc.end())
      {
          Schema* aSchema = new Schema();
          aSchema = getSchemaFromBlock(it->second);
          schemaCache.push_back(aSchema);
          it++;
      }
      std::map<uint32_t, int>::iterator it2;
      it2 = rowToc.begin();
      while (it2 != rowToc.end())
      {
          Row* aRow = new Row();
          aRow = getRowFromBlock(it2->second);
          rowCache.push_back(aRow);
          it2++;
      }

  }
  //breif: will get schema from cache or storage
  //returns nullptr if not found
  Schema* Database::getSchemaFromName(std::string aName)
  {
      Schema* aSchema();

      if (isSchemaInCache(aName))
      {
          return getSchemaFromCache(aName);
      }
      else 
      {
          auto pos = schemaToc.find(name);
          if (pos != schemaToc.end()) //using toc table to read block# from file
          {
              return getSchemaFromBlock(schemaToc[aName]);
          }
      }
      return nullptr;
  }
  //brief: check if schema is in cache
  bool Database::isSchemaInCache(std::string name)
  {
      for (auto v : schemaCache)
      {
          if (v->getName() == name)
              return true;
      }

      return false;
  }
  //breif: uses schemaToC to find schema in file
  Schema* Database::getSchemaFromCache(std::string name)
  {
      auto pos = schemaToc.find(name);
      if (pos != schemaToc.end())
      {
          for (auto v : schemaCache)
          {
              if (v->getName() == name)
                  return v;
          }
      }
      return nullptr;
  }
  //breif: get get schema from storage
  Schema* Database::getSchemaFromBlock(int blockNumber)
  {
      Schema* aSchema = new Schema;
      StorageBlock tmpBlock;

      storage.BlockToStorable(*aSchema, blockNumber);
      return aSchema;
  }
  //breif: get get row from storage
  Row* Database::getRowFromBlock(int blockNumber)
  {
      Row* aRow = new Row;
      StorageBlock tmpBlock;

      storage.BlockToStorable(*aRow, blockNumber);
      return aRow;
  }



  

}

