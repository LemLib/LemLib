# 1 - Getting Started

### Installation - VS Code

We recommend using [Visual Studio Code](https://code.visualstudio.com/) to write code. It Download and run the installer.

### Installation - PROS

[PROS](https://pros.cs.purdue.edu/) provides the tooling needed to build and upload programs. It can be installed as a VSCode [Extension](https://marketplace.visualstudio.com/items?itemName=sigbots.pros).

### Installation - clangd

[clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) is a linter and provides code completion, and is essential for any programmer. We recommend this over the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) extension as its much faster.

### Project Setup

Time to create your first PROS project!

### LemLib Installation

To install LemLib, first need to open the PROS integrated terminal

Then, copy/paste the following commands into your command line. This will tell PROS where it can find LemLib, and to apply the latest version of LemLib to your project:
```
pros c add-depot LemLib https://raw.githubusercontent.com/LemLib/LemLib/depot/stable.json # adds LemLib's stable depot
pros c apply LemLib # applies latest stable version of LemLib
```

finally, put this at the top of your main.cpp file
```c++
#include "lemlib/api.hpp"
```

Now you are ready to config LemLib!