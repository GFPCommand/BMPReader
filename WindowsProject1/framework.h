// header.h: включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта
//

#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "targetver.h"

#include "ImageEdit.h"

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows
#include <windows.h>
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <algorithm>

#include <commdlg.h>
#include <fstream>
#include <regex>