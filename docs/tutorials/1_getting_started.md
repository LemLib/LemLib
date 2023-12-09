# 01 - Getting Started

## Introduction

Welcome to LemLib! This tutorial will walk you through the basics of LemLib and how to get everything set up.

## Installation - Development Environment

LemLib is a [PROS](https://pros.cs.purdue.edu) template, so you can use it with any text editor. However, we recommend using Visual Studio Code, as it has the best support for PROS.

To install Visual Studio Code, go to [https://code.visualstudio.com](https://code.visualstudio.com) and download the installer for your operating system.

<img src="./assets/1_getting_started/download-visual-studio-code.png"  width="800">

<br>
Once you have installed it, open it and install the PROS extension. You can do this by clicking typing `Ctrl+Shift+X` (`⌘+Shift+X` on Mac), searching for "PROS", and clicking the install button.
<br>
<img src="./assets/1_getting_started/install-pros.png" height=500 style="display: block;margin-left: auto;margin-right: auto;">
<br>

And finally, we can create a new project. Navigate to the new PROS tab on the left, and click the "Create New Project" button. Select a folder, a name, v5, and when asked to select a version, select "latest".

Now we can install LemLib!
<br>

## Installation

In Visual Studio Code, press the PROS button on the sidebar and click `Open Integrated Terminal`.

<br>
<img src="./assets/1_getting_started/integrated_terminal.png" height=800 style="display: block;margin-left: auto;margin-right: auto;">

Then copy and paste the following into the terminal, and hit enter.
```bash
curl -o LemLib@0.5.0.zip <placeholder_link>;
pros c fetch LemLib@0.5.0.zip;
pros c apply LemLib@0.5.0;
rm LemLib@0.5.0.zip;
```

Finally, put the following at the top of your `main.cpp` file:
```cpp
#include "lemlib/api.hpp"
```

You are now ready to start using LemLib!

[Next Tutorial](2_setting_up_the_chassis.md)
