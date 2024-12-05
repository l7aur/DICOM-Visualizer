#pragma once

#include "dcmtk/config/osconfig.h" 
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmdata/dcistrmf.h"
#include "Tuple.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <utility>

class CustomFileReaderWriter {
public:
	CustomFileReaderWriter();
	~CustomFileReaderWriter();
	int fopen(std::string path);
	void printFile();
	std::vector<Tuple> getAll();
	void write(std::vector<Tuple> values, std::string path);
private:
	OFString retrieveDescription(DcmTag tag);
	OFString retrieveValue(DcmTag tag);

	DcmFileFormat fileFormat;
	OFCondition status;
	DcmDataset* dataSet;
};