# adbc

`adbc` is an ADB facade which shows a device selection dialog, if multiple Android devices are connected.

To find a path to `adb` executable `adbc` uses `ANDROID_HOME` or `ANDROID_SDK_ROOT` environment variables.
If none of the above is defined, `adbc` will assume `adb` is available via `PATH`.

# Usage

```sh
$ adbc <adb command>
```

If there are multiple connected Android devices, the select dialog will be shown.

```sh
$ adbc
```

With no arguments `adbc` will print selected device id. It can be used by scripts to prompt user to select a device.

# Building

`adbc` requires `ncurses` library.

A binary is build with this command:

```sh
$ gcc adbc.c -lncurses -o adbc
```
