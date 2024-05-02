# 1 - Getting Started

## Installation - VS Code

We recommend using [Visual Studio Code](https://code.visualstudio.com/) as a code editor. Click on the link and click the download button, as shown in the image below:
![vscode](../assets/1_getting_started/download-visual-studio-code.png)

## Installation - PROS

[PROS](https://pros.cs.purdue.edu/) provides the tooling needed to build and upload programs. It can be installed as a VSCode [Extension](https://marketplace.visualstudio.com/items?itemName=sigbots.pros). Simply click on the blocks at the bottom of the sidebar on the left in VSCode, search up `PROS`, and click `install`, as shown in the image below:

![pros](../assets/1_getting_started/install-pros.png)

After the extension has been downloaded, you should see a notification as seen in the image below. Click install, and wait for it to finish:

![pros-toolchain](../assets/1_getting_started/download-pros-toolchain.png)

After the toolchain has been installed, you can create a new PROS project. Select the PROS logo in the sidebar, click `Create Project`, select an empty folder for the project, give it a suitable name, e.g `1010N-Spin-Up`, select `v5`, and for kernel version select `3.8.3`. A notification in the bottom right will pop up notifying you that the project is being created. VSCode will open a new window as soon as it is ready. There will be a popup asking whether to "trust" the folder, click `yes`. You have now installed PROS and created your first project!

## Installation - clangd

[clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) is a linter and provides code completion, and is essential for any programmer. We recommend this over the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) extension as its much faster.

To install clangd, repeat the steps to install the PROS extension, but search up `clangd` instead of `PROS`. After it has been installed, open the `main.cpp` file in your project. A notification from clangd will appear in the bottom right stating that it needs to install. Click `install` and wait for it to finish installing. If the notification does not appear, ensure you have clangd installed and reopen `main.cpp`. You have now installed clangd!

## LemLib Installation

To install LemLib, first need to open the PROS integrated terminal. This can be done by clicking on the PROS icon on the left of the screen and clicking `integrated terminal`

Then, copy/paste the following commands into your command line. This will tell PROS where it can find LemLib, and to apply the latest version of LemLib to your project:
```
pros c add-depot LemLib https://raw.githubusercontent.com/LemLib/LemLib/depot/stable.json # adds LemLib's stable depot
pros c apply LemLib # applies latest stable version of LemLib
```

finally, put this at the top of your main.cpp file
```c++
#include "lemlib/api.hpp"
```

You've now installed LemLib, and you're ready to configure your robot!
