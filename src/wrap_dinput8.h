#pragma once

bool installWrappers(char * legit_dll_name);

typedef HRESULT(__stdcall* t_DirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter);

extern HINSTANCE	mHinst;
extern HINSTANCE	mHinstDLL;
extern UINT_PTR		mProcs[5];
extern LPCSTR		mImportNames[];

extern t_DirectInput8Create oDirectInput8Create;
HRESULT WINAPI DirectInput8Create_wrapper(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter);