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
		DcmObject* obj = stack.top();
		data.push_back(Tuple(
			{ obj->getTag(), (stack.card() / 2) - 1 },
			DcmVR(obj->getVR()).getVRName(),
			obj->getVM(),
			obj->getLengthField(),
			retrieveDescription(obj->getTag()),
			retrieveValue(dataSet, obj->getTag())
		));
	}
	return data;
}

OFString CustomFileReader::retrieveDescription(DcmTag tag)
{
	OFString description = "";
	description = tag.getTagName();
	return (description == "") ? "NO DESCRIPTION AVAILABLE" : description;
}

OFString CustomFileReader::retrieveValue(DcmDataset* dSet, DcmTag tag)
{
	OFString value = "";
	dSet->findAndGetOFString(tag, value);
	return (value == "") ? "NO VALUE AVAILABLE" : value;
}

// getLength sau getLengthField?