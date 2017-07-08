/* eslint-disable no-console,import/no-unresolved */
const keylogger = require('./osx-keylogger');

//process.stdin.setRawMode(true);

keylogger.listen((usage, usagePage, value1, value2, value3, value4) => {
  console.log('Example: ', usage, usagePage, value1, value2, value3, value4);
});
