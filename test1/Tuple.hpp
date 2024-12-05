#pragma once

#include "dcmtk/dcmdata/dctk.h" 
#include <utility>

struct Tuple
{
	std::pair<DcmTag, int> tag;
	DcmVR vr;
	unsigned long vm;
	Uint32 length;
	OFString description;
	OFString value;

	Tuple()
	{
		tag = { DcmTag(), -1 };
		vr = DcmVR();
		vm = 0;
		length = 0;
		description = "none";
		value = "none";
	}

	Tuple(std::pair<DcmTag, int> tagg, DcmVR vrr, unsigned long vmm, Uint32 lengthh, OFString descriptionn, OFString valuee)
		: tag(tagg), vr(vrr), vm(vmm), length(lengthh), description(descriptionn), value(valuee)
	{
	};

	std::pair<DcmTag, int> getTag() const { return tag; };
	DcmVR getVr() const { return vr; };
	unsigned long getVm() const { return vm; };
	Uint32 getLength() const { return length; };
	OFString getDescription() const { return description; };
	OFString getValue() const { return value; };
};