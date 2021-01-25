#include <iostream>
#include <vector>
#include <algorithm>
#include "FolderReader.hpp"

#ifndef FileChecker_h
#define FileChecker_h

namespace ECE141 {
	class CheckFile : public FolderListener {
	public:
		CheckFile() {}
		~CheckFile() {}
		bool operator()(const std::string& existingFile) {
			files.push_back(existingFile);
			return true;
		}

		bool doesFileExist(const std::string& targetFile) {
			bool found = false;
			for (auto& fname : files) {
				if (targetFile == fname) {
					found = true;
					break;
				}
			}
			return found;
		}

		std::vector<std::string> files;
	};

}
#endif