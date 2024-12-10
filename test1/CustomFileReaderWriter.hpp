#pragma once

#include "dcmtk/config/osconfig.h" 
#include "dcmtk/dcmdata/dctk.h" 
#include "dcmtk/dcmdata/dcistrmf.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "Tuple.hpp"
#include <string>
#include <vector>
#include <deque>
#include <iostream>
#include <utility>

class CustomFileReaderWriter {
public:
	CustomFileReaderWriter();
	~CustomFileReaderWriter();
	int fopen(std::string path);
	std::vector<Tuple> getAll();
	void writeValueAtTag(DcmTag tag, OFString newValue);
	void writeValueAtTag(DcmTag sequenceTag, DcmTag targetTag, OFString newValue);
	void writeValueAtTag(const std::vector<DcmTag> sequenceTags, const std::vector<int> itemIndices, DcmTag targetTag);
	void saveOnDisk(std::string path);
private:
	OFString retrieveDescription(DcmTag tag);
	OFString retrieveValue(DcmTag tag);
	OFString retrieveValue(DcmTag sequenceTag, const int itemIndex, DcmTag targetTag);
	OFString retrieveValue(const std::vector<DcmTag> sequenceTags, const std::vector<int> itemIndices, DcmTag targetTag);
	int putValueTemplateF(DcmElement* element, OFString newValue);

	DcmFileFormat fileFormat;
	OFCondition status;
	DcmDataset* dataSet{ nullptr };
};