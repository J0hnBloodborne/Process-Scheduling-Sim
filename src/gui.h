#pragma once
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include "pclass.h"
#include "reader.h"
#include "algos.h"
#include "gl_graphs.h"

void ShowMainWindow(HINSTANCE hInstance);
const char* GetSelectedFile();
int GetQuantumInput();