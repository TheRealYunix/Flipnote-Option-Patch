#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

unsigned short calculateChecksum(const std::vector<unsigned char>& data)
{
	unsigned short chk = 0x0000;

	for (int i = 0; i < data.size(); i++)
	{
		chk = chk + (data[i] ^ i);
	}

	return chk;
}

bool isValidFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file)
	{
		std::cerr << "Error opening file." << std::endl;
		return false;
	}

	file.seekg(0, std::ifstream::end);
	std::streampos fileSize = file.tellg();

	if (fileSize != static_cast<std::streampos>(256))
	{
		std::cerr << "Invalid file size. The file must be 256 bytes in size." << std::endl;
		return false;
	}

	file.close();

	return true;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <option.bin>" << std::endl;
		return 1;
	}

	std::string filename(argv[1]);

	if (!isValidFile(filename))
	{
		return 1;
	}

	std::vector<unsigned char> data(256);

	std::ifstream file(filename, std::ios::binary);
	file.read(reinterpret_cast<char*>(data.data()), 256);
	file.close();

	//set the checksum bytes to 0x00
	data[0x18] = 0x00;
	data[0x19] = 0x00;

	//calculate the checksum
	unsigned short checksum = calculateChecksum(data);

	//store the checksum in the data
	data[0x18] = checksum & 0xFF;
	data[0x19] = (checksum >> 8) & 0xFF;

	//save the updated data to the file
	std::ofstream outFile(filename, std::ios::binary);
	outFile.write(reinterpret_cast<const char*>(data.data()), 256);
	outFile.close();

	std::cout << "The checksum has been successfully updated." << std::endl;

	return 0;
}