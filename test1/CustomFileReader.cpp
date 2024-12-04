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
