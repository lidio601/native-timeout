//
// Created by Fabio Cigliano on 16/10/18.
//

#include "common.h"

uv_loop_t *loop;
uv_async_t async;

#define ELAPSED (getticks() - t)

/*
 * wait for the specified amount of seconds
 */
void asyncWork(uv_work_t* req) {
    TMessage* message = (TMessage*) req->data;

#ifdef DEBUG_MESSAGE
    float t = getticks();
    printf("thread :: asyncWork start waiting for %.2f seconds\n", message->timeout / 1000.0);
#endif

    // convert milli second to microseconds
    usleep(message->timeout * 1000);

#ifdef DEBUG_MESSAGE
    printf("thread :: asyncWork end wait after %.2f seconds\n", ELAPSED);
#endif

    if (async.type != UV_UNKNOWN_HANDLE) {
        async.data = message;
        uv_async_send(&async);
    }

#ifdef DEBUG_MESSAGE
    printf("thread :: asyncWork end after %.2f seconds\n", ELAPSED);
#endif
}

/*
 * - send an snapshot
 */
void asyncCallback(uv_async_t* req, int status)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    TMessage* message = (TMessage*) req->data;

#ifdef DEBUG_TIMES
    printf("thread :: calling callback function\n");
#endif

    Local<Value> argv[] = {};
    Local<Function> callBack = Local<Function>::New(isolate, message->callback);
    callBack->Call(isolate->GetCurrentContext()->Global(), 0, argv);

    if (async.type != UV_UNKNOWN_HANDLE && !uv_is_closing((uv_handle_t *) &async)) {
        uv_close((uv_handle_t *) &async, NULL);
    }
}

/*
 * - stop camera capture
 */
void asyncEnd(uv_work_t* req, int status) {
#ifdef DEBUG_MESSAGE
    printf("thread :: asyncEnd start\n");
#endif

    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
//    TMessage* message = (TMessage*) req->data;

    uv_loop_close(loop);

    delete req;

#ifdef DEBUG_MESSAGE
    printf("thread :: asyncEnd end\n");
#endif
}

void setTimeout(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

#ifdef DEBUG_TIMES
    float start = getticks();
#endif

    /*
     * Validate input argument
     */

    // Check if callback function is passed
    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument is missing: callback")));
        return;
    }

    // Check if timeout is passed
    if (args.Length() < 2) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Second argument is missing: timeout")));
        return;
    }

    // First parameter is callback, which must be a function
    if (!args[0]->IsFunction()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a function")));
        return;
    }

    // Second parameter is timeout, which must be a number
    if (!args[1]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Second argument must be a number")));
        return;
    }

    /*
     * Compose thread payload
     */

    TMessage *bag = new TMessage();
    bag->callback.Reset(isolate, Handle<Function>::Cast(args[0]));
    bag->timeout = args[1]->NumberValue();
    bag->cancelled = false;

    /*
     * compose the thread
     */

    loop = uv_default_loop();
    uv_work_t* req = new uv_work_t();
    req->data = bag;
    async = uv_async_t();
    uv_async_init(loop, &async, (uv_async_cb) asyncCallback);
    uv_queue_work(loop, req, asyncWork, (uv_after_work_cb) asyncEnd);

    /*
     * TODO: return timer handle
     */
    args.GetReturnValue().Set(Boolean::New(isolate, TRUE));
}

void init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    exports->Set(String::NewFromUtf8(isolate, "setTimeout"), FunctionTemplate::New(isolate, setTimeout)->GetFunction());
}

NODE_MODULE(camera, init);
