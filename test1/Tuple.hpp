#pragma once

#include "dcmtk/dcmdata/dctk.h" 
#include <utility>

struct Tuple
{
	std::pair<DcmTag, int> tag;
	DcmVR vr;
	OFString vm;
	OFString length;
	OFString description;
	OFString value;

	Tuple()
	{
		tag = { DcmTag(), -1 };
		vr = DcmVR();
		vm = "none";
		length = "none";
		description = "none";
		value = "none";
	}

	Tuple(std::pair<DcmTag, int> tagg, DcmVR vrr, OFString vmm, OFString lengthh, OFString descriptionn, OFString valuee)
		: tag(tagg), vr(vrr), vm(vmm), length(lengthh), description(descriptionn), value(valuee)
	{
	};

	Tuple(std::pair<DcmTag, int> tagg, DcmVR vrr)
		: tag(tagg), vr(vrr)
	{
		vm = "none";
		length = "none";
		description = "none";
		value = "none";
	};

	std::pair<DcmTag, int> getTag() const { return tag; };
	DcmVR getVr() const { return vr; };
	OFString getVm() const { return vm; };
	OFString getLength() const { return length; };
	OFString getDescription() const { return description; };
	OFString getValue() const { return value; };
};