# KernelMon
KernelMon is a virtualization-based driver monitoring infrastructure that draws inspiration from the Sysinternals suite and in particular, ProcMon.  

It basically hooks prominent KernelMode API's (for now Zw's) and transfers this information in the form of logs to a usermode UI.
KernelMon traces registry and fs reads and writes, process and thread operations.  
  
## Example:
![kernelmonitor](https://user-images.githubusercontent.com/60041914/115114151-6696c600-9f96-11eb-85cb-7650dc3664de.gif)
  
## Installation:
**Note: Make sure to run this inside a VM!**   
   
You might need to follow some of the instructions at: https://revers.engineering/day-0-virtual-environment-setup-scripts-and-windbg/ before running the app. (more specifically, disabling driver signing enforcement).  
  
Compile the driver(*KernelMonitor.sys*) and usermode app(*KernelMon.exe*).
  
Make a service for running the driver using the *sc* utility:
```console
C:\Users\User> sc create KernelMonitor type= kernel binPath= <path-to-driver>
```
  
Run it:
```console
C:\Users\User> sc start KernelMonitor
```
  
And then open the usermode app:
```console
C:\Users\User> KernelMon.exe
```
## How it works?
TBD
