#pragma once

#include <vgm_header.h>

class vgm {
public:
	vgm();

	/// <summary>
	/// VGM header object (see <c>vgm_header.h</c>).
	/// </summary>
	vgm_header header;

	/// <summary>
	/// Flag for determining if the provided VGM data is compressed (i.e. VGZ file supplied).
	/// </summary>
	const bool& compressed = _compressed;

protected:
	bool _compressed = false;
};