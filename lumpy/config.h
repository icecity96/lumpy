#pragma once

#ifndef __LUMPY_BUILD__
#define     lumpy_api
#else
#define     lumpy_api   __declspec(dllexport)
#endif

#define     lumpy_abi   extern "C" lumpy_api