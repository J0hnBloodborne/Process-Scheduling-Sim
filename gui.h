#pragma once
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>

void ShowMainWindow(HINSTANCE hInstance);
const char* GetSelectedFile();
int GetQuantumInput();