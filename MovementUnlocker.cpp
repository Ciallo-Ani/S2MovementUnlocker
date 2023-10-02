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

PLUGIN_EXPOSE(MovementUnlocker, g_MovementUnlocker);
bool MovementUnlocker::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);

	CModule libserver(server);

	CMemory fn = libserver.FindPatternSIMD(WIN_LINUX("76 ? F2 0F 10 57 ? F3 0F 10 47 ? 0F 28 CA F3 0F 59 C0", "E8 ? ? ? ? 48 89 C7 E8 ? ? ? ? 5D 48 8B 00 C3"));
	if (!fn)
	{
		V_strncpy(error, "Could not find \'CGamemovement::WalkMove\' patch signature!", maxlen);
		ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", GetLogTag(), error);

		return false;
	}
	
	uintptr_t pPatchAddress = fn.GetPtr();
	// META_CONPRINTF( "pPatchAddress -> %p, start value -> %x\n", reinterpret_cast<void*>(pPatchAddress), *reinterpret_cast<int8_t*>(pPatchAddress) );

	SourceHook::SetMemAccess((void*)pPatchAddress, 1, SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
	*(unsigned char*)(pPatchAddress) = ((unsigned char*)"\xEB")[0];
	SourceHook::SetMemAccess((void*)pPatchAddress, 1, SH_MEM_READ | SH_MEM_EXEC);

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
