GetAllOpenFilesByProcessPid
===========================

History:
One day I required a tool that could emulate DB file corruption, hard drive or OS malfunction so the tested application will not be able to access the file locked by it. There are UI tools in the internet, but I was going to automate this destructive tests as well, so I wrote my own solution. Code is free to use.

Usage:

just run it from the command line.
To check list of files opened by process, simply do:
OpenedFilesByProcess.exe <ProcesID>

To start removing file handles from the process (emulating hard drive or OS malfunction add the file name (you may use not the full path) as second parameter and chose 'y' when it promts to remove necessary file).


Warning:
Code and binary is for Windows 8.1 and probably Windows 8. To use it on earlier versions you need to change if statement to look for ObjectType == 28.