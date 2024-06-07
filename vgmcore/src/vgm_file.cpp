#include <vgm_file.h>
#include <zlib.h>

void vgm_file::init_stub() {
	/* read first 2 bytes to check if it's a compressed file */
	fseek(_file, 0, SEEK_SET); // seek back to beginning (is this even needed?)
	uint16_t sig; fread(&sig, 2, 1, _file);
	fseek(_file, 0, SEEK_SET); // seek back to beginning again
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	_compressed = (sig == 0x1F8B) || (sig == 0x7801) || (sig == 0x789C) || (sig == 0x78DA);
#else
	_compressed = (sig == 0x8B1F) || (sig == 0x0178) || (sig == 0x9C78) || (sig == 0xDA78);
#endif



	header.init(_file); // initialise header object
}

vgm_file::vgm_file(FILE* file, bool close_on_destruct) : _file(file), _close_on_destruct(close_on_destruct) {
	if(!_file) throw std::invalid_argument("file cannot be null");

	init_stub();
}

vgm_file::vgm_file(const char* path) : _close_on_destruct(true) { // since we're opening the file here, we need to close it when this class is destructed
	_file = fopen(path, "r");
	if(!_file) throw std::runtime_error("cannot open file");

	init_stub();
}

vgm_file::vgm_file() {

}

vgm_file::~vgm_file() {
	if(_close_on_destruct) fclose(_file); // close our file if specified
}