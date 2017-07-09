/* eslint-disable no-console,import/no-unresolved */
/*
  This program uses keypress along with the keylogger to
  make a rough start for a keylayout by trying all the
  different keys on a keyboard so remember to go through it
  and fill in the blanks afterwards
*/
if (process.argv.length !== 3) {
  console.error(
    'This program can only be run using: ' +
    'node makeKeylayout.js <json languagefile>'
  );
  process.exit(1);
}

const keypress = require('keypress');
const fs = require('fs');

const keylogger = require('./build/Release/osx-keylogger');

let keyLayout = {};

const layoutFile = process.argv[2];

try {
  keyLayout = JSON.parse(fs.readFileSync(layoutFile));
} catch(ex) {
  console.log('New file created');
}

const modifiers = {
  225: 'L_SHIFT',
  224: 'CTRL',
  227: 'META',
  226: 'ALT',
  230: 'ALT_GR',
  229: 'R_SHIFT',
};

const modifierStates = Object
  .keys(modifiers)
  .reduce((states, key) => {
    states[key] = 0;
    return states
  }, {});

let currentKeypress = null;
let currentKeycode = null;
let currentModifiers = null;

function modifiersToKey() {
  return Object
    .keys(modifiers)
    .reduce((keys, key) => {
      if (modifierStates[key] === 1) {
        keys.push(modifiers[key]);
      }
      return keys
    }, [])
    .join('+');
}

function checkKeyData() {
  if (currentKeycode && currentKeypress) {
    if (!keyLayout[currentModifiers]) {
      keyLayout[currentModifiers] = {};
    }
    keyLayout[currentModifiers][currentKeycode] = currentKeypress;
    currentKeycode = null;
    currentKeypress= null;
    fs.writeFileSync(
      layoutFile,
      JSON.stringify(keyLayout, undefined, 2)
    );
  }
}

keylogger.listen((page, ...keyCodesParam) => {
  const keysDown = keyCodesParam.filter((code) => code !== 0);

  if (keysDown.length>1) {
    console.error('One key at a time please, clearing data');
    currentKeycode = null;
    currentKeypress = null;
    return;
  }
  const keyCode = keysDown[0];

  if (page > -1) {
    modifierStates[page] = keyCode;
  } else {
    currentKeycode = keyCode;
    currentModifiers = modifiersToKey();
    checkKeyData();
  }
});

keypress(process.stdin);

// listen for the "keypress" event
process.stdin.on('keypress', function (ch, key) {
  if (ch) {
    currentKeypress = ch;
    checkKeyData();
  }
  if (key && key.ctrl && key.name === 'c') {
    process.stdin.pause();
    process.exit(0);
  }
});

process.stdin.setRawMode(true);
process.stdin.resume();
