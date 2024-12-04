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
	DcmDataset* dataSet2 = fileFormat.getDataset();
	DcmStack stack;

	while (dataSet->nextObject(stack, OFTrue).good()) {
		DcmObject* obj = stack.top();
		data.push_back(Tuple(
			{ obj->getTag(), (stack.card() / 2) - 1 },
			DcmVR(obj->getVR()).getVRName(),
			obj->getVM(),
			obj->getLength(),
			retrieveDescription(obj->getTag()),
			retrieveValue(dataSet2, obj->getTag())
		));
	}
	return data;
}

OFString CustomFileReader::retrieveDescription(DcmTag tag)
{
	OFString description = "";
	description = tag.getTagName();
	return description;
}

OFString CustomFileReader::retrieveValue(DcmDataset* dSet, DcmTag tag)
{
	OFString value(300, '\0');
	dSet->findAndGetOFString(tag, value);
	return value;
}