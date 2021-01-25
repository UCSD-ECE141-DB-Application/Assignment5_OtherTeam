//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Tester_h
#define Tester_h

#include <string>
#include <vector>
#include <iostream>

#include "DatabaseProcessor.hpp"
#include "Tokenizer.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <algorithm>
#include <fstream>
#include "FolderReader.hpp"
#include "AppProcessor.hpp"
#include "CommandProcessor.hpp"
#include "DatabaseProcessor.hpp"
#include "FolderReader.hpp"
#include "FileChecker.hpp"
#include "Storage.hpp"
#include "FolderView.hpp"
#include <vector>

namespace ECE141 {

    class Tester {
    public:
        Tester(std::ostream& anOutput) : output(anOutput) {}
        bool runTests();
        bool doCreateTests();
        bool doDropTests();
        bool doUseTests();
        bool doDescribeTests();
        bool doShowTests();
        void fileTests();

        bool assertTrue(const char* aMessage, bool aValue);
        bool assertFalse(const char* aMessage, bool aValue);
        bool fail(const char* aMessage);
        bool succeed(const char* aMessage);

        bool checkExistence(FolderReader& r, std::string& aPath) {
            if (r.exists(aPath)) {
                return true;
            }
            return false;
        }

        std::ostream& getOutput() { return output; }

    private:
        std::ostream& output;



    };
}
#endif /* FolderReader_h */