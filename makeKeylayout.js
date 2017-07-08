/* eslint-disable no-console,import/no-unresolved */
const keypress = require('keypress');

const keylogger = require('./osx-keylogger');

const keyLayout = {};

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
  if (currentKeycode !== null && currentKeypress !== null) {
    if (!keyLayout[currentModifiers]) {
      keyLayout[currentModifiers] = {};
    }
    keyLayout[currentModifiers][currentKeycode] = currentKeypress;
    currentKeycode = null;
    currentKeypress= null;
    console.log(JSON.stringify(keyLayout, undefined, 2));
  }
}

keylogger.listen((page, keyCode) => {
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
