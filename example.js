/* eslint-disable no-console,import/no-unresolved */
const keylogger = require('./osx-keylogger');

//shows only the stuff from the keylogger but breaks ctrl+c
//process.stdin.setRawMode(true);

currentModifiers = '';

keylogger.listen((modifiers, key) => {
  if (modifiers!==currentModifiers) {
    currentModifiers = modifiers;
    process.stdout.write(`[${modifiers}]`);
  }
  process.stdout.write(key);
}, 'swedish.json');
