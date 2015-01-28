#include "stdafx.h"
#include "server.h"


int _tmain(int argc, TCHAR* argv[])
{

	Server s = Server();
	s.run();

	WCHAR * waitval = new WCHAR[260];
	do{
		std::wcin >> waitval;

		s.sendName(waitval, wcslen(waitval));
	} while (waitval != 0);

	s.exit();
	return 0;
}