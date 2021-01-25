//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderReader_h
#define FolderReader_h

#include <string>
#include <vector>
#include <filesystem>

namespace filesys = std::filesystem;

namespace ECE141 {

    class FolderListener {
    public:
        virtual bool operator()(const std::string& aName) = 0;
    };

    class FolderReader {
    public:
        FolderReader() {}

        FolderReader(const char* aPath) : path(aPath) {

        }

        virtual       ~FolderReader() {}

        virtual bool  exists(const std::string& aPath) {
            //can't use this to check if file exists, only directories (folders)!!!                 
            filesys::path thePath(aPath);
            if (filesys::is_directory(thePath)) {
                return true;
            }
            return false;
        }

        bool IsFile(const filesys::path& anEntry) const {
            if (is_regular_file(status(anEntry))) {
                return true;
            }
            return false;
        }

        std::string GetFile(std::string& wholePath) const {
            std::string file_name = filesys::path(wholePath).filename().string();

            return file_name;

        }
        virtual void  each(FolderListener& aListener, const std::string& anExtension) const {
            //STUDENT: iterate db's, pass the name of each to listener
            std::string check = path + const_cast<std::string&>(anExtension);

            if (filesys::is_directory(check)) {
                for (auto& ptr : filesys::directory_iterator(check)) {
                    std::string currPath = ptr.path().string();
                    if (IsFile(ptr)) {
                        std::string currFile = GetFile(currPath);
                        aListener(currFile);
                    }
                }
            }

            return;
        }

        std::vector<std::string> getDirectoryFiles(const std::string& aPath) const {
            std::vector<std::string> files;
            std::string check = path;

            if (filesys::is_directory(check)) {
                for (auto& ptr : filesys::directory_iterator(check)) {
                    std::string currPath = ptr.path().string();
                    if (IsFile(ptr)) {
                        std::string currFile = GetFile(currPath);
                        files.push_back(currFile);
                    }
                }
            }

            return files;
        }

        bool createFolder(std::string path)
        {
            std::clog << path << std::endl;
            return filesys::create_directory(path);
        }


        std::string path;

    };

}
#endif /* FolderReader_h */