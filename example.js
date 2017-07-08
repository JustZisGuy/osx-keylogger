/* eslint-disable no-console,import/no-unresolved */
const keylogger = require('./osx-keylogger');

//process.stdin.setRawMode(true);

keylogger.listen((usage, ...keys) => {
  console.log('Example: ', usage, keys);
});
