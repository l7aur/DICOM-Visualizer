#include "CustomFileReaderWriter.hpp"

namespace {
	//SIZES IN BYTES
	const int TAG_SIZE = 4;
	const int LENGTH_FIELD_SIZE = 4;
	const int METADATA_SIZE = TAG_SIZE + LENGTH_FIELD_SIZE;
	const size_t VALUE_BUFFER_SIZE = 150;
	const size_t PIXEL_DATA_BUFFER_SIZE = 70'000;
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
	DcmStack stack;
	DcmItem* item{ nullptr };
	bool isSequence{ false };
	int itemIndex{ -1 };
	Uint32 seqLength{ 0 };
	Uint32 itemLength{ 0 };
	DcmTag seqTag{};
	OFCondition cond{};
	while ((cond = dataSet->nextObject(stack, OFTrue)).good()) {
		DcmObject* obj = stack.top();
		Uint32 objLength{ obj->getLength() };
		DcmVR objVr = DcmVR(obj->getVR()).getVRName();
		int depthLevel = (stack.card() / 2) - 1;

		// if (!strcmp(objVr.getVRName(), "UN")) // remove unknown things,
			// continue;

		if (itemIndex < 0)
			data.push_back(Tuple(
				{ obj->getTag(), depthLevel },
				objVr,
				obj->getVM(),
				objLength,
				retrieveDescription(obj->getTag()),
				retrieveValue(obj->getTag())
			));
		else
		{
			if (seqLength == 0xFFFF'FFFF) {
				// undefined sequence length is not supported
				std::cerr << "Undefined length sequence encountered!\ninterrupting data fetching!\n";
				break;
			}
			else {
				itemLength -= (objLength + METADATA_SIZE);
				data.push_back(Tuple(
					{ obj->getTag(), depthLevel },
					DcmVR(obj->getVR()).getVRName(),
					obj->getVM(),
					objLength,
					retrieveDescription(obj->getTag()),
					retrieveValue(seqTag, itemIndex, obj->getTag())
				));
			}
		}
		if (!strcmp(DcmVR(obj->getVR()).getVRName(), "SQ")) {
			seqTag = obj->getTag();
			isSequence = true;
			seqLength = objLength - METADATA_SIZE;
		}
		if (!strcmp(DcmVR(obj->getVR()).getVRName(), "na")) { // item
			itemIndex++;
			seqLength -= (objLength + METADATA_SIZE);
			itemLength = objLength;
		}
		if (isSequence && seqLength <= 0 && itemLength <= 0) {
			if (seqLength < 0 || itemLength < 0) {
				std::cerr << "Something wrong with sequence size computation!\nInterrupting fetching data!\n";
				break;
			}
			isSequence = false;
			itemIndex = -1;
		}
	}
	std::cout << "[STATUS] FETCHING DATA ENDED!\n";
	return data;
}

void CustomFileReaderWriter::writeValueAtTag(DcmTag tag, OFString newValue)
{
	DcmElement* element = nullptr;
	if (dataSet->findAndGetElement(tag, element).bad() || element == nullptr) {
		std::cerr << "[WR] something went wrong!\n";
		return;
	}
	if (putValueTemplateF(element, newValue) < 0)
		std::cerr << "[WR] value did not change!\n";
	else
		std::cout << "[STATUS] VALUE AT TAG " << tag << " HAS BEEN UPDATED!\n";
}

void CustomFileReaderWriter::writeValueAtTag(DcmTag sequenceTag, DcmTag targetTag, OFString newValue)
{
	DcmSequenceOfItems* sequence = nullptr;

	if (dataSet->findAndGetSequence(sequenceTag, sequence).bad() || sequence == nullptr) {
		std::cerr << "[SQ/WR] not even trying!\n";
		return;
	}
	for (unsigned long i = 0; i < sequence->card(); ++i) {
		DcmItem* item = sequence->getItem(i);
		if (item == nullptr) {
			std::cerr << "[SQ/WR] no item!\n";
			return;
		}
		DcmElement* element = nullptr;
		if (item->findAndGetElement(targetTag, element).bad() || element == nullptr) {
			std::cerr << "[SQ/WR] no element!\n";
			return;
		}
		if (putValueTemplateF(element, newValue) < 0)
			std::cerr << "changed nothing: " << newValue << "\n";
		else
			std::cout << "[STATUS] VALUE AT TAG " << targetTag << " HAS BEEN UPDATED!\n";
	}
}

void CustomFileReaderWriter::writeValueAtTag(const std::vector<DcmTag> sequenceTags, const std::vector<int> itemIndices, DcmTag targetTag)
{
	return;
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
	if (!strcmp(tag.getVR().getVRName(), "SQ") || !strcmp(tag.getVR().getVRName(), "na"))
		return "";

	/**
	 * to do list.
	 * fetch the pixel data
	 */

	if (tag == DCM_PixelData)
	{
		/*const Uint8* pData = nullptr;
		unsigned long length = 0;
		OFCondition cond = dataSet->findAndGetUint8Array(tag, pData, &length);
		if (cond.bad())
			return "erred";
		for (Uint8 i = 0; i < length; ++i)
			std::cout << *(pData + i) << "|\n";
		return std::to_string(length).c_str();*/

		DicomImage* image = new DicomImage
		("C:\\PXD\\Studii\\zzTest1^zzTest2 2024-12-02.12_47_55\\ST0\\SE6\\1.3.12.2.1107.5.1.4.65117.30000020080307014720000003216.dcm");
		if (image == nullptr) {
			std::cerr << "DcmImage constructor failed!\n";
			return "err";
		}
		if (image->getStatus() != EIS_Normal) {
			std::cerr << "Image status not normal!\n";
			return "err";
		}
		Uint8* pData = (Uint8*)(image->getOutputData(8));
		if (pData == nullptr) {
			std::cerr << "Empty pixel data\n";
			return "err";
		}
		for (unsigned long i = 0; i < 10; i++)
			std::cout << *(pData + i) << '\n';
		return "smth";
	}

	OFString value(VALUE_BUFFER_SIZE, '\0');
	OFCondition cond = dataSet->findAndGetOFStringArray(tag, value);
	if (!cond.good())
		std::cerr << "failed to fetch value for " << tag.getTagName() << '\n';
	return value;
}

OFString CustomFileReaderWriter::retrieveValue(DcmTag sequenceTag, const int itemIndex, DcmTag targetTag)
{
	DcmSequenceOfItems* sequence = nullptr;
	OFCondition cond = dataSet->findAndGetSequence(sequenceTag, sequence);
	if (cond.bad() || sequence == nullptr) {
		std::cerr << "[SQ] broke its legs! " << targetTag << ' ' << targetTag.getTagName() << "\n";
		return "err";
	}
	DcmItem* item = sequence->getItem(itemIndex);
	if (item == nullptr) {
		std::cerr << "[SQ] no item " << targetTag << ' ' << targetTag.getTagName() << "\n";
		return "err";
	}
	DcmElement* element = nullptr;
	if (item->findAndGetElement(targetTag, element).bad() || element == nullptr) {
		std::cerr << "[SQ] failed to fetch value for " << targetTag.getTagName() << '\n';
		return "err";
	}
	OFString value(VALUE_BUFFER_SIZE, '\0');
	element->getOFString(value, 0);
	return value;
}

OFString CustomFileReaderWriter::retrieveValue(const std::vector<DcmTag> sequenceTags, const std::vector<int> itemIndices, DcmTag targetTag)
{
	if (sequenceTags.size() != itemIndices.size()) {
		std::cerr << "[FW] vector size mismatch!\n";
		return "err";
	}
	size_t n = sequenceTags.size() - 1;
	DcmSequenceOfItems* sequence = nullptr;
	DcmItem* item = nullptr;
	DcmElement* element = nullptr;
	OFCondition cond = dataSet->findAndGetSequence(sequenceTags.at(n), sequence);
	for (int i = n; i >= 0; --i, cond = item->findAndGetSequence(sequenceTags.at(i), sequence)) {
		if (cond.bad() || sequence == nullptr) {
			std::cerr << "[FW] failed at fetching sequence!\n";
			return "err";
		}
		item = sequence->getItem(itemIndices.at(i));
		if (item == nullptr) {
			std::cerr << "[FW] failed at fetching item in sequence!\n";
			return "err";
		}
	}
	element = nullptr;
	if (item->findAndGetElement(targetTag, element).bad() || item == nullptr) {
		std::cerr << "[FW] failed at fetching the element from item in sequence!\n";
		return "err";
	}
	OFString value(VALUE_BUFFER_SIZE, '\0');
	element->getOFString(value, 0);
	return value;
}

int CustomFileReaderWriter::putValueTemplateF(DcmElement* element, OFString newValue)
{
	/*should be abstract factory pattern*/
	switch (element->getVR()) {
	case EVR_FL: {
		Float32 floatValue{ static_cast<Float32>(OFStandard::atof(newValue.c_str())) };
		OFCondition cond = element->putFloat32(floatValue);
		return cond.good() ? 0 : -1;
	}
	case EVR_FD: {
		Float64 doubleValue{ OFStandard::atof(newValue.c_str()) };
		OFCondition cond = element->putFloat64(doubleValue);
		return cond.good() ? 0 : -1;
	}
	default: {
		OFCondition cond = element->putString(newValue.c_str());
		if (cond.bad()) {
			std::cerr << "[UNDEFINED WR] default write method does not work!\n";
			return -1;
		}
		return 0;
	}
	}
	return -1;
}