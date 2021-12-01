Folder containing firmware for the robot.

-----
## Librairies Warning
For the moment, custom librairies like `cpp_intercom` are included by using the `lib_extra_dirs` PlatformIO configuration option, which doesn't parse the contents of the `library.json`.

It might result in missing dependencies; those should be added to the `lib_deps` option of the PlatformIO configuration (`platformio.ini`).

*Example when requiring `cpp_intercom`:*
```ini
lib_deps = bblanchon/ArduinoJson@^6.18.5
```
