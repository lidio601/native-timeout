//
// Created by Fabio Cigliano on 02/07/18.
//

#ifndef COMMON_H
#define COMMON_H

//#define DEBUG_MESSAGE

// Core
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <uv.h>
#include <unistd.h>

// Node.js deps
#include <node.h>
#include <v8.h>

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

using namespace v8;

/*
 * Thread message
 */
struct TMessage {
    // frame callback function
    Persistent<Function> callback;

    bool cancelled;

    double timeout;

    ~TMessage() {
        callback.Reset();
    }
};

//Define functions in scope
std::string stringValue(Local<Value> value);
float getticks();

#endif
