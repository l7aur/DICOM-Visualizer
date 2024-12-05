#include "CustomFileReaderWriter.hpp"

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

void CustomFileReaderWriter::writeValueAtTag(DcmTag tag, OFString newValue)
{
	DcmElement* element = nullptr;
	if (dataSet->findAndGetElement(tag, element).good()) //implies element is not nullptr?
	{
		element->putString(newValue.c_str());
		std::cout << "changed value into " << newValue << '\n';
	}
}

//void CustomFileReaderWriter::writeValueAtTag(DcmTag tag, void *newValue, const std::string& typee)
//{ /*just overload the method*/
//	DcmElement* element = nullptr;
//
//	if (dataSet->findAndGetElement(tag, element).good() && element != nullptr)
//	{
//		std::cout << "changed value into "; /*debug*/
//		if (typee == "string") {
//			const char* fVal = static_cast<const char*>(newValue);
//			element->putString(fVal);
//			std::cout << fVal << '\n'; /*debug*/
//		}
//		else if (typee == "int16") {
//			const int16_t fVal = *static_cast<const int16_t*>(newValue);
//			element->putSint16(fVal);
//			std::cout << fVal << '\n'; /*debug*/
//		}
//		else if (typee == "int32") {
//			const int32_t fVal = *static_cast<const int32_t*>(newValue);
//			element->putSint32(fVal);
//			std::cout << fVal << '\n'; /*debug*/
//		}
//		else if (typee == "float32") {
//			const float fVal = *static_cast<float*>(newValue);
//			element->putFloat32(fVal);
//			std::cout << fVal << '\n'; /*debug*/
//		}
//		else if (typee == "float64") {
//			const double fVal = *static_cast<double*>(newValue);
//			element->putFloat64(fVal);
//			std::cout << fVal << '\n'; /*debug*/
//		}
//		else {
//			std::cerr << "Type \'" << typee << "\' does not exist!\n";
//		}
//	}
//}

void CustomFileReaderWriter::saveOnDisk(std::string path)
{
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