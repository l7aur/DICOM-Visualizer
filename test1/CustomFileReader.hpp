#pragma once

#include "dcmtk/config/osconfig.h" 
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmdata/dcistrmf.h"
#include "Tuple.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <utility>

class CustomFileReader {
public:
	CustomFileReader();
	~CustomFileReader();
	int fopen(std::string path);
	void printFile();
	std::vector<Tuple> getAll();
private:
	OFString retrieveDescription(DcmTag tag);
	OFString retrieveValue(DcmDataset* dSet, DcmTag tag);

	DcmFileFormat fileFormat;
	OFCondition status;
};