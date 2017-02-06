#include "windows.h"
#include <stdio.h>

#include "nes.h"

#ifdef _MSC_VER
// Disable warnings for fopen
#pragma warning(disable:4996)
#endif
void setupConsole() {
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    setupConsole();

	NES::NesControlDeck controlDeck;
	initNes("C:\\Users\\dc\\Desktop\\nestest\\cpu_reset\\registers.nes", controlDeck);

	nesLoop(controlDeck, 6200);

	int tmp;
	scanf("%d", &tmp);
	return 0;
} 