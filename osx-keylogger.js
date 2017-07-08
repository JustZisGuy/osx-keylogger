/* eslint-disable import/no-unresolved */
const fs = require('fs');

const keylogger = require('./build/Release/osx-keylogger');

module.exports = {
  listen(callback) {
    keylogger.listen(callback);
  }
};
