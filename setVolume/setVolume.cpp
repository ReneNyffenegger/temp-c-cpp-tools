//
//  g++ setVolume.cpp -lole32 -osetVolume.exe
//
//
//  Copied and modified from Larry Osterman's original (https://docs.microsoft.com/en-us/archive/blogs/larryosterman/how-do-i-change-the-master-volume-in-windows-vista)
//

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
  IAudioEndpointVolume *endpointVolume = NULL;
  hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
  defaultDevice->Release();  defaultDevice = NULL;
// ------------------------- 
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
