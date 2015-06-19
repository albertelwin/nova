# NOVA

## About

_todo_

## Build Instructions

### Windows

_todo_

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