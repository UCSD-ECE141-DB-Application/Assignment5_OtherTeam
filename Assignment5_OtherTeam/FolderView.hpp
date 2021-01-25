#include <iostream>
#include "FolderReader.hpp"
#ifndef FolderView_h
#define FolderView_h

namespace ECE141 {
	class FolderView : public FolderListener {
	public:
		FolderView() {}

		bool operator()(const std::string& aName) {
			std::cout << aName << std::endl;
			return true;
		}
	};
}
#endif