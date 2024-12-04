#include "CustomFileReader.hpp"

CustomFileReader::CustomFileReader()
{
}

CustomFileReader::~CustomFileReader()
{
}

int CustomFileReader::fopen(std::string path)
{
	status = fileFormat.loadFile(path.c_str());
	return (status.good()) ? 0 : -1;
}

void CustomFileReader::printFile()
{
	fileFormat.print(COUT);
}

std::vector<Tuple> CustomFileReader::getAll()
{
	std::vector<Tuple> data;

	DcmItem* item = nullptr;
	DcmDataset* dataSet = fileFormat.getDataset();
	DcmStack stack;
	while (dataSet->nextObject(stack, OFTrue).good()) {
		data.push_back(Tuple(
			{ stack.top()->getTag(), OFString((stack.card() / 2) - 1, '>').size() },
			DcmVR(stack.top()->getVR()).getVRName()
		));
	}
	return data;
}
