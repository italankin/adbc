# adbc

`adbc` is an ADB facade which shows a device selection dialog, if multiple Android devices are connected.

To find a path to `adb` executable `adbc` uses `ANDROID_HOME` or `ANDROID_SDK_ROOT` environment variables.
If none of the above is defined, `adbc` will assume `adb` is available via `PATH`.

# Usage

## Running a command

```sh
$ adbc <adb command>
```

For example:

```sh
$ adbc shell dumpsys activity | grep mResumed
```

If there are multiple connected Android devices, the select dialog will be shown:

```
select a device:
  1: emulator-5554      device
  2: emulator-5556      device
  ... other devices ...
  q: quit
```

## Running with no arguments

With no arguments `adbc` will print selected device id. It can be used by scripts to prompt user to select a device, and then execute commands on the selected device.

For example, if we need to execute multiple commands on one device:

```sh
$ SELECTED_DEVICE=`adbc`
$ adb -s $SELECTED_DEVICE shell input text login
$ adb -s $SELECTED_DEVICE shell input keyevent TAB
$ adb -s $SELECTED_DEVICE shell input text password
$ adb -s $SELECTED_DEVICE shell input keyevent ENTER
```

## Print version

Run `adbc` with `--adbc-version` option:

```sh
$ adbc --adbc-version
adbc version: 1.0.1
Using adb path: /home/user/android-sdk/platform-tools/adb
Android Debug Bridge version 1.0.41
Version 29.0.5-5949299
Installed as /home/user/android-sdk/platform-tools/adb
```

# Building

## Requirements

* gcc
* ncurses

## Building executable

An executable can be built with this command:

```sh
$ make adbc
```

The executable will be created at `./build/adbc`.
