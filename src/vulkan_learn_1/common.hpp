#pragma once

#include <iostream>
#include "renderer_helper.h"

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#endif

#ifdef __linux__ 
		// Get Linux File Path
#elif _WIN32
#include<Windows.h>
#endif

typedef uint32_t size;

constexpr int screen_width = 1280;
constexpr int screen_height = 720;

constexpr size		instance_count = 1 << 0;

#define MAX_TITLE_CHARS 128
static char title[MAX_TITLE_CHARS];

#define log(str) std::cout << str << std::endl

namespace files
{
	static std::string get_app_path()
	{
		char current_path[FILENAME_MAX];
#ifdef __linux__ 
		// Get Linux File Path
#elif _WIN32
		GetModuleFileNameA(0, current_path, sizeof(current_path));
#endif
		return std::string(current_path);
	}
}