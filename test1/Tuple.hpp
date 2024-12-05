#pragma once

#include "dcmtk/dcmdata/dctk.h" 
#include <utility>

struct Tuple
{
	const std::pair<DcmTag, int> tag;
	const DcmVR vr;
	const unsigned long vm;
	const Uint32 length;
	const OFString description;
	const OFString value;

	Tuple(std::pair<DcmTag, int> tagg, DcmVR vrr, unsigned long vmm, Uint32 lengthh, OFString descriptionn, OFString valuee)
		: tag(tagg), vr(vrr), vm(vmm), length(lengthh), description(descriptionn), value(valuee)
	{
	};

	const std::pair<DcmTag, int> getTag() const { return tag; };
	const DcmVR getVr() const { return vr; };
	const unsigned long getVm() const { return vm; };
	const Uint32 getLength() const { return length; };
	const OFString getDescription() const { return description; };
	const OFString getValue() const { return value; };
};