#include <stdio.h>
#include "MovementUnlocker.h"
#include "utils/module.h"
#include <cstdint>
#include <sh_memory.h>
#include "iserver.h"
#ifdef _WIN32
#include <Windows.h>
#endif

MovementUnlocker g_MovementUnlocker;
IServerGameDLL* server = NULL;

#ifdef _WIN32
#define WIN_LINUX(win, linux) win
#else
#define WIN_LINUX(win, linux) linux
#endif

const char* pPatchSignature = WIN_LINUX(
	"F3 0F 5E D8 0F 28 C3 F3 0F 59 E3 F3 0F 59 47 ? F3 0F 59 5F ? F3 0F 11 47 ? F3 0F 11 67 ? F3 0F 11 5F ?",
	"F3 0F 5E C8 F3 0F 59 E1 F3 0F 59 D9 F3 0F 59 CA F3 41 0F 11 65 ? F3 41 0F 11 5D ? F3 41 0F 11 4D ?");

int iPatchLen = WIN_LINUX(36, 34);

PLUGIN_EXPOSE(MovementUnlocker, g_MovementUnlocker);
bool MovementUnlocker::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);

	CModule libserver(server);

	CMemory fn = libserver.FindPatternSIMD(pPatchSignature);
	if (!fn)
	{
		V_strncpy(error, "Could not find \'CGamemovement::WalkMove\' patch signature!", maxlen);
		ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", GetLogTag(), error);

		return false;
	}
	
	uintptr_t pPatchAddress = fn.GetPtr();
	// META_CONPRINTF( "pPatchAddress -> %p, start at -> %x\n", reinterpret_cast<void*>(pPatchAddress), *reinterpret_cast<int8_t*>(pPatchAddress) );

	SourceHook::SetMemAccess((void*)pPatchAddress, iPatchLen, SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);

	for (int i = 0; i < iPatchLen; i++)
	{
		*(unsigned char*)(pPatchAddress + i) = ((unsigned char*)"\x90")[0];
	}

	SourceHook::SetMemAccess((void*)pPatchAddress, iPatchLen, SH_MEM_READ | SH_MEM_EXEC);

	return true;
}

bool MovementUnlocker::Unload(char *error, size_t maxlen)
{
	return true;
}

void MovementUnlocker::AllPluginsLoaded()
{
}

bool MovementUnlocker::Pause(char *error, size_t maxlen)
{
	return true;
}

bool MovementUnlocker::Unpause(char *error, size_t maxlen)
{
	return true;
}

const char *MovementUnlocker::GetLicense()
{
	return "GNU General Public License v3.0";
}

const char *MovementUnlocker::GetVersion()
{
	return "1.0";
}

const char *MovementUnlocker::GetDate()
{
	return __DATE__;
}

const char *MovementUnlocker::GetLogTag()
{
	return "MOVEMENTUNLOCKER";
}

const char *MovementUnlocker::GetAuthor()
{
	return "Vauff";
}

const char *MovementUnlocker::GetDescription()
{
	return "CS2 MM:S port of Movement Unlocker, removes max speed limitation from players on the ground.";
}

const char *MovementUnlocker::GetName()
{
	return "Movement Unlocker";
}

const char *MovementUnlocker::GetURL()
{
	return "https://github.com/Source2ZE/MovementUnlocker";
}
