/* eslint-disable no-console,import/no-unresolved */
const keylogger = require('./build/Release/osx-keylogger');

// console.log(keylogger.pass_number(100));

keylogger.listen((pressed, scancode) => {
  if (pressed === 1) {
    console.log(`Keypress ${pressed}, ${scancode}`);
  }
});
