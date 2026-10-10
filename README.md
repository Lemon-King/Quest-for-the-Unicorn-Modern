# Quest for the Unicorn
A repo mirror for the Rogue-like Game 'Quest for the Unicorn' by Michael H Riley.

[Archive.org Quest for the Unicorn Mirror](https://web.archive.org/web/20180730182147/http://www.elf-emulation.com/rcs/unicorn5/index.html)

## Build

Visual Studio 2026 (Desktop development with C++), Windows 10 or later, x64. From a Developer Command Prompt:

```bat
MSBuild Unicorn.slnx /m /p:Configuration=Release /p:Platform=x64
```

or open `Unicorn.slnx` and build. The game lands in `bin\Release\` together with `unicorn.cfg`, `unicorn.doc` and the
music; run it from there.

## Run

Start `bin\Release\unicorn.exe`. `unicorn.doc` is the manual: options, keys and game modes. Alt+Enter switches between
the window and fullscreen.

## Tests

```bat
bin\Release\platformtests.exe .
```

Run from the repo root, it checks the drawing, scaling, input and icon code against what they replaced. The golden-screen
check and the rest are described in `AGENTS.md` under Testing.

Copyright 1985-2014 by Michael H Riley
