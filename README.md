# Dragbase

Dragbase is a library, written in C, which adds enhanced drag-and-drop functionality to your Windows software - independently of the programming language your software is written in. It takes a few minutes to integrate Dragbase into your software. With dragbase, users can drag-and-drop the currently opened file to any other application. For instance, the file, respectively the icon, could be dropped in the Windows Explorer to a certain directory and the file would be saved in this directory. Or the file could be dropped in Microsoft Outlook and then a new email would be created with the file as attachment.

What is Notepad++ ?
===================

[![Join the disscussions at https://notepad-plus-plus.org/community/](https://notepad-plus-plus.org/assets/images/NppCommunityBadge.svg)](https://notepad-plus-plus.org/community/)
&nbsp;&nbsp;&nbsp;&nbsp;[![Join the chat at https://gitter.im/notepad-plus-plus/notepad-plus-plus](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/notepad-plus-plus/notepad-plus-plus?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Notepad++ is a free (free as in both "free speech" and "free beer") source code
editor and Notepad replacement that supports several programming languages and
natural languages. Running in the MS Windows environment, its use is governed by
GPL License.



# What's in the GitHub Folder

  - Installer/Dragbase Installer - The Dragbase Plugin installer for Windows
  - Notepad Non - This template is done for making plugin development as easy and simple as possible.
                  A simple plugin can be done through 4 steps, by editing only 2 files (PluginDefinition.h and PluginDefinition.cpp) :
                         1. Define your plugin name in "PluginDefinition.h"
                         2. Define your plugin commands number in "PluginDefinition.h"
                         3. Customize plugin commands names and associated function name (and the other stuff, optional) in                                         "PluginDefinition.cpp".
                         4. Define the associated functions
                         
                 For more information about plugin development, please check here:
                 http://sourceforge.net/apps/mediawiki/notepad-plus/index.php?title=Plugin_Development
                 
  - PowerEditor - ...
  - Scintilla - Scintilla can be built by itself.
                A C++ compiler is required. Visual Studio 2010 is the development system used for most development although TDM Mingw32                 4.7.1 is also supported.



## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.


### Prerequisites

There are three components that need to be downloaded onto the system for the Dragbase to work:

 - Notepad++ 32-bit
 - Visual Studio
 - Dragbase




### Installing

Dragbase is a plugin that can be integrated into Notepad++

  1. Download [Dragbase Repository](https://github.com/dyllew3/Software-Eng-Group28/tree/sharon-olorunns-patch-1)
  2. Download [Notepad++](https://notepad-plus-plus.org/download/v7.5.6.html)
  3. Extract the folder
  4. Open [Software-Eng-Group28/Installer/Dragbase Installer](https://github.com/dyllew3/Software-Eng-Group28/tree/master/Installer/Dragbase%20Installer)
  5. Double-click the Dragbase_Installer file and accept changes to your machine (Don't worry it's safe)
  6. Navigate to [Software-Eng-Group28/Notepad Non/bin/](https://github.com/dyllew3/Software-Eng-Group28/tree/new-dll-name/Notepad%20Non/bin) folder and copy DragbaseForNotepadpp.dll
  7. Open Program Files(x86) located on the drive
  8. Navigate to the Notepad++ folder and open it
  9. Open the plugin folder in the Notepad++ folder and paste the DragbaseForNotepadpp.dll file into the plugin folder.


## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management


## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 



See the [Notepad++ official site](https://notepad-plus-plus.org/) for more information.

[Notepad++ Contributors](https://notepad-plus-plus.org/contributors)
