/* eslint-disable no-console,import/no-unresolved */
const keylogger = require('./osx-keylogger');

//process.stdin.setRawMode(true);

keylogger.listen((result) => {
  console.log('Example: ', result);
});
