#pragma once

#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
//#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX


#define NOWINOFFSETS
#define NOOPENFILE
//#define NODRAWTEXT

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ�ļ����ų�����ʹ�õ�����

// Windows ͷ�ļ�
#include <windows.h>
#include <shellapi.h>

#include <d2d1.h>
#pragma comment(lib,"d2d1")

//#include <dwrite.h>
//#pragma comment(lib,"dwrite")

#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <chrono>
#include <thread>
#include <random>

using namespace std;
using namespace D2D1;
