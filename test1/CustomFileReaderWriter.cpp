#include "CustomFileReaderWriter.hpp"

namespace {
	// IN BYTES
	const int TAG_SIZE = 4;
	const int LENGTH_FIELD_SIZE = 4;
	const int PRELUDE_SIZE = TAG_SIZE + LENGTH_FIELD_SIZE;
	const size_t VALUE_BUFFER_SIZE = 150;
}

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

/**
 * Delimitation of the last Item of a Sequence of Items, encapsulated in a Data Element of Value Representation SQ, 
 * shall be in one of the two following ways:
 *	> Explicit Length: The number of bytes (even) contained in the Data Element Value (following but not including the Value (Sequence) 
 * Length Field) is encoded as a 32-bit unsigned integer value (see Section 7.1). This length shall include the total length resulting 
 * from the sequence of zero or more items conveyed by this Data Element. This Data Element Length shall be equal to 00000000H if the 
 * sequence of Items contains zero Items.
 *	> Undefined Length: The Value (Sequence) Length Field shall contain a Value FFFFFFFFH to indicate a Sequence of Undefined Length. 
 * It shall be used in conjunction with a Sequence Delimitation Item. A Sequence Delimitation Item shall be included after the last 
 * Item in the sequence. Its Item Tag shall be (FFFE,E0DD) with a Value (Item) Length Field of 00000000H. No Value shall be present. 
 * A Sequence containing zero Items is encoded by a Sequence Delimitation Item only.
 * 
 * https://dicom.nema.org/medical/dicom/current/output/chtml/part05/sect_7.5.2.html
 * NOTES!
 *	> no nested sequences (may need to replace bool wih a stack to make it work)
 *	> only explicit length sequences are supported 
 *	> multiple items should be supported, but not tested
 */

std::vector<Tuple> CustomFileReaderWriter::getAll()
{
	std::vector<Tuple> data;
	DcmItem* item = nullptr;
	DcmStack stack;
	bool isSequence{ false };
	int itemIndex = -1;
	Uint32 seqLength = -1;
	Uint32 itemLength = -1;
	DcmTag seqTag{};
	while (1) {
		OFCondition cond = dataSet->nextObject(stack, OFTrue);
		if (!cond.good()) {
			std::cerr << "fetching data ended!\n";
			break;
		}

		DcmObject* obj = stack.top();
		Uint32 objLength{ obj->getLength() };
		DcmVR objVr = DcmVR(obj->getVR()).getVRName();
		int depthLevel = (stack.card() / 2) - 1;

		if (!strcmp(objVr.getVRName(), "UN")) //remove unknown things
			continue;

		if (!strcmp(DcmVR(obj->getVR()).getVRName(), "SQ")) {
			seqTag = obj->getTag();
			isSequence = true;
			seqLength = objLength - PRELUDE_SIZE;
			data.push_back(Tuple(
				{ obj->getTag(), depthLevel },
				objVr,
				obj->getVM(),
				objLength,
				retrieveDescription(obj->getTag()),
				""
			));
			continue;
		}
		if (!strcmp(DcmVR(obj->getVR()).getVRName(), "na")) { // item
			itemIndex++;
			seqLength -= (objLength + PRELUDE_SIZE);
			itemLength = objLength;
			data.push_back(Tuple(
				{ obj->getTag(), depthLevel },
				objVr,
				obj->getVM(),
				objLength,
				retrieveDescription(obj->getTag()),
				""
			));
			continue;
		}
		if (isSequence && seqLength <= 0 && itemLength <= 0) {
			if (seqLength < 0 || itemLength < 0) {
				std::cerr << "Something wrong with sequence size computation!\nInterrupting fetching data!\n";
				break;
			}
			isSequence = false;
			itemIndex = -1;
		}
		if (!isSequence)
			data.push_back(Tuple(
				{ obj->getTag(), depthLevel},
				objVr,
				obj->getVM(),
				objLength,
				retrieveDescription(obj->getTag()),
				retrieveValue(obj->getTag())
			));
		else
		{
			// undefined sequence length is not supported
			if (seqLength == 0xFFFFFFFF) {
				std::cerr << "Undefined length sequence encountered!\ninterrupting data fetching!\n";
				break;
			}
			itemLength -= (objLength + PRELUDE_SIZE);
			data.push_back(Tuple(
				{ obj->getTag(), (stack.card() / 2) - 1 },
				DcmVR(obj->getVR()).getVRName(),
				obj->getVM(),
				objLength,
				retrieveDescription(obj->getTag()),
				retrieveValue(seqTag, itemIndex, obj->getTag())
			));
		}
	}
	return data;
}

void CustomFileReaderWriter::writeValueAtTag(DcmTag tag, OFString newValue)
{
	DcmElement* element = nullptr;
	if (dataSet->findAndGetElement(tag, element).good() && element != nullptr)
	{
		element->putString(newValue.c_str());
		std::cout << "changed value into " << newValue << '\n';
	}
}

void CustomFileReaderWriter::writeValueAtTag(DcmTag sequenceTag, DcmTag targetTag, OFString newValue) 
{
	DcmSequenceOfItems* sequence = nullptr;
	if (dataSet->findAndGetSequence(sequenceTag, sequence).good() && sequence != nullptr) {
		for (unsigned long i = 0; i < sequence->card(); ++i) {
			DcmItem* item = sequence->getItem(i);
			if (item != nullptr) {
				DcmElement* element = nullptr;
				if (item->findAndGetElement(targetTag, element).good() && element != nullptr) {
					element->putString(newValue.c_str());
					std::cout << "changed value into " << newValue << '\n';
				}
				else
					std::cerr << "[SQ/WR]no element!\n";
			}
			else
				std::cerr << "[SQ/WR]no item!\n";
		}
	}
	else
		std::cerr << "[SQ/WR]not even trying!\n";
}

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
	OFString value(VALUE_BUFFER_SIZE, '\0');
	OFCondition cond = dataSet->findAndGetOFStringArray(tag, value);
	if (!cond.good())
		std::cerr << "failed to fetch value for " << tag.getTagName() << '\n';
	return value;
}

OFString CustomFileReaderWriter::retrieveValue(DcmTag sequenceTag, const unsigned int itemIndex, DcmTag targetTag)
{
	OFString value(VALUE_BUFFER_SIZE, '\0');
	DcmSequenceOfItems* sequence = nullptr;
	OFCondition cond = dataSet->findAndGetSequence(sequenceTag, sequence);
	if (cond.good() && sequence != nullptr) {
		DcmItem* item = sequence->getItem(itemIndex);
		if (item != nullptr)
		{
			DcmElement* element = nullptr;
			if (item->findAndGetElement(targetTag, element).good() && element != nullptr)
				element->getOFString(value, 0);
			else
				std::cerr << "[SQ]failed to fetch value for " << targetTag.getTagName() << '\n';
		}
		else
			std::cerr << "[SQ]no item\n";
	}
	else
		std::cerr << "[SQ]broke its legs!\n";
	return value;
}