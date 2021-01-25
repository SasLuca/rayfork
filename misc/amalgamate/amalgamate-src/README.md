# Amalgamate
[![Build Status](https://travis-ci.org/rindeal/Amalgamate.svg?branch=master)](https://travis-ci.org/rindeal/Amalgamate)

A cross platform command line tool for producing an amalgamation of
C or C++ sources.

## What is an amalgamation?

An amalgamation is an alternate way to distribute a library's source code using
only a few files (as low as one or two). This tool produces an amalgamation by
inlining included files to create one or more large source or header files.

## How is this useful?

For libraries which are mature (i.e. do not change often) the amalgamated
distribution format is often both easier to integrate, and update. The
amalgmation is added as an additional source file to an existing project
rather than needing to be built separately by build tools. Here are some
examples of source code libraries that have been amalgamated:

- **[FreeType][1]** http://github.com/vinniefalco/FreeTypeAmalgam
- **[JUCE][2]** http://github.com/vinniefalco/JUCEAmalgam
- **[TagLib][3]** http://github.com/vinniefalco/TagLibAmalgam

## How to build

On linux simply running `make all` should be sufficient to build the program.

On windows you should use the `build.bat` script which requires the MSVC command line toolchain to be enabled. You can read about how to enable it [here](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019).

## Usage

```plain
  NAME

   amalgamate - produce an amalgamation of C/C++ source files.

  SYNOPSIS

   amalgamate [-s]
     [-w {wildcards}]
     [-f {file|macro}]...
     [-p {file|macro}]...
     [-d {name}={file}]...
     [-i {dir}]...
     {inputFile} {outputFile}

  DESCRIPTION

   Produces an amalgamation of {inputFile} by replacing #include statements with
   the contents of the file they refer to. This replacement will only occur if
   the file was located in the same directory, or one of the additional include
   paths added with the -i option.

   Files included in angle brackets (system includes) are only inlined if the
   -s option is specified.

   If an #include line contains a macro instead of a string literal, the list
   of definitions provided through the -d option is consulted to convert the
   macro into a string.

   A file will only be inlined once, with subsequent #include lines for the same
   file silently ignored, unless the -f option is specified for the file.

  OPTIONS

    -s                Process #include lines containing angle brackets (i.e.
                      system includes). Normally these are not inlined.

    -w {wildcards}    Specify a comma separated list of file name patterns to
                      match when deciding to inline (assuming the file can be
                      located). The default setting is "*.cpp;*.c;*.h;*.mm;*.m".

    -f {file|macro}   Force reinclusion of the specified file or macro on
                      all appearances in #include lines.

    -p {file|macro}   Prevent reinclusion of the specified file or macro on
                      subsequent appearances in #include lines.

    -d {name}={file}  Use {file} for macro {name} if it appears in an #include
                      line.

    -i {dir}          Additionally look in the specified directory for files when
                      processing #include lines.

    -v                Verbose output mode
```

## License

Copyright (C) 2012 [Vinnie Falco][4]<br>
Amalgamate is provided under the terms of the [MIT license][5].<br>
Amalgamate uses portions of [JUCE][2], licensed separately.

[1]: http://www.freetype.org "The FreeType Project"
[2]: http://rawmaterialsoftware.com/juce.php "JUCE"
[3]: http://developer.kde.org/~wheeler/taglib.html "TagLib"
[4]: http://vinniefalco.com "Vinnie Falco's Home Page"
[5]: http://www.opensource.org/licenses/MIT "MIT License"
