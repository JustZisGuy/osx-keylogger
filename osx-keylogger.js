/* eslint-disable import/no-unresolved */
const fs = require('fs');

const keylogger = require('./build/Release/osx-keylogger');

module.exports = {
  listen(callback, layoutPath) {
    //TODO Use layout from layoutPath to simplify callback
    keylogger.listen(callback);
  }
};
