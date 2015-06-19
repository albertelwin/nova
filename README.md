# Nova

## About

Nova is an experimental game about the formation and evolution of stars, written in C++.

Development status: **sketch**.

## Build Instructions

### Windows

You'll need [*MSVC*](https://www.visualstudio.com/), download and install it if you don't have it already.

Then simply run `build.bat`.

```
build.bat
```
And the .exe will be created in the `bin` directory, where you can run it. 

```
bin\nova
```

### Mac

You'll need `GLFW` which you can get via `brew`. You might need to first have the tap. Open up terminal and do the following.

```
brew tap homebrew/versions
brew install glfw3
```

You'll also need XCode command line tools, which you can get by installing XCode from the app store.

After that run the `bash script`

```
./build.sh
```

If you get a permission error change the permissions first.

```
chmod 755 build.sh
```

Once its built you can run it.

```
./nova
```