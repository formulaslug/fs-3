# How to setup an MbedOS project using only the mbed-tools CLI

Prerequisites:

- Python3
- CMake
- An editor that uses clangd for intellisense/autocompletion of c/c++
  - eg. VSCode with the c/c++ extension
  - Clion
  - vim/nvim with some lsp client installed and setup to use clangd

<!-- 1. `python -m venv .venv` -->
<!-- 1. `source .venv/bin/active` for Mac/Linux, `.\.venv\Scripts\activate` for -->
<!--    Windows -->

Open a terminal / command prompt window.

1. Run `pip install mbed-tools`
2. mbed-tools uses some optional libraries but it doesn't install them
   automatically, so run this too (optional): `pip install intelhex prettytable
future`
3. Run `mbed-tools` to make sure the cli is installed properly and available!
   You should see a help menu and list of commands printed out.
4. Run `mbed-tools init` to create a new project, or run `mbed-tools deploy` to
   setup mbed-os with an existing project (the project should have an
   mbed-os.lib file specifying which mbed-os version to deploy)
5. Run `mbed-tools configure -t GCC_ARM -m NUCLEO_L432KC` to set up the cmake
   build system for the NUCLEO_L432KC microcontroller (run this for as many
   MCU's as you want, it will create separate build directories in
   `./cmake_build/` for each one!)
   <!-- 1. Compile with cli to generate necessary build files (the `compile` flags are -->
   <!--    identical to the `configure` ones, with the addition of -f (flash) and -s -->
   <!--    (open serial terminal) which we will ignore this time): `mbed-tools compile -->
   <!--    -t GCC_ARM -m NUCLEO_L432KC` -->
6. Now run the build system manually with CMake to generate the
   compile_commands.json file (necessary for clangd to find mbed-os): `cmake -S
. -B cmake_build/NUCLEO_L432KC/develop/GCC_ARM -GNinja
-DCMAKE_EXPORT_COMPILE_COMMANDS=1`
7. To make clangd find the generated compile_commands.json file, there's two
   options:

   1. Create a file named `.clangd` in the project's root, in which you specify
      the path to the compile_commands.json file:
      ```txt
      CompileFlags:
        CompilationDatabase: ./cmake_build/NUCLEO_L432KC/develop/GCC_ARM
      ```
   2. Create a symlink (shortcut) to compile_commands.json in the project root.
      Run `ln -s cmake_build/NUCLEO_L432KC/develop/GCC_ARM/compile_commands.json
.` for MacOS/Linux, or create a shortcut manually in file exporer on
      Windows

8. Now, restart your editor. Clangd should have found and parsed your mbed-os
   configuration for your specified microcontroller! Eg. start typing pin
   aliases like PA_2 and wait for autocompletion to show up. Then
   go-to-definition on one and you should be brought into the mbed-os source
   code.
    
Steps 5-8 will have to be repeated any time you want to switch microcontrollers.
