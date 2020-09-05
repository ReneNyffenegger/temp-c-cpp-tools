//
//  g++ setVolume.cpp -lole32 -o setVolume.exe
//
//
//  Copied and modified from Larry Osterman's original (https://docs.microsoft.com/en-us/archive/blogs/larryosterman/how-do-i-change-the-master-volume-in-windows-vista)
//
//     setVolume -f 0.5     (0 .. 1)
//     setVolume -d …

#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

void Usage(){
   printf("Usage: \n");
   printf(" SetVolume [Reports the current volume]\n");
   printf(" SetVolume -d <new volume in decibels> [Sets the current default render device volume to the new volume]\n");
   printf(" SetVolume -f <new volume as an amplitude scalar> [Sets the current default render device volume to the new volume]\n");
}

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char* argv[]) {

   HRESULT hr;
   bool    decibels = false;
   bool    scalar  = false;
   double  newVolume;

   if (argc != 3 && argc != 1)  {
       Usage();
       return -1;
   }  

   if (argc == 3)  {

     if (argv[1][0] == '-') {
         if (argv[1][1] == 'f') {
            scalar = true;
         }
         else if (argv[1][1] == 'd')  {
            decibels = true;
         }
     }
     else {
       Usage();
       return -1;
     }     
  // newVolume = _tstof(argv[2]);
     newVolume = ::atof(argv[2]);
  }
  // -------------------------  

  CoInitialize(NULL);
  IMMDeviceEnumerator *deviceEnumerator = NULL;

//
// LPOLESTR lpolestr: used to show various GUIDS that are
// determined with __uuidof(…)
//
  LPOLESTR lpolestr;
  StringFromCLSID(__uuidof(MMDeviceEnumerator), &lpolestr);
  wprintf_s(L"MMDeviceEnumerator: %s\n", lpolestr);         // {BCDE0395-E52F-467C-8E3D-C4579291692E} -> Computer\HKEY_CLASSES_ROOT\CLSID\{BCDE0395-E52F-467C-8E3D-C4579291692E} -> MMDeviceEnumerator class -> %SystemRoot%\System32\MMDevApi.dll

  StringFromCLSID(__uuidof(IMMDeviceEnumerator), &lpolestr);
  wprintf_s(L"IMMDeviceEnumerator: %s\n", lpolestr);        // {A95664D2-9614-4F35-A746-DE8DB63617E6} -> Not found in the registry.

  hr = CoCreateInstance(
     __uuidof(MMDeviceEnumerator),
       NULL,
       CLSCTX_INPROC_SERVER,
     __uuidof(IMMDeviceEnumerator),
      (LPVOID *)&deviceEnumerator
  );

  std::cout << "hr = " << hr << std::endl;

  IMMDevice *defaultDevice = NULL;
  hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
  deviceEnumerator->Release();
  deviceEnumerator = NULL;


  StringFromCLSID(__uuidof(IAudioEndpointVolume), &lpolestr);
  wprintf_s(L"IAudioEndpointVolume: %s\n", lpolestr);        // {5CDF2C82-841E-4546-9722-0CF74078229A} -> Not found in the registry (but is defined in endpointvolume.h)

  IAudioEndpointVolume *endpointVolume = NULL;
  hr = defaultDevice->Activate(
     __uuidof(IAudioEndpointVolume),
       CLSCTX_INPROC_SERVER,
       NULL,
      (LPVOID *)
      &endpointVolume
  );

  defaultDevice->Release();
  defaultDevice = NULL;

  float currentVolume = 0;
  endpointVolume->GetMasterVolumeLevel(&currentVolume);
  printf("Current volume in dB is: %f\n", currentVolume);

  hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
  printf("Current volume as a scalar is: %f\n", currentVolume);

  if (decibels)  {
     hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
  }
  else if (scalar)  {
     std::cout << "setting scalar to " << newVolume << std::endl;
     hr = endpointVolume->SetMasterVolumeLevelScalar( (float) newVolume, NULL);
  }

  std::cout << "hr = " << hr << std::endl;

  endpointVolume->Release();
  CoUninitialize();
  return 0;

}
