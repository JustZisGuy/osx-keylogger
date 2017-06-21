#include <nan.h>
#include <unistd.h>

#include <list>

typedef struct {
  uint32_t pressedFirst;
  uint32_t pressedSecond;
  uint32_t scancode;
} KeyEvent;

std::list<KeyEvent> keyEvents;

using namespace Nan;

#include <IOKit/hid/IOHIDValue.h>
#include <IOKit/hid/IOHIDManager.h>

void myHIDKeyboardCallback( void* context,  IOReturn result,  void* sender,  IOHIDValueRef value ) {
  IOHIDElementRef elem = IOHIDValueGetElement( value );
  if (IOHIDElementGetUsagePage(elem) != 0x07) {
    return;
  }
  uint32_t scancode = IOHIDElementGetUsage( elem );
  long pressed = IOHIDValueGetIntegerValue( value );
  uint32_t pressedFirst = pressed;
  uint32_t pressedSecond = (pressed>>32);
  KeyEvent event = {pressedFirst, pressedSecond, scancode};
  keyEvents.push_back(event);
}

CFMutableDictionaryRef myCreateDeviceMatchingDictionary( UInt32 usagePage,  UInt32 usage ) {
  CFMutableDictionaryRef dict = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0
    , & kCFTypeDictionaryKeyCallBacks
    , & kCFTypeDictionaryValueCallBacks );
  if ( ! dict ) {
    return NULL;
  }
  CFNumberRef pageNumberRef = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, & usagePage );
  if ( ! pageNumberRef ) {
    CFRelease( dict );
    return NULL;
  }
  CFDictionarySetValue( dict, CFSTR(kIOHIDDeviceUsagePageKey), pageNumberRef );
  CFRelease( pageNumberRef );
  CFNumberRef usageNumberRef = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, & usage );
  if ( ! usageNumberRef ) {
    CFRelease( dict );
    return NULL;
  }
  CFDictionarySetValue( dict, CFSTR(kIOHIDDeviceUsageKey), usageNumberRef );
  CFRelease( usageNumberRef );
  return dict;
}

class KeyloggerWorker : public AsyncProgressWorker {
 public:
  KeyloggerWorker(Callback *callback, Callback *progress)
    : AsyncProgressWorker(callback), progress(progress) {
      this->hidManager = IOHIDManagerCreate( kCFAllocatorDefault, kIOHIDOptionsTypeNone );
      CFArrayRef matches;
      {
        CFMutableDictionaryRef keyboard = myCreateDeviceMatchingDictionary( 0x01, 6 );
        CFMutableDictionaryRef keypad   = myCreateDeviceMatchingDictionary( 0x01, 7 );
        CFMutableDictionaryRef matchesList[] = { keyboard, keypad };
        matches = CFArrayCreate( kCFAllocatorDefault, (const void **)matchesList, 2, NULL );
      }
      IOHIDManagerSetDeviceMatchingMultiple( this->hidManager, matches );
    }

  ~KeyloggerWorker() {
    //TODO Cleanup
  }

  void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {
    IOHIDManagerRegisterInputValueCallback( this->hidManager, myHIDKeyboardCallback, NULL);
    IOHIDManagerScheduleWithRunLoop( this->hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode );
    IOHIDManagerOpen( this->hidManager, kIOHIDOptionsTypeNone );

    while(true) {
      CFRunLoopRunInMode(kCFRunLoopDefaultMode, 60, TRUE);
      while (!keyEvents.empty()) {
        KeyEvent event = keyEvents.front();
        progress.Send(reinterpret_cast<const char*>(&event), sizeof(KeyEvent));
        keyEvents.pop_front();
      }
    }
  }

  void HandleProgressCallback(const char *data, size_t size) {
    Nan::HandleScope scope;
    KeyEvent event = *reinterpret_cast<KeyEvent*>(const_cast<char*>(data));

    v8::Local<v8::Value> argv[] = {
      New<v8::Integer>(event.pressedFirst),
      New<v8::Integer>(event.pressedSecond),
      New<v8::Integer>(event.scancode)
    };
    progress->Call(3, argv);
  }

 private:
  Callback *progress;
  IOHIDManagerRef hidManager;
};

NAN_METHOD(Listen) {
  Callback *progress = new Callback(info[0].As<v8::Function>());
  Callback *callback = new Callback(info[0].As<v8::Function>());
  AsyncQueueWorker(new KeyloggerWorker(callback, progress));
}

NAN_MODULE_INIT(Init) {
  Set(target
    , New<v8::String>("listen").ToLocalChecked()
    , New<v8::FunctionTemplate>(Listen)->GetFunction());
}

NODE_MODULE(my_addon, Init)
