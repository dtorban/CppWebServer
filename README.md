# CellVis

## Getting Started

### Pre-requisites
 * JDK version between 8 and 11
 * Processing
 * CMake
 * (Windows) Visual Studio 17
 * Setup ssh keys for automatic dependency git downloads.
 
### Clone

```shell
git clone git@github.umn.edu:ivlab-cs/CellVis.git
cd CellVis
```

### Build

* Linux and Mac (command line)

  ```shell
  mkdir build
  cd build
  cmake ..
  make
  ```

* Windows (Visual Studio)

  ```shell
  mkdir build
  cd build
  cmake-gui ..
  # Make sure to change to Release and set JAVA_HOME (see screen shot below).
  # Configure
  # Generate
  # Open project
  # Change to Release Mode
  # Build All
  # Build INSTALL
  ```
  
  ![alt text](https://github.umn.edu/ivlab-cs/CellVis/blob/master/docs/WinCMakeCfg.PNG "Configuration using cmake-gui")
  
### Run
  
  * Open apps/CellViewer/CellViewer.pde in Processing and run.
