/* eslint-disable no-console,import/no-unresolved */
const keylogger = require('./build/Release/osx-keylogger');

// console.log(keylogger.pass_number(100));

keylogger.listen((event) => {
  console.log(event);
});