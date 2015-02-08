#include <fstream>
#include <string>
using namespace std;

int getfilesize(ifstream&instream)
{
	size_t pos = instream.tellg();
	instream.seekg(0,std::ios::end);
	size_t size = instream.tellg();
	instream.seekg(pos,std::ios::beg);
	return size;
}

int main(int argc,char*argv[])
{
	string path = argv[0];
	size_t pos = path.rfind('\\');
	string prepath = path.substr(0,pos);
	ofstream out;
	ifstream in;
	string inf;
	string ouf;
	switch(argc)
	{
	case 2:
		{
			inf =string(argv[1]);
			ouf = inf + ".decode.tsc";
		}

		break;
	case 3:
		{
			inf = string(argv[1]);
			ouf = string(argv[2]);
		}
		break;
	default:
		fprintf(stderr,"woring input\n");
	}

	in.open(inf.c_str(),std::ios::binary);
	out.open(ouf.c_str(),std::ios::binary);

	if(!in.is_open() || !out.is_open())
	{
		in.close();
		out.close();
	}

	size_t size = getfilesize(in);
	char*buff = new char[size];
	in.read(buff,size);
	
	//
	int mid_pos = size/2;
	char key = buff[mid_pos];
	for(int i = 0;i < size;i++)
	{
		if(i != mid_pos)
		{
			buff[i] = buff[i] - key;
		}
	}
	//


	
	out.write(buff,size);
	out.close();
	in.close();
	delete []buff;


	return 0;
}