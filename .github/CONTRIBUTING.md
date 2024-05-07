<!-- omit in toc -->
# Contributing

First off, thanks for taking the time to contribute! ❤️

All types of contributions are encouraged and valued. See the [Table of Contents](#table-of-contents) for different ways to help and details about how this project handles them. Please make sure to read the relevant section before making your contribution. It will make it a lot easier for us maintainers and smooth out the experience for all involved. The community looks forward to your contributions. 🎉

> And if you like the project, but just don't have time to contribute, that's fine. There are other easy ways to support the project and show your appreciation, which we would also be very happy about:
> - Star the project
> - Refer this project in your project's readme
> - Mention the project at local meetups with your fellow competitors
> - Mention the use of this project in judging presentations
> - Open a bug report or feature request [here](https://github.com/LemLib/LemLib/issues/new/choose)
> - Discuss and give feedback in our [Discord](https://discord.gg/pCHr7XZUTj)




<!-- omit in toc -->
## Table of Contents

- [🙋I Have a Question](#i-have-a-question)
- [🧑‍💻I Want To Contribute](#i-want-to-contribute)
  - [🐛Reporting Bugs](#reporting-bugs)
  - [✨Suggesting Enhancements](#suggesting-enhancements)
  - [🚀Your First Code Contribution](#your-first-code-contribution)
  - [📝Improving The Documentation](#improving-the-documentation)
- [💄Styleguides](#styleguides)
  - [💄C++ Coding Style](#c-coding-style)
  - [💄Commit Messages](#commit-messages)




## 🙋I Have a Question

> If you want to ask a question, we assume that you have read the available [Documentation](https://lemlib.readthedocs.io/en/v0.5.0/index.html).

Before you ask a question, it is best to search for existing [Issues](https://github.com/LemLib/LemLib/issues) that might help you. In case you have found a suitable issue and still need clarification, you can write your question in this issue. In addition, you can also search for existing questions in the [Vex Discord](https://discord.gg/VUStG8p), the [VEX Forum](https://www.vexforum.com/), or our [Discord](https://discord.gg/pCHr7XZUTj).

If you then still feel the need to ask a question and need clarification, we recommend the following:

- Open an [Issue](https://github.com/LemLib/LemLib/issues/new).
- Provide as much context as you can about what you're running into.
- Provide Kernel, CLI, and LemLib versions (e.g PROS 3.8.0, CLI 3.4.1, LemLib 0.4.5)
- Any additional information you think is relevant

We will then take care of the issue as soon as possible.




## 🧑‍💻I Want To Contribute


> ### Legal Notice <!-- omit in toc -->
> When contributing to this project, you must agree that you have authored 100% of the content, that you have the necessary rights to the content and that the content you contribute may be provided under the project license.


### 🐛Reporting Bugs

<!-- omit in toc -->
#### 🐛Before Submitting a Bug Report

A good bug report shouldn't leave others needing to chase you up for more information. Therefore, we ask you to investigate carefully, collect information and describe the issue in detail in your report. Please complete the following steps in advance to help us fix any potential bug as fast as possible.

- Make sure that you are using the latest version of LemLib, PROS, and the CLI.
- Determine if your bug is really a bug and not an error on your side e.g. incorrect chassis setup (Make sure that you have read the [documentation](https://lemlib.readthedocs.io/en/v0.5.0/index.html). If you are looking for support, you might want to check [this section](#i-have-a-question)).
- To see if other users have experienced (and potentially already solved) the same issue you are having, check if there is not already a bug report existing for your bug or error in the [bug tracker](https://github.com/LemLib/LemLib/issues?q=label%3Abug).
- Collect information about the bug:
  - Logger Messages
  - Kernel, CLI, and LemLib versions
  - Any additional templates being used and their versions
  - Can you reliably reproduce the issue?

<!-- omit in toc -->
#### 🐛How Do I Submit a Good Bug Report?

> You must never report security-related issues, vulnerabilities, or bugs including sensitive information to the issue tracker, or elsewhere in public. Instead sensitive bugs must be sent by email to <liamteale6@gmail.com>.
<!-- You may add a PGP key to allow the messages to be sent encrypted as well. -->

We use GitHub issues to track bugs and errors. If you run into an issue with the project:

- Open an [Issue](https://github.com/LemLib/LemLib/issues/new). Use the Bug Report issue template.
- Explain the behavior you would expect and the actual behavior.
- Provide the version of the PROS kernel, the PROS CLI, and LemLib
- List any additional templates and their versions
- Please provide as much context as possible and describe the *reproduction steps* that someone else can follow to recreate the issue on their own. This usually includes your code. For good bug reports you should isolate the problem and create a reduced test case.
- Provide the information you collected in the previous section.
- Provide any additional information you think is relevant, but does not fit previous sections.

Once it's filed:

- The project team will label the issue accordingly.
- A team member will try to reproduce the issue with your provided steps. If there are no reproduction steps or no obvious way to reproduce the issue, the team will ask you for those steps and mark the issue as `needs-repro`. Bugs with the `needs-repro` tag will not be addressed until they are reproduced.
- If the team is able to reproduce the issue, it will be marked `confirmed-bug`, as well as possibly other tags (such as `p: critical`), and the issue will be left to be [implemented by someone](#your-first-code-contribution).


### ✨Suggesting Enhancements

This section guides you through submitting an enhancement suggestion for LemLib, **including completely new features and minor improvements to existing functionality**. Following these guidelines will help maintainers and the community to understand your suggestion and find related suggestions.

<!-- omit in toc -->
#### ✨Before Submitting an Enhancement

- Make sure that you are using the latest version.
- Read the [documentation](https://lemlib.readthedocs.io/en/v0.5.0/index.html) carefully and find out if the functionality is already covered, maybe by an individual configuration.
- Perform a [search](https://github.com/LemLib/LemLib/issues) to see if the enhancement has already been suggested. If it has, add a comment to the existing issue instead of opening a new one.
- Find out whether your idea fits with the scope and aims of the project. It's up to you to make a strong case to convince the project's developers of the merits of this feature. Keep in mind that we want features that will be useful to the majority of our users and not just a small subset.

<!-- omit in toc -->
#### ✨How Do I Submit a Good Enhancement Suggestion?

Enhancement suggestions are tracked as [GitHub issues](https://github.com/LemLib/LemLib/issues). You can use the Feature Request issue template upon creation of a new issue.

- Use a **clear and descriptive title** for the issue to identify the suggestion.
- Provide a **step-by-step description of the suggested enhancement** in as many details as possible.
- **Describe the current behavior** so the project team can understand the difference between your suggestion and the current behavior.
- **Explain why this enhancement would be useful** to most LemLib users and/or maintainers.
- **(Optional) List possible implementations and alternatives** you have in mind.


### 🚀Your First Code Contribution

> [!NOTE]
> Using a development environment different from the one recommended below is fine, but you will have to adjust the steps yourself.

> [!NOTE]
> We do not recommend using the C/C++ extension, as its slow for linting and autocomplete.

> [!NOTE]
> At a minimum, we recommend the PROS and clangd extensions


We recommend using [Visual Studio Code](https://code.visualstudio.com/) as your IDE. It is free, (mostly) open-source, and cross-platform.
Here is a list of recommended extensions:
- [Better C++ Syntax](https://marketplace.visualstudio.com/items?itemName=jeff-hykin.better-cpp-syntax) better syntax highlighting for C++
- [Better Comments](https://marketplace.visualstudio.com/items?itemName=aaron-bond.better-comments) comment highlighting
- [clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd), super fast liner, code completion, and auto-formatter
- [Code Spell Checker](https://marketplace.visualstudio.com/items?itemName=streetsidesoftware.code-spell-checker) to check for spelling mistakes
- [CodeSnap](https://marketplace.visualstudio.com/items?itemName=adpyke.codesnap) take very nice screenshots of your code
- [Doxygen Documentation Generator](https://marketplace.visualstudio.com/items?itemName=cschlosser.doxdocgen) automatically generates Doxygen comments
- [Error Lens](https://marketplace.visualstudio.com/items?itemName=usernamehw.errorlens) displays errors, warning, and info nicely
- [GitHub Copilot](https://marketplace.visualstudio.com/items?itemName=GitHub.copilot) AI autocomplete useful for repetitive and trivial tasks
- [GitLens](https://marketplace.visualstudio.com/items?itemName=eamodio.gitlens) useful git tools
- [Material Icon Theme](https://marketplace.visualstudio.com/items?itemName=PKief.material-icon-theme) for improved visual grepping of files
- [MyST-Markdown](https://marketplace.visualstudio.com/items?itemName=ExecutableBookProject.myst-highlight) for previewing markdown files with MyST extensions
- [PROS](https://marketplace.visualstudio.com/items?itemName=sigbots.pros) to build and upload your code

> If asked to disable the IntelliSense in favor of clangd, do so.

In order to build the project, you need to use PROS
- Generate compile commands: `pros build-compile-commands`
- Build the project: `pros build`
- Create the template zip: `pros build template`

You can apply the kernel to your project with the following commands
- Fetch the kernel: `pros c fetch LemLib@a.b.c-d.zip`
- Apply the kernel: `pros c apply LemLib@a.b.c-d`
- Remove the template: `pros c remove LemLib@a.b.c-d`

In order to contribute to LemLib, you will need to [fork](https://help.github.com/en/github/getting-started-with-github/fork-a-repo) the repository and clone it to your local machine. You can then [commit](#commit-messages) your changes to your fork. Once you are done, you can [create a pull request](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request) to the master branch. You can use the [Pull Request Template](https://github.com/LemLib/LemLib/blob/master/.github/PULL_REQUEST_TEMPLATE.md) to structure your pull request.


### 📝Improving The Documentation
We use a combination of [Doxygen](http://www.doxygen.nl/), [Breathe](https://www.breathe-doc.org/), and [Sphinx](https://www.sphinx-doc.org/en/master/) to generate our documentation. The documentation is hosted on [Read The Docs](https://docs.readthedocs.io/en/stable/), which gives us both free hosting, and automatic versioning of the documentation. It automatically deploys after every commit.

You can improve the documentation by:
- Editing the [README](../README.md)
- Adding/Editing comments in the source code
- Editing the [Doxygen configuration file](https://github.com/LemLib/LemLib/blob/master/docs/Doxyfile)
- Adding/Editing [Tutorials](https://github.com/LemLib/LemLib/tree/master/docs/tutorials)
- Opening an [Issue](https://github.com/LemLib/LemLib/issues) to suggest a change or to report a bug

Changes should be requested via a [Pull Request](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request) to the LemLib repository. You can use the [Pull Request Template](https://github.com/LemLib/LemLib/blob/master/.github/PULL_REQUEST_TEMPLATE.md) to structure your pull request. Additionally, you should verify that the documentation builds correctly on your machine.

You can do so by installing Doxygen, and all of the python dependencies listed in `docs/requirements.txt`. Once you've done that, run the following commands to render the documentation:

```sh
cd docs
doxygen
make html
```
Then you can view the documentation by pointing your browser at the `./_build/html/index.html` file.

## 💄Styleguides


### 💄C++ Coding Style

We use our own style guide for LemLib. Here are the most important rules:
 - Use `lowerCamelCase` for variables and functions
 - Use `UpperCamelCase` for classes and structs
 - Use `UPPER_SNAKE_CASE` for macros
 - Braces should always be on the same line as the statement
 - Pointers and references are always to the left
 - Max column width is 120 characters
 - Operators should be surrounded by spaces
 - Commas should be followed by a space
 - Indents are 4 spaces

For further information, check out [.clang-format](../.clang-format).

We use the [clang-format](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format) extension to format our code. You can use the `.clang-format` file in the root directory of the project to configure your IDE to use the same formatting rules as we do. You can also use the `clang-format` command line tool to format your code. You can find more information about the tool [here](https://clang.llvm.org/docs/ClangFormat.html).


### 💄Commit Messages

Commit titles should be short and descriptive. They should be written in the imperative mood, i.e. as if you were commanding someone. They should not end with a period. If you are fixing an issue, you should include the issue number in the commit title. Additional information can be provided in the commit body. For example:

```
Add support for Differential Drivetrains (fixes #42)

This commit adds support for Differential Drivetrains. It also adds a new class called DifferentialDrivetrain, which is a subclass of Drivetrain.
```




<!-- omit in toc -->
## Attribution
This guide is based on the **contributing-gen**. [Make your own](https://github.com/bttger/contributing-gen)!
