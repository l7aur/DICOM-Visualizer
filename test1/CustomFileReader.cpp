#include "CustomFileReader.hpp"

CustomFileReaderWriter::CustomFileReaderWriter()
{
}

CustomFileReaderWriter::~CustomFileReaderWriter()
{
}

int CustomFileReaderWriter::fopen(std::string path)
{
	status = fileFormat.loadFile(path.c_str());
	if (!status.good())
		return -1;
	dataSet = fileFormat.getDataset();
	return 0;
}

void CustomFileReaderWriter::printFile()
{
	fileFormat.print(COUT);
}

std::vector<Tuple> CustomFileReaderWriter::getAll()
{
	std::vector<Tuple> data;

	DcmItem* item = nullptr;
	DcmStack stack;

	while (dataSet->nextObject(stack, OFTrue).good()) {
		DcmObject* obj = stack.top();
		data.push_back(Tuple(
			{ obj->getTag(), (stack.card() / 2) - 1 },
			DcmVR(obj->getVR()).getVRName(),
			obj->getVM(),
			obj->getLength(),
			retrieveDescription(obj->getTag()),
			retrieveValue(obj->getTag())
		));
	}
	return data;
}

void CustomFileReaderWriter::write(std::vector<Tuple> values, std::string path)
{
	for (auto& i : values) {
		DcmElement* element = new DcmLongString(i.tag.first);
		element->putString(i.value.c_str(), i.length);
		dataSet->insert(element);
	}
	dataSet->saveFile(path.c_str());
}

OFString CustomFileReaderWriter::retrieveDescription(DcmTag tag)
{
	OFString description = "";
	description = tag.getTagName();
	return description;
}

OFString CustomFileReaderWriter::retrieveValue(DcmTag tag)
{
	OFString value(150, '\0');
	dataSet->findAndGetOFString(tag, value);
	return value;
}