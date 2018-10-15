'use strict'

var native = require('../src')
var test = require('tape')

test('core setTimeout', { timeout: 1000 }, function (t) {
    console.log('using core setTimeout')
    setTimeout(function () {
        console.log('callback function called')
        t.assert(true)
        t.end()
    }, 500)
})

test('custom setTimeout', { timeout: 1000 }, function (t) {
    console.log('using custom setTimeout')
    native.setTimeout(function () {
        console.log('callback function called')
        t.assert(true)
        t.end()
    }, 500)
})
