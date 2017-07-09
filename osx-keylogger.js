/* eslint-disable import/no-unresolved */
const fs = require('fs');

const keylogger = require('./build/Release/osx-keylogger');

const loggerModule = {
  keysDown: []
};

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

let currentModifiers = null;

function modifiersToKey() {
  return Object
    .keys(modifierStates)
    .reduce((keys, key) => {
      if (modifierStates[key] === 1) {
        if (modifiers[key]) {
          keys.push(modifiers[key]);
        } else {
          keys.push(key);
        }
      }
      return keys
    }, [])
    .join('+');
}

function processData(page, ...keyCodesParam) {
  if (page > -1) {
    keyCodesParam.forEach((code) => {
      modifierStates[page] = code;
    })
  } else {
    //keys held down now
    const keysDown = keyCodesParam.filter((code) => code !== 0);

    //compare to which keys used to be held down
    //to find which keys are new
    const newKeys = keyCodesParam.filter((code) => {
      if (code === 0) {
        return false;
      }
      return !loggerModule.keysDown.includes(code);
    }).forEach((code) => {
      //then check the new keys to find out whether or not
      //we have a match in the choosen key layout and return
      //as best we can
      const currentModifiers = modifiersToKey();
      const keyLayoutPart = loggerModule.keyLayout[currentModifiers];
      const keyLayoutFallback = loggerModule.keyLayout[''];

      if (keyLayoutPart && keyLayoutPart[code]) {
        loggerModule.callback(currentModifiers, keyLayoutPart[code]);
      } else if (keyLayoutFallback[code]) {
        //using fallback
        loggerModule.callback(currentModifiers, keyLayoutFallback[code]);
      } else {
        //not found
        loggerModule.callback(currentModifiers, `<${code}>`);
      }
    });
    loggerModule.keysDown = keysDown;
  }

}

loggerModule.listen = (callback, layoutPath) => {
  loggerModule.callback = callback;
  try {
    loggerModule.keyLayout = JSON.parse(fs.readFileSync(layoutPath));
  } catch(ex) {
    console.error('Can´t find or parse the key layout file');
    process.exit(1);
  }
  keylogger.listen(processData);
};

module.exports = loggerModule;
