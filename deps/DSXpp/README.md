![DSX++-banner](https://github.com/user-attachments/assets/2aeab764-279c-45e1-8f7b-984d3d9b7929)

❯ DSX++ is a C++ Port of the popular DualSenseX (DSX) API that lets you interract
with the DSX app and tweak advanced features of your DualSense
controller, i.e., Adaptive Triggers and Haptic Feedback on PC!
    </em>
</p>
<p align="center">
	<!-- local repository, no metadata badges. --></p>
<p align="center">Built with the tools and technologies:</p>
<p align="center">
	<img src="https://img.shields.io/badge/-CMake-darkslateblue?logo=cmake" "alt"="CMake">
    <img src="https://img.shields.io/badge/-C++-magenta?logo=cplusplus" "alt"="C++">
    <img src="https://img.shields.io/badge/Vim-%2311AB00.svg?logo=vim&logoColor=white" alt="Vim">
</p>
<br>

##  Table of Contents

- [ Overview](#overview)
- [ Features](#features)
- [ Getting Started](#getting-started)
  - [ Prerequisites](#prerequisites)
  - [ Installation](#installation)
  - [ Usage](#usage)
  - [ Testing](#testing)
- [ Project Roadmap](#project-roadmap)
- [ Contributing](#contributing)
- [ License](#license)
- [ Acknowledgments](#acknowledgments)

---

##  Overview


[DSX]: https://store.steampowered.com/app/1812620/DSX/
[DualSenseX]: https://github.com/Paliverse/DualSenseX
[DSX Github]: https://github.com/Paliverse/DSX
[DSX Mod System]: https://github.com/Paliverse/DSX/tree/main/Mod%20System%20(DSX%20v3)#dsx-mod-system

DSX++ is a C++ framework that lets you interact with the [DSX] app and alter
your DualSense controller advanced features on the fly. It is mostly meant to
be used by Game Developers or modders that want to bump up the immersion of
their games.

This framework is a Port of the original DSX framework implemented in C#
language. The official project and framework can be found in the following
links:
* [DualSenseX]
* [DSX Github]


---

##  Features

DSX++ supports all the features implemented by the latest version of DSX Mod
System (DSX v3.1+). It also has support of the Legacy v2 Mod System. Here's a
brief summary of the supported features:
* Tweaking the Adaptive Triggers
* Changing the Lightbar LED
* Changing the Player LED
* Changing the Mic LED
* Legacy Addaptive Triggers functions

For more details of the above features, you can have a look on the official
[DSX Mod System] page or visit the store page of [DSX] on Steam.


---
##  Getting Started

###  Prerequisites


> [!IMPORTANT]
> DSX++ framework interacts with the DSX app, so having the app isntalled is
a requirements in order to use the framework. The app can be purchased on
Steam: **[DSX]**.

*The following can be also installed via the DSX app during setup:*

* Very important and required get ViGEmBus Driver:
    * Download the latest `ViGEmBus` drivers from the [ViGEmBus release page](https://github.com/ViGEm/ViGEmBus/releases/latest).
    * To install, double-click `ViGEmBus_Setup_{version}.exe` and follow the instructions presented.

* HidHide Driver (OPTIONAL):
    * Download the latest `HidHide` drivers from the [HidHide release page](https://github.com/ViGEm/HidHide/releases/latest).
    * To install, double-click `HidHideMSI.msi` and follow the instructions presented.
    * **Make sure you reboot your device**

> [!NOTE]
> You'll also need a DualSense controller :video_game:

Once that's  done, the DualSense Controller should auto connect to the App.

In order to use the API, make sure to switch to either DSX v2:
* There should be a button on the top left of the app

or DSX v3.1 (BETA at the time of writing) — **recommended**
* Follow the instructions here: [DSX — How to Switch Branch on Steam](https://steamcommunity.com/sharedfiles/filedetails/?id=3330635677)

*DSX v3.0 is not supported!*

Before getting started with DSX++, ensure your runtime environment meets the following requirements:

- **Programming Language:** CPP
- **Package Manager:** Cmake


###  Installation

Install DSX++ using one of the following methods:

**Build from source:**

1. Clone the DSX++ repository:
```sh
❯ git clone https://github.com/tpetsas/DSXpp.git
```

2. Navigate to the project directory:
```sh
❯ cd DSXpp
```

3. Install the project dependencies:

**Using `cmake`** &nbsp; [<img align="center" src="https://img.shields.io/badge/C++-00599C.svg?style={badge_style}&logo=c%2B%2B&logoColor=white" />](https://isocpp.org/)

```sh
❯ mkdir build && cd build
❯ cmake .. && cmake --build .
```

**Download the latest release:**

... text here ...



###  Usage

[FTXUI]()
**Addaptive Triggers CLI Testing app:**

Open a PowerShell session and run the DSXTest.exe using the following command:

```sh
❯ .\Debug\DSXTest.exe
```

A terminal interface will appear looking like this:

![alt text](imgs/dsxtest-0.png?raw=true)

You can use either the mouse or the keyboard to navigate through this
[FTXUI]-based terminal user interface and choose the addaptive trigger
configuration you want to send down to your controller.

It is recommended to use the keyboard for fine-grain adjustments, e.g., when
setting a value using a slider.

![alt text](imgs/dsxtest.png?raw=true)

The :rocket:**Send** button will send the chosen configuration to the
controller, while the :boom:**Reset** will wipe it from the client. If you
want to reset the state to the controller to what it was before using the
`DSXTest.exe`, use the combination of :boom:**Reset** & :rocket:**Send**.


**Getting started with sample code:**

There is a DSX sample file you can use as a starter when creating your
own app: `src\DSXSample\sample.cpp`

``` C++
#include <iostream>
#include "DSX++.h"

int main (void)
{
    if ( DSX::init() != DSX::Success ) {
        std::cerr << "* DSX++ client failed to initialize!" << std::endl;
        return -1;
    }
    std::cout << "* DSX++ client initialized successfully!" << std::endl;

#ifdef DSX_V3
    DSX::setLeftTrigger(Weapon, {2, 6, 8});
    DSX::setRightTrigger(Vibration, {2, 8, 20});
#else
    DSX::setLeftTrigger(GameCube);
    DSX::setRightTrigger(Hard);
#endif

    if (DSX::sendPayload() != DSX::Success) {
        std::cerr << "* DSX++ client failed to send data!" << std::endl;
        return -2;
    }
    std::cout << "* DSX++ client sent the data successfully!" << std::endl;

    if ( DSX::terminate() != DSX::Success ) {
        std::cerr << "* DSX++ client failed to terminate!" << std::endl;
        return -3;
    }
    std::cout << "* DSX++ client terminated successfully!" << std::endl;

    return 0;
}
```

You can run the `DSXSample.exe` on terminal similarly to the DSXTest.exe:
```
❯ .\Debug\DSXSample.exe
```

![alt text](imgs/dsxsample.png?raw=true)

---
##  Project Roadmap

[comment]: <> (- [X] **`Task 1`**: <strike>Implement feature one.</strike>)
- [ ] **`Task 1`** Add a Wiki page describing the API.

---

##  Contributing

- **💬 [Join the Discussions](https://github.com/tpetsas/DSXpp/discussions)**: Share your insights, provide feedback, or ask questions.
- **🐛 [Report Issues](https://github.com/tpetsas/DSXpp/issues)**: Submit bugs found or log feature requests for the `DSX++` project.
- **💡 [Submit Pull Requests](https://github.com/tpetsas/DSXpp/blob/main/CONTRIBUTING.md)**: Review open PRs, and submit your own PRs.

<details closed>
<summary>Contributing Guidelines</summary>

1. **Fork the Repository**: Start by forking the project repository to your LOCAL account.
2. **Clone Locally**: Clone the forked repository to your local machine using a git client.
   ```sh
   git clone .\DSX++\
   ```
3. **Create a New Branch**: Always work on a new branch, giving it a descriptive name.
   ```sh
   git checkout -b new-feature-x
   ```
4. **Make Your Changes**: Develop and test your changes locally.
5. **Commit Your Changes**: Commit with a clear message describing your updates.
   ```sh
   git commit -m 'Implemented new feature x.'
   ```
6. **Push to LOCAL**: Push the changes to your forked repository.
   ```sh
   git push origin new-feature-x
   ```
7. **Submit a Pull Request**: Create a PR against the original project repository. Clearly describe the changes and their motivations.
8. **Review**: Once your PR is reviewed and approved, it will be merged into the main branch. Congratulations on your contribution!
</details>

---

##  License

This project is protected under the [MIT License](https://opensource.org/license/mit), refer to the [LICENSE](https://github.com/tpetsas/DSXpp/blob/main/LICENSE) file.

---

##  Acknowledgments

- [Paliverse](https://github.com/Paliverse) for making [DSX]
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI)

---
