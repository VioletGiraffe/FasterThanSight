# Faster Than Sight

Extensively configurable speed reading software based on the [rapid serial visual representation (RSVP)](https://en.wikipedia.org/wiki/Rapid_serial_visual_presentation) method (and possibly other methods in the future).

[![Build status](https://ci.appveyor.com/api/projects/status/7vf29egkjcbi7uw1?svg=true)](https://ci.appveyor.com/project/VioletGiraffe/fasterthansight)

![Screenshot of FTS](http://i.imgur.com/dqiVw5K.png)

![Built with Qt](http://i.imgur.com/O7SUWxn.png)

### Currently supported platforms:
* Windows
* Mac OS X
* Linux

### Current status of the project:
See the issue #53 for up-to-date info on the status of the project and suggested goals for the near future.

### Known Issues
For the list of known issues, refer to the project issues on Github, sort by the "bug" label. Or just use <a href="https://github.com/VioletGiraffe/FasterThanSight/labels/bug">this link</a>.

### Reporting an issue
<a href="https://github.com/VioletGiraffe/FasterThanSight/issues/new">Create an issue</a> on the project's page on Github.

### Contributing

***Cloning the repository***

   The main git repository has submodules, so you need to execute the `update_repository` script (available as .bat for Windows and .sh for Linux / Mac) after cloning the project in order to clone the nested repositories. Subsequently, you can use the same `update_repository` script at any time to pull incoming changes to the main repo, as well as to all the subrepos, thus updating everything to the latest revision.

***Building***

* A compiler with C++ 0x/11 support is required (std::thread, lambda functions etc.).
* Qt 5.6 or newer required.
* Windows: you can build using either Qt Creator or Visual Studio for IDE. Visual Studio 2013 or newer is required - v120 toolset or newer. Run `qmake -tp vc -r` to generate the solution for Visual Studio. I have not tried building with MinGW, but it should work as long as you enable C++ 11 support.
* Linux: open the project file in Qt Creator and build it.
* Mac OS X: You can use either Qt Creator (simply open the project in it) or Xcode (run `qmake -r -spec macx-xcode` and open the Xcode project that's been generated).
