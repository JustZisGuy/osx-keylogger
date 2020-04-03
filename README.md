# POC Keylogger for Mac OS X 10.5+

## Introduction
By using native bindings to IOKit we can talk directly with the keyboard. Unfortunately this means we have to provide our own key layouts but since we can talk directly with the hardware this should work regardless of which state the rest of the system is in.

### Help wanted
I've noticed that this is my most popular package so feel free to help out. Here is some examples of what I could use some help with:
- Figure out a way to use the active keyboard layout instead of a homemade one.
- Have a look at `osx-keylogger.cc` and suggest improvements(by creating issues) or make pull requests to improve it.
- Adding key layouts.

Anything missing or poorly designed, let me know. I built this as a POC and will only continue to work on it provided that I get feedback from the community.

## Last tested working version of OS X
macOS Catalina 10.15.3
### Note
Since the writing of this library OS X now has added security features to let the user allow/deny direct recording of keystrokes.

## Node working versions
When I tested this I found it to work for versions of node between 8 and 11

## Requirements
Building this package requires XCode to be installed for Catalina, I am unsure if that was needed for
previous versions of OSX.

## Usage example
```javascript
const keylogger = require('osx-keylogger');

// shows only the stuff from the keylogger(not stdin) but breaks ctrl+c
// process.stdin.setRawMode(true);

let currentModifiers = '';

keylogger.listen((modifiers, key) => {
  if (modifiers !== currentModifiers) {
    currentModifiers = modifiers;
    process.stdout.write(`[${modifiers}]`);
  }
  process.stdout.write(key);
}, 'swedish.json');

```
This example uses the swedish.json file as the key layout and will write the users input to stdout.

## makeKeylayout.js
By running the command
```
node makeKeylayout.js yourlanguage.json
```
And then using the console you are running it in to read what keys are pressed you can create your own language key layout. In this case it would create yourlanguage.json

## Installation
via npm
```
npm install osx-keylogger
```

## Tests
Sorry, I didn't feel like it. It's a POC

## Why?
I couldn't find a decent opensource keylogger for a modern version of OS X and I was looking for an excuse to try out native bindings.

## License
MIT, see LICENSE file
