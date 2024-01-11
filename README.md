# Kosuzu

Download and view Danbooru pools with their translation notes.

Can also function as a simple comic book reader.

![Main window](https://raw.githubusercontent.com/nostrenz/kosuzu/master/img/screenshot.png)

## Dependencies:

* **QuaZIP** by _stachenov_:
[GitHub](https://github.com/stachenov/quazip)

* **SingleApplication** by _itay-grudev_:
[GitHub](https://github.com/itay-grudev/SingleApplication)

* **QDarkStyleSheet** by _ColinDuquesnoy_:
[GitHub](https://github.com/ColinDuquesnoy/QDarkStyleSheet)

## Why

There are tons of translated doujinshi on Danbooru, but while it's easy to download them with tools like [Grabber](https://github.com/Bionus/imgbrd-grabber) there's unfortunatly no way to keep a local copy without losing the translation notes.

This tool can download an entire pool or just a single post from Danbooru and stores the result into a .ksz file which can be opened to view it with the translation notes on top of the images.

## How to use

1. Download the latest release [here](https://github.com/nostrenz/kosuzu/releases/latest)
2. Start the program and click on the "Downloader" button
3. In the opened window, enter a Danbooru pool URL in the text field then click the "Add" button
4. Once added, click the "**Start**" button and wait for the download to be completed
5. Double-click on the download row to start reading.

## KSZ files

Each downloaded pool or post is stored in a .ksz file.
Those are just ZIP archives containing the images and XML files for the translation notes.

Once downloaded, Kosuzu stores her books in a `downloads` folder in the program's directory. You can move them somewhere else and open them by drag&drop onto the main window or associating .ksz files with the Kosuzu executable.

Also, this program can open other zip-based comic book formats (like CBZ), and ksz files can be opened by any comic book reader supporting the CBZ format, but of courses translation notes won't be displayed by those.

## Building from source

1) After cloning the repo, install submodules with:

`git submodule update --init --recursive`

2) Depending on your system, you might need to replace `#include <zlib.h>` with `#include <QtZlib/zlib.h>` for all .cpp and .h files in `lib/QuaZIP/quazip` to use Qt's internal zlib library. Otherwise the project might file to compile.

3) Open the `Kosuzu.pro` file in QtCreator.

4) On the "Configure Project" page, select the "Desktop Qt 5.15.2 MinGW 64-bit" kit then hit the "Configure Project" button.

5) You should now be able to build and run the program _inside_ QtCreator.

Before running the program you'll also need to copy what's in the `dist/common/` folder next to the compiled executable.
Depending on your system you'll also need to copy what's in the `dist/Windows/` or `dist/Linux/` folder.

6) The compiled executable won't run by itself unless executed by Qt Creator as it misses a bunch of required libraries next to it.
To fix that, open a terminal and execute the `qtenv2.bat` script located in Qt's installation directory, for example:
```
"C:\path\to\Qt\5.15.2\mingw_64\bin\qtenv2.bat"
```

Then, using the same terminal instance, give the path to the compiled executable to the `windeployqt.exe` tool:
```
C:\path\to\Qt\5.15.2\mingw_64\bin\windeployqt.exe C:\build-Kosuzu-Desktop_Qt_5_15_2_MinGW_64_bit-Release\release\Kosuzu.exe
```

This should add a bunch of directories and .dll files next to the executable so it can run outside of the QtCreator environment.
