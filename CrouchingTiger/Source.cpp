#include "tiger.h"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("[CTHD]: Invalid arguments\n");
		printf("[CTHD]: Usage \"cthd ProcessToHide.exe\"");
		exit(EXIT_FAILURE);
	}
	system("cls");
	CrouchingTiger tiger(argv[1]);
	return 0;
}
