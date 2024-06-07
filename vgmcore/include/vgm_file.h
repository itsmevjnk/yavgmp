#pragma once

#include <vgm.h>
#include <stdio.h>

class vgm_file : public vgm {
public:
	vgm_file(FILE* file, bool close_on_destruct = true);

	vgm_file(const char* path);

	vgm_file();

	~vgm_file();
private:
	/// <summary>
	/// The file pointer of the opened VGM file.
	/// </summary>
	FILE* _file = NULL;

	/// <summary>
	/// Flag for specifying if <c>_file</c> is to be closed upon destruction of the class.
	/// </summary>
	bool _close_on_destruct = false;

	void init_stub();
};