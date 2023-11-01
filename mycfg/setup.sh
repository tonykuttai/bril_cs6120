#!/bin/bash

# Project folder
project_folder="MyCfg"

# Create the project folder
mkdir $project_folder

# Change to the project directory
cd $project_folder

# Create main.cpp or other source files
touch main.cpp

# Initialize a basic CMakeLists.txt file for C++ (optional)
echo "cmake_minimum_required(VERSION 3.0)" > CMakeLists.txt
echo "project(MyCfg)" >> CMakeLists.txt
echo "add_executable(main main.cpp)" >> CMakeLists.txt

# Create .vscode folder if it doesn't exist
mkdir -p .vscode

# Create tasks.json for compiler build settings
echo '{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "build",
      "type": "shell",
      "command": "clang++",
      "args": ["-g", "main.cpp", "-o", "main"],
      "group": {
        "kind": "build",
        "isDefault": true
      }
    }
  ]
}' > .vscode/tasks.json

# Create launch.json for debugger settings
echo '{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "C++ Launch",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/main",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}",
      "environment": [],
      "externalConsole": false,
      "MIMode": "lldb",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ],
      "preLaunchTask": "build"
    }
  ]
}' > .vscode/launch.json

# Create c_cpp_properties.json for IntelliSense settings
echo '{
  "configurations": [
    {
      "name": "Mac",
      "includePath": ["${workspaceFolder}"],
      "defines": [],
      "macFrameworkPath": [
        "/System/Library/Frameworks",
        "/Library/Frameworks"
      ],
      "compilerPath": "/usr/bin/clang",
      "cStandard": "c17",
      "cppStandard": "c++14",
      "intelliSenseMode": "clang-x64"
    }
  ],
  "version": 4
}' > .vscode/c_cpp_properties.json
